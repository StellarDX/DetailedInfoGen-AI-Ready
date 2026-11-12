/**
 * Host half of the InfoGen-DSH bundle.
 *
 * This row does two things and nothing else:
 *  - it watches whether any live Agent is bound to the `infogen-dsh` preset
 *    (so the reminder only exists after the user enables this Agent), probes the
 *    InfoGen MCP endpoint with a bounded retry budget, and publishes one
 *    notice after `attempts` consecutive failures;
 *  - it serves `GET /infogen-dsh/status` for the browser half, which renders the
 *    notice as a top-of-window toast.
 *
 * It never throws into the Host: probe failures are values, timers are unref'd,
 * and every registration is owned by this plugin's Cordis effect.
 */

const DEFAULTS = {
  mcpUrl: 'http://localhost:8000/mcp',
  presetId: 'infogen-dsh',
  attempts: 3,
  retryDelaysMs: [1000, 2000, 4000],
  probeTimeoutMs: 4000,
  pollIntervalMs: 10000,
};

/** HTTP statuses below this prove the endpoint answered; 5xx is treated as down. */
const UNHEALTHY_STATUS = 500;

function positiveNumber(value, fallback) {
  return typeof value === 'number' && Number.isFinite(value) && value > 0 ? value : fallback;
}

/**
 * Merge user configuration over the defaults, dropping unusable values.
 * @param {unknown} raw - row configuration from the Loader.
 * @returns the effective monitor configuration.
 */
export function resolveConfig(raw) {
  const value = raw !== null && typeof raw === 'object' ? raw : {};
  const retryDelaysMs = Array.isArray(value.retryDelaysMs)
    ? value.retryDelaysMs.filter(delay => typeof delay === 'number' && Number.isFinite(delay) && delay >= 0)
    : [];
  return {
    mcpUrl: typeof value.mcpUrl === 'string' && value.mcpUrl.length > 0 ? value.mcpUrl : DEFAULTS.mcpUrl,
    presetId: typeof value.presetId === 'string' && value.presetId.length > 0 ? value.presetId : DEFAULTS.presetId,
    attempts: Number.isInteger(value.attempts) && value.attempts > 0 ? value.attempts : DEFAULTS.attempts,
    retryDelaysMs: retryDelaysMs.length > 0 ? retryDelaysMs : DEFAULTS.retryDelaysMs,
    probeTimeoutMs: positiveNumber(value.probeTimeoutMs, DEFAULTS.probeTimeoutMs),
    pollIntervalMs: positiveNumber(value.pollIntervalMs, DEFAULTS.pollIntervalMs),
  };
}

/**
 * Probe the MCP endpoint without speaking the MCP protocol.
 *
 * A streamable-HTTP MCP endpoint answers any HTTP status below 500 while it is
 * listening (the InfoGen server replies 400 to a bare GET), so this is a cheap
 * liveness check that neither creates a session nor consumes a tool call.
 * @param {string} url - MCP endpoint URL.
 * @param {number} timeoutMs - per-probe budget in milliseconds.
 * @returns {Promise<{ ok: boolean, status: number, error?: string }>} the probe outcome.
 */
export async function probeEndpoint(url, timeoutMs) {
  try {
    const response = await fetch(url, {
      method: 'GET',
      headers: { accept: 'application/json, text/event-stream' },
      cache: 'no-store',
      signal: AbortSignal.timeout(timeoutMs),
    });
    return { ok: response.status < UNHEALTHY_STATUS, status: response.status };
  } catch (error) {
    return {
      ok: false,
      status: 0,
      error: error instanceof Error ? error.message : String(error),
    };
  }
}

/**
 * Install the monitor and its read-only status route.
 * @param {import('@deepseek-ai/cordis').Context} ctx - plugin context.
 * @param {unknown} rawConfig - row configuration.
 */
export function apply(ctx, rawConfig = {}) {
  const config = resolveConfig(rawConfig);

  /** @type {'idle' | 'checking' | 'up' | 'failed'} */
  let phase = 'idle';
  let attempts = 0;
  let lastError = '';
  let notice = null;
  let noticeSeq = 0;
  let timer;
  let running = false;
  let disposed = false;

  const presets = () => ctx.get('agentPresets');

  /**
   * Whether at least one live Agent currently runs this bundle's preset.
   * The reminder therefore stays silent until the user enables the Agent.
   * @returns true while the Agent is in use.
   */
  function agentInUse() {
    const registry = presets();
    if (registry === undefined || typeof registry.composedPreset !== 'function') return false;
    let agents;
    try {
      agents = ctx.agents.list();
    } catch {
      return false;
    }
    return agents.some(agent => {
      try {
        return registry.composedPreset(agent.ctx) === config.presetId;
      } catch {
        return false;
      }
    });
  }

  function clearTimer() {
    if (timer !== undefined) clearTimeout(timer);
    timer = undefined;
  }

  function schedule(delayMs = config.pollIntervalMs) {
    if (disposed) return;
    clearTimer();
    timer = setTimeout(() => { void check(); }, delayMs);
    timer.unref?.();
  }

  /** Coalesce lifecycle notifications into one imminent check. */
  function wake() {
    if (disposed) return;
    clearTimer();
    timer = setTimeout(() => { void check(); }, 200);
    timer.unref?.();
  }

  function statusPayload() {
    return {
      plugin: 'infogen-dsh',
      preset: config.presetId,
      mcpUrl: config.mcpUrl,
      state: phase,
      attempts,
      notice,
      ...(lastError === '' ? {} : { error: lastError }),
      updatedAt: Date.now(),
    };
  }

  async function check() {
    if (disposed || running) return;
    running = true;
    try {
      if (!agentInUse()) {
        phase = 'idle';
        attempts = 0;
        lastError = '';
        notice = null;
        return;
      }

      const wasFailed = phase === 'failed';
      phase = 'checking';

      for (let index = 0; index < config.attempts; index += 1) {
        if (disposed) return;
        const result = await probeEndpoint(config.mcpUrl, config.probeTimeoutMs);
        if (disposed) return;

        if (result.ok) {
          phase = 'up';
          attempts = 0;
          lastError = '';
          notice = null;
          return;
        }

        attempts = index + 1;
        lastError = result.error ?? `HTTP ${String(result.status)}`;

        // The user may disable the Agent during a retry round; stop quietly.
        if (!agentInUse()) {
          phase = 'idle';
          attempts = 0;
          lastError = '';
          notice = null;
          return;
        }

        if (attempts >= config.attempts) break;

        const delayMs = config.retryDelaysMs[Math.min(index, config.retryDelaysMs.length - 1)];
        if (delayMs > 0) await new Promise(resolve => setTimeout(resolve, delayMs));
        if (disposed) return;
      }

      phase = 'failed';
      // One notice per outage: a still-failing service does not re-announce.
      if (!wasFailed) {
        noticeSeq += 1;
        notice = { seq: noticeSeq, at: Date.now() };
      }
    } catch (error) {
      // The monitor must never crash the Host or the Agent.
      lastError = error instanceof Error ? error.message : String(error);
      phase = 'failed';
    } finally {
      running = false;
      schedule();
    }
  }

  ctx.effect(() => () => {
    disposed = true;
    clearTimer();
  }, 'infogen-dsh: monitor lifetime');

  ctx.on('agent/created', wake);
  ctx.on('agent/disposed', wake);

  // The preset registry may activate after this row; re-check once it exists.
  ctx.inject(['agentPresets'], () => wake());

  ctx.inject(['webServer'], (child) => {
    child.effect(() => child.webServer.register({
      kind: 'exact',
      path: '/infogen-dsh/status',
      resource: async () => new Response(JSON.stringify(statusPayload()), {
        status: 200,
        headers: {
          'content-type': 'application/json; charset=utf-8',
          'cache-control': 'no-store',
          'x-content-type-options': 'nosniff',
        },
      }),
    }), 'infogen-dsh: status route');
  });

  wake();
}
