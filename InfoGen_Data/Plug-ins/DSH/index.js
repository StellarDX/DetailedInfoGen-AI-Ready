/**
 * Host half of the InfoGen-DSH bundle.
 *
 * This row does three things and nothing else:
 *  - it owns the InfoGen MCP endpoint. The endpoint is stored in the local
 *    credential store under `INFOGEN_DSH_MCP_URL` (seeded from `mcpUrl` when it
 *    is absent), and the preset's `infogen-mcp` row references that same
 *    credential through `${credential:INFOGEN_DSH_MCP_URL}`. Rewriting the
 *    credential makes the MCP client rebuild its connection in place, so a new
 *    address takes effect for running sessions without a restart;
 *  - it watches whether any live Agent is bound to the `infogen-dsh` preset
 *    (so the reminder only exists after the user enables this Agent), probes
 *    the effective InfoGen MCP endpoint with a bounded retry budget, and
 *    publishes one notice after `attempts` consecutive failures;
 *  - it serves the routes the browser half uses:
 *      GET  /infogen-dsh/status  — read-only monitor state for the toast;
 *      GET  /infogen-dsh/config  — current endpoint, credential state, preset
 *                                  wiring, and an optional `?probe=1` probe;
 *      POST /infogen-dsh/config  — save a new endpoint (validated, bounded,
 *                                  same-origin only) and probe it.
 *
 * It never throws into the Host: probe and credential failures are values,
 * timers are unref'd, and every registration is owned by this plugin's Cordis
 * effect.
 */

const DEFAULTS = {
  mcpUrl: 'http://localhost:8000/mcp',
  presetId: 'infogen-dsh',
  attempts: 3,
  retryDelaysMs: [1000, 2000, 4000],
  probeTimeoutMs: 4000,
  pollIntervalMs: 10000,
};

/**
 * Credential reference that holds the MCP endpoint. The name is part of the
 * bundle contract: `cordis.patch.yml` references it in the preset's
 * `infogen-mcp` row, and the in-client settings page writes it.
 */
export const MCP_URL_CREDENTIAL = 'INFOGEN_DSH_MCP_URL';

/** Loader and preset identities this bundle owns. */
const PRESET_PLUGIN_NAME = '@deepseek-ai/dsh-agent-preset';
const MCP_PLUGIN_NAME = '@deepseek-ai/dsh-mcp-client';
const MCP_ROW_ID = 'infogen-mcp';
const MCP_SERVER_NAME = 'infogen';

/** Browser routes served by this plugin. */
const STATUS_PATH = '/infogen-dsh/status';
const CONFIG_PATH = '/infogen-dsh/config';

/** Request bounds: the config body is one URL, so anything larger is hostile. */
const MAX_BODY_BYTES = 16 * 1024;
const MAX_URL_LENGTH = 2048;

/** HTTP statuses below this prove the endpoint answered; 5xx is treated as down. */
const UNHEALTHY_STATUS = 500;

/** @returns true for a plain JSON object. */
function isRecord(value) {
  return value !== null && typeof value === 'object' && !Array.isArray(value);
}

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
 * Validate and trim one user-supplied MCP endpoint.
 *
 * Only absolute `http:`/`https:` URLs are accepted: the MCP client's
 * streamable-HTTP transport cannot use anything else, and rejecting credentials
 * in the authority keeps the value safe to show in the settings page.
 * @param {unknown} value - candidate address.
 * @returns the canonical absolute URL.
 * @throws {Error} when the candidate is not a usable endpoint.
 */
export function normalizeMcpUrl(value) {
  if (typeof value !== 'string') throw new Error('MCP address must be a string');
  const text = value.trim();
  if (text.length === 0) throw new Error('MCP address must not be empty');
  if (text.length > MAX_URL_LENGTH) throw new Error(`MCP address must be at most ${String(MAX_URL_LENGTH)} characters`);
  let parsed;
  try {
    parsed = new URL(text);
  } catch {
    throw new Error('MCP address must be an absolute URL, for example http://localhost:8000/mcp');
  }
  if (parsed.protocol !== 'http:' && parsed.protocol !== 'https:') {
    throw new Error('MCP address must start with http:// or https://');
  }
  if (parsed.hostname === '') throw new Error('MCP address must name a host');
  if (parsed.username !== '' || parsed.password !== '') {
    throw new Error('MCP address must not embed credentials; configure headers instead');
  }
  return parsed.href;
}

/** @returns whether `value` is an absolute http(s) URL, without throwing. */
function isValidHttpUrl(value) {
  try {
    normalizeMcpUrl(value);
    return true;
  } catch {
    return false;
  }
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

/** JSON response helper for the node:http handler route. */
function sendJson(res, status, payload) {
  res.statusCode = status;
  res.setHeader('content-type', 'application/json; charset=utf-8');
  res.setHeader('cache-control', 'no-store');
  res.setHeader('x-content-type-options', 'nosniff');
  res.end(JSON.stringify(payload));
}

/** Failure response in the one error envelope the browser half understands. */
function sendError(res, status, code, message) {
  sendJson(res, status, { error: { code, message } });
}

/** Collect a bounded request body as UTF-8 text; null past the ceiling. */
async function readBoundedBody(req) {
  const chunks = [];
  let size = 0;
  for await (const chunk of req) {
    size += chunk.byteLength;
    if (size > MAX_BODY_BYTES) {
      // Drain the remainder so the refusal is a readable response, not a socket cut.
      req.resume();
      return null;
    }
    chunks.push(chunk);
  }
  return Buffer.concat(chunks, size).toString('utf8');
}

/**
 * Install the monitor, the endpoint owner, and the browser routes.
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

  /** Endpoint the last read resolved; what the status reports. */
  let currentUrl = config.mcpUrl;
  /** Where `currentUrl` came from: a credential source name, or 'config'. */
  let urlSource = 'config';
  /** Endpoint the current outage evaluation belongs to. */
  let lastProbedUrl = config.mcpUrl;
  /** Set once seeding the credential proves impossible, so polls do not retry and log. */
  let seedSuspended = false;

  const presets = () => ctx.get('agentPresets');

  /** The optional credential provider; every operation checks it again. */
  function credentials() {
    const provider = ctx.get('credentials');
    return provider !== undefined && typeof provider.resolve === 'function' ? provider : undefined;
  }

  /**
   * Store the configured default so the preset's credential template resolves.
   * @param {object} provider - the credential provider.
   * @returns whether the value was stored.
   */
  async function seedDefault(provider) {
    if (seedSuspended || typeof provider.set !== 'function' || !isValidHttpUrl(config.mcpUrl)) return false;
    try {
      if (typeof provider.describe === 'function') {
        const info = await provider.describe(MCP_URL_CREDENTIAL);
        if (info !== undefined && info.writable !== true) {
          seedSuspended = true;
          return false;
        }
      }
      await provider.set(MCP_URL_CREDENTIAL, config.mcpUrl);
      return true;
    } catch (error) {
      seedSuspended = true;
      ctx.logger?.warn?.(`infogen-dsh: cannot store the default MCP address: ${String(error)}`);
      return false;
    }
  }

  /**
   * Read the endpoint the MCP client row would resolve right now.
   *
   * The credential seam is per-operation by contract, so this is re-read at
   * every probe and every settings GET; a changed value reaches the next read
   * without a restart. When the credential is absent, the configured `mcpUrl`
   * is the answer and `seed` stores it so the preset's template can resolve.
   * @param {{ seed?: boolean }} options - whether to store the configured default when absent.
   * @returns {Promise<{ url: string, source: string, configured: boolean }>} the effective endpoint.
   */
  async function readEndpoint(options = {}) {
    const provider = credentials();
    if (provider === undefined) return { url: config.mcpUrl, source: 'config', configured: false };
    let hit;
    try {
      hit = await provider.resolve(MCP_URL_CREDENTIAL);
    } catch {
      hit = undefined;
    }
    if (hit === undefined && options.seed === true && await seedDefault(provider)) {
      try {
        hit = await provider.resolve(MCP_URL_CREDENTIAL);
      } catch {
        hit = undefined;
      }
    }
    if (hit !== undefined && typeof hit.value === 'string' && hit.value !== '') {
      return {
        url: hit.value,
        source: typeof hit.source === 'string' ? hit.source : 'credential',
        configured: true,
      };
    }
    return { url: config.mcpUrl, source: 'config', configured: false };
  }

  /**
   * Whether the preset declaration still references the credential.
   *
   * A profile patch that replaces the preset's whole `config` can pin a plain
   * URL again; the settings page then warns instead of pretending the saved
   * address is used.
   * @returns the preset wiring facts for the settings page.
   */
  function inspectPreset() {
    const loader = ctx.get('loader');
    if (loader === undefined || typeof loader.entries !== 'function') return { available: false };
    const token = `\${credential:${MCP_URL_CREDENTIAL}}`;
    for (const entry of loader.entries()) {
      const options = entry?.options;
      if (!isRecord(options) || options.name !== PRESET_PLUGIN_NAME) continue;
      if (!isRecord(options.config) || options.config.id !== config.presetId) continue;
      const plugins = Array.isArray(options.config.plugins) ? options.config.plugins : [];
      const row = plugins.find(candidate => isRecord(candidate) && candidate.id === MCP_ROW_ID)
        ?? plugins.find(candidate => isRecord(candidate) && candidate.name === MCP_PLUGIN_NAME
          && isRecord(candidate.config) && candidate.config.serverName === MCP_SERVER_NAME);
      if (row === undefined) return { available: true, found: true, mcpRow: false };
      const rowConfig = isRecord(row.config) ? row.config : {};
      const url = typeof rowConfig.url === 'string' ? rowConfig.url : '';
      return {
        available: true,
        found: true,
        mcpRow: true,
        templated: url.includes(token),
        expandTemplates: rowConfig.expandTemplates === true,
        ...(url === '' ? {} : { url }),
      };
    }
    return { available: true, found: false };
  }

  /**
   * Read the endpoint that actually reaches the MCP client.
   *
   * Normally that is the credential read. A profile override may pin a plain
   * URL in the preset instead; that address then wins over the credential, so
   * the settings page (and the monitor) must report and probe the pinned one.
   * @returns {Promise<{ url: string, source: string, configured: boolean }>} the effective endpoint.
   */
  async function effectiveEndpoint() {
    // Always resolve and seed the credential first: it is the endpoint a later
    // override removal falls back to, and seeding keeps the template resolvable.
    const credentialEndpoint = await readEndpoint({ seed: true });
    const preset = inspectPreset();
    if (preset.found === true && preset.mcpRow === true && preset.templated === false
      && typeof preset.url === 'string' && isValidHttpUrl(preset.url)) {
      return { url: preset.url, source: 'preset', configured: false };
    }
    return credentialEndpoint;
  }

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
      mcpUrl: currentUrl,
      credentialRef: MCP_URL_CREDENTIAL,
      source: urlSource,
      state: phase,
      attempts,
      notice,
      ...(lastError === '' ? {} : { error: lastError }),
      updatedAt: Date.now(),
    };
  }

  /**
   * Full configuration snapshot for the in-client settings page.
   * @param {{ probe?: boolean }} options - whether to probe the current endpoint.
   * @returns the snapshot.
   */
  async function configPayload(options = {}) {
    const endpoint = await effectiveEndpoint();
    currentUrl = endpoint.url;
    urlSource = endpoint.source;

    let credential = { ref: MCP_URL_CREDENTIAL, configured: endpoint.configured, writable: false };
    let editable = false;
    let unavailableReason;
    const provider = credentials();
    if (provider === undefined) {
      unavailableReason = 'credentials-unavailable';
    } else if (typeof provider.describe === 'function') {
      try {
        const info = await provider.describe(MCP_URL_CREDENTIAL);
        credential = {
          ref: MCP_URL_CREDENTIAL,
          configured: info?.configured === true,
          writable: info?.writable === true,
          ...(typeof info?.source === 'string' ? { source: info.source } : {}),
        };
        editable = credential.writable;
        if (!editable) unavailableReason = 'credentials-read-only';
      } catch {
        unavailableReason = 'credentials-unavailable';
      }
    }

    const payload = {
      plugin: 'infogen-dsh',
      presetId: config.presetId,
      mcpUrl: endpoint.url,
      defaultMcpUrl: config.mcpUrl,
      source: endpoint.source,
      valid: isValidHttpUrl(endpoint.url),
      credential,
      editable,
      ...(unavailableReason === undefined ? {} : { unavailableReason }),
      preset: inspectPreset(),
      monitor: {
        state: phase,
        attempts,
        ...(lastError === '' ? {} : { error: lastError }),
      },
      updatedAt: Date.now(),
    };
    if (options.probe === true) payload.probe = await probeEndpoint(endpoint.url, config.probeTimeoutMs);
    return payload;
  }

  async function check() {
    if (disposed || running) return;
    running = true;
    try {
      const endpoint = await effectiveEndpoint();
      if (disposed) return;
      currentUrl = endpoint.url;
      urlSource = endpoint.source;
      if (endpoint.url !== lastProbedUrl) {
        // A new address deserves a fresh outage evaluation and a fresh notice.
        lastProbedUrl = endpoint.url;
        phase = 'idle';
        attempts = 0;
        lastError = '';
        notice = null;
      }

      if (!agentInUse()) {
        phase = 'idle';
        attempts = 0;
        lastError = '';
        notice = null;
        return;
      }

      const wasFailed = phase === 'failed';
      phase = 'checking';
      const url = endpoint.url;

      for (let index = 0; index < config.attempts; index += 1) {
        if (disposed) return;
        const result = await probeEndpoint(url, config.probeTimeoutMs);
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

  /**
   * Answer one untrusted request: the composition's connection fence rejects
   * foreign origins and unauthenticated peers before anything is read.
   * @returns true when the request was rejected.
   */
  function rejected(req, res) {
    const connection = ctx.get('connection');
    if (connection === undefined || typeof connection.requestRejection !== 'function') return false;
    const rejection = connection.requestRejection(req);
    if (rejection === undefined) return false;
    res.statusCode = rejection;
    res.end();
    return true;
  }

  /**
   * Handle the settings page's write: validate, store the credential, and
   * probe the address so the page can report the outcome immediately.
   */
  async function saveEndpoint(req, res) {
    const essence = String(req.headers['content-type'] ?? '').split(';', 1)[0]?.trim().toLowerCase();
    if (essence !== 'application/json') {
      sendError(res, 415, 'unsupported-media-type', 'content-type must be application/json');
      return;
    }
    let text;
    try {
      text = await readBoundedBody(req);
    } catch {
      sendError(res, 400, 'bad-request', 'request body is unreadable');
      return;
    }
    if (text === null) {
      sendError(res, 413, 'payload-too-large', 'request body is too large');
      return;
    }
    let body;
    try {
      body = JSON.parse(text);
    } catch {
      sendError(res, 400, 'bad-request', 'request body must be JSON');
      return;
    }
    if (!isRecord(body)) {
      sendError(res, 400, 'bad-request', 'request body must be a JSON object with "mcpUrl"');
      return;
    }
    let url;
    try {
      url = normalizeMcpUrl(body.mcpUrl);
    } catch (error) {
      sendError(res, 400, 'invalid-mcp-url', error instanceof Error ? error.message : String(error));
      return;
    }

    const provider = credentials();
    if (provider === undefined || typeof provider.set !== 'function') {
      sendError(res, 409, 'not-writable', 'This profile has no writable credential store, so the address cannot be saved');
      return;
    }
    try {
      const info = typeof provider.describe === 'function' ? await provider.describe(MCP_URL_CREDENTIAL) : undefined;
      if (info !== undefined && info.writable !== true) {
        sendError(res, 409, 'not-writable', 'The MCP address is not writable in this profile');
        return;
      }
      await provider.set(MCP_URL_CREDENTIAL, url);
    } catch (error) {
      sendError(res, 502, 'write-failed', `Cannot save the MCP address: ${error instanceof Error ? error.message : String(error)}`);
      return;
    }

    // The MCP client rebuilds from the credential change; this probe only tells
    // the settings page whether the new address answers right now.
    wake();
    const payload = await configPayload({ probe: true });
    sendJson(res, 200, { ok: true, ...payload });
  }

  ctx.effect(() => () => {
    disposed = true;
    clearTimer();
  }, 'infogen-dsh: monitor lifetime');

  ctx.on('agent/created', wake);
  ctx.on('agent/disposed', wake);

  // A changed endpoint — from this settings page, another surface, or an
  // external edit of the credential file — is probed immediately.
  ctx.on('credentials/reference-updated', (ref) => {
    if (String(ref) === MCP_URL_CREDENTIAL) wake();
  });

  // The preset registry and credential provider may activate after this row.
  ctx.inject(['agentPresets'], () => wake());
  ctx.inject(['credentials'], () => wake());

  ctx.inject(['webServer'], (child) => {
    child.effect(() => child.webServer.register({
      kind: 'exact',
      path: STATUS_PATH,
      resource: async () => new Response(JSON.stringify(statusPayload()), {
        status: 200,
        headers: {
          'content-type': 'application/json; charset=utf-8',
          'cache-control': 'no-store',
          'x-content-type-options': 'nosniff',
        },
      }),
    }), 'infogen-dsh: status route');

    child.effect(() => child.webServer.register({
      kind: 'exact',
      path: CONFIG_PATH,
      handler: async (req, res) => {
        try {
          if (rejected(req, res)) return;
          if (req.method === 'GET') {
            let probe = false;
            try {
              probe = new URL(String(req.url), 'http://localhost').searchParams.get('probe') === '1';
            } catch {
              probe = false;
            }
            sendJson(res, 200, await configPayload({ probe }));
            return;
          }
          if (req.method === 'POST') {
            await saveEndpoint(req, res);
            return;
          }
          res.statusCode = 405;
          res.setHeader('allow', 'GET, POST');
          res.end();
        } catch (error) {
          // The carrier would answer a bare 400 for a rejected handler; keep the
          // settings page's error envelope instead.
          ctx.logger?.warn?.(`infogen-dsh: config route failed: ${String(error)}`);
          if (res.headersSent) {
            res.destroy();
            return;
          }
          sendError(res, 500, 'internal-error', error instanceof Error ? error.message : String(error));
        }
      },
    }), 'infogen-dsh: config route');
  });

  wake();
}
