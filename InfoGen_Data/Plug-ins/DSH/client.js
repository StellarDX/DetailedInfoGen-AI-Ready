/**
 * Browser half of InfoGen-DSH.
 *
 * It contributes two things:
 *  - a settings page ("InfoGen-DSH") where the MCP endpoint is edited in the
 *    app. The page reads and writes `/infogen-dsh/config`; the Host stores the
 *    address in the local credential store, which makes every running MCP
 *    client rebuild its connection in place;
 *  - at most one top-of-window toast. The Host monitor only publishes a notice
 *    after the user has enabled the Agent and the MCP endpoint has failed
 *    `attempts` consecutive probes, so an ordinary session never sees it.
 */
window.__ModuleLoader__.load({
  id: 'infogen-dsh',
  factory(require) {
    const React = require('react');
    const h = React.createElement;
    const NS = 'infogen-dsh';

    const STATUS_URL = '/infogen-dsh/status';
    const CONFIG_URL = '/infogen-dsh/config';
    const POLL_MS = 4000;
    const INPUT_ID = 'infogen-dsh-mcp-url';

    const zh = {
      'section.nav': 'InfoGen-DSH',
      'section.title': 'InfoGen MCP 连接',
      'section.intro': '在这里修改 InfoGen MCP 服务的连接地址。地址保存在本机凭据存储中；保存后，正在运行的 InfoGen-DSH 会话会自动重连到新地址，无需重启 DSH。',
      'field.label': 'MCP 服务地址',
      'field.placeholder': 'http://localhost:8000/mcp',
      'field.hint': '支持 http:// 和 https:// 地址，例如 http://localhost:8000/mcp 或 http://127.0.0.1:8000/mcp。',
      'action.save': '保存地址',
      'action.saving': '保存中…',
      'action.probe': '检测连接',
      'action.probing': '检测中…',
      'action.reset': '恢复默认',
      'status.loading': '正在读取配置…',
      'status.saved': '已保存：{url}',
      'error.load': '读取配置失败：{error}',
      'error.invalid': '地址无效：请输入以 http:// 或 https:// 开头的完整地址。',
      'error.save': '保存失败：{error}',
      'probe.ok': '连接正常（HTTP {status}）。',
      'probe.fail': '无法连接：{error}',
      'probe.run': '检测失败：{error}',
      'meta.effective': '当前生效：{url}',
      'source.file': '来源：本机凭据存储',
      'source.env': '来源：环境变量 {ref}',
      'source.config': '来源：插件配置 mcpUrl',
      'source.preset': '来源：预设中固定的地址（端内保存未生效）',
      'monitor.idle': '当前没有会话使用 InfoGen-DSH 预设，后台未探测服务。',
      'monitor.checking': '正在探测 MCP 服务…',
      'monitor.up': 'MCP 服务可达。',
      'monitor.failed': 'MCP 服务不可达：{error}',
      'readonly': '当前 profile 没有可写的凭据服务，无法在端内保存地址。',
      'override.warning': '注意：profile 覆盖了预设里的 MCP 行，端内保存的地址可能不会生效；请删除 profile 补丁中对该预设 MCP 地址的自定义覆盖。',
      'preset.missing': '没有找到 InfoGen-DSH 预设行：地址仍会保存，但不会建立 MCP 连接。',
      'note.storage': '地址保存为凭据 {ref}，修改后会立即重连当前会话。',
      'toast.unavailable': 'InfoGen MCP 服务（{url}）不可达，连续 {count} 次探测失败。请启动 InfoGen MCP 服务，Agent 会在后台自动恢复。',
    };
    const en = {
      'section.nav': 'InfoGen-DSH',
      'section.title': 'InfoGen MCP connection',
      'section.intro': 'Change the InfoGen MCP service address here. The address is stored in the local credential store; saving it reconnects running InfoGen-DSH sessions to the new address without restarting DSH.',
      'field.label': 'MCP service address',
      'field.placeholder': 'http://localhost:8000/mcp',
      'field.hint': 'Use an http:// or https:// address, such as http://localhost:8000/mcp or http://127.0.0.1:8000/mcp.',
      'action.save': 'Save address',
      'action.saving': 'Saving…',
      'action.probe': 'Test connection',
      'action.probing': 'Testing…',
      'action.reset': 'Use default',
      'status.loading': 'Loading configuration…',
      'status.saved': 'Saved: {url}',
      'error.load': 'Could not load configuration: {error}',
      'error.invalid': 'Invalid address: enter a full http:// or https:// URL.',
      'error.save': 'Could not save: {error}',
      'probe.ok': 'Connection works (HTTP {status}).',
      'probe.fail': 'Cannot connect: {error}',
      'probe.run': 'Connection test failed: {error}',
      'meta.effective': 'In effect: {url}',
      'source.file': 'Source: local credential store',
      'source.env': 'Source: environment variable {ref}',
      'source.config': 'Source: plugin configuration mcpUrl',
      'source.preset': 'Source: address pinned in the preset (saved value not in effect)',
      'monitor.idle': 'No session uses the InfoGen-DSH preset, so the service is not being probed.',
      'monitor.checking': 'Probing the MCP service…',
      'monitor.up': 'The MCP service is reachable.',
      'monitor.failed': 'The MCP service is unreachable: {error}',
      'readonly': 'This profile has no writable credential store, so the address cannot be saved here.',
      'override.warning': 'Note: the profile overrides the preset MCP row, so the address saved here may not take effect. Remove that override from the profile patch.',
      'preset.missing': 'No InfoGen-DSH preset row was found: the address is saved, but no MCP connection will be created.',
      'note.storage': 'The address is stored as credential {ref}; changing it reconnects running sessions immediately.',
      'toast.unavailable': 'The InfoGen MCP service ({url}) is unreachable after {count} consecutive probes. Start the InfoGen MCP service; the agent reconnects automatically.',
    };

    /** Settings-section styles, kept beside the component so unmounting removes them. */
    const SETTINGS_CSS = `
      .igs{display:flex;flex-direction:column;gap:12px;max-width:720px;color:var(--dsw-alias-label-primary,#f4f6fb)}
      .igs-title{margin:0;font-size:18px;font-weight:600;line-height:1.4}
      .igs-intro{margin:0;font-size:13px;line-height:1.6;color:var(--dsw-alias-label-tertiary,#9aa3b2)}
      .igs-card{display:flex;flex-direction:column;gap:12px;padding:16px;border:0.5px solid var(--dsw-alias-settings-card-stroke,var(--dsw-alias-border-l1,#343a46));border-radius:var(--dsw-radius-xl,12px);background:var(--dsw-alias-settings-card-fill,var(--dsw-alias-bg-layer-1,#1b1e24))}
      .igs-label{font-size:12px;font-weight:600;letter-spacing:.06em;text-transform:uppercase;color:var(--dsw-alias-label-tertiary,#9aa3b2)}
      .igs-field{display:flex;align-items:center;gap:8px;flex-wrap:wrap}
      .igs-input{flex:1 1 320px;min-width:0;height:32px;box-sizing:border-box;padding:0 8px;border:0.5px solid var(--dsw-alias-border-l4,var(--dsw-alias-border-l3,#3a3f4b));border-radius:var(--dsw-radius-md,8px);background:var(--dsw-alias-bg-base,#111418);color:var(--dsw-alias-label-primary,#f4f6fb);font-size:14px;line-height:22px;font-family:inherit;outline:none}
      .igs-input:focus{border-color:var(--dsw-alias-state-business-primary,#4b8dff)}
      .igs-input:disabled{opacity:.55}
      .igs-input::placeholder{color:var(--dsw-alias-label-dimmed,#6b7482)}
      .igs-button{box-sizing:border-box;display:inline-flex;align-items:center;justify-content:center;gap:4px;height:32px;padding:0 14px;border:none;border-radius:var(--dsw-radius-md,8px);font:inherit;font-size:14px;line-height:22px;cursor:pointer;background:transparent;color:var(--dsw-alias-label-primary,#f4f6fb);white-space:nowrap}
      .igs-button:disabled{cursor:not-allowed;opacity:.4}
      .igs-button:focus-visible{outline:2px solid var(--dsw-focus-ring-color,var(--dsw-alias-state-business-primary,#4b8dff));outline-offset:2px}
      .igs-primary{background:var(--dsw-alias-button-primary-fill,var(--dsw-alias-brand-primary,#3b6ff5));color:var(--dsw-alias-label-primary-foreground,#fff)}
      .igs-ghost{box-shadow:inset 0 0 0 0.5px var(--dsw-alias-border-l3,#3a3f4b)}
      @media (hover:hover){.igs-primary:hover:not(:disabled){background:var(--dsw-alias-button-primary-hover,var(--dsw-alias-brand-primary,#3b6ff5))}.igs-ghost:hover:not(:disabled){background:var(--dsw-alias-interactive-bg-hover,#262b34)}}
      .igs-hint{margin:0;font-size:12px;line-height:1.5;color:var(--dsw-alias-label-tertiary,#9aa3b2)}
      .igs-meta{display:flex;flex-wrap:wrap;gap:4px 16px;font-size:12px;line-height:1.5;color:var(--dsw-alias-label-tertiary,#9aa3b2)}
      .igs-mono{font-family:var(--dsw-font-mono,ui-monospace,SFMono-Regular,Menlo,monospace);word-break:break-all}
      .igs-warn{margin:0;padding:8px 10px;border:0.5px solid var(--dsw-alias-state-warn-primary,#d29922);border-radius:var(--dsw-radius-md,8px);font-size:12px;line-height:1.55;color:var(--dsw-alias-state-warn-primary,#d29922)}
      .igs-status{display:flex;align-items:center;gap:8px;font-size:13px;line-height:1.5;color:var(--dsw-alias-label-secondary,#c3cad6)}
      .igs-dot{flex:none;width:8px;height:8px;border-radius:50%;background:var(--dsw-alias-state-idle-primary,#6b7482)}
      .igs-dot-up{background:var(--dsw-alias-state-success-primary,#3fb950)}
      .igs-dot-failed{background:var(--dsw-alias-state-error-primary,#f85149)}
      .igs-dot-checking{background:var(--dsw-alias-state-warn-primary,#d29922)}
      .igs-message{margin:0;font-size:13px;line-height:1.5;overflow-wrap:anywhere}
      .igs-ok{color:var(--dsw-alias-state-success-primary,#3fb950)}
      .igs-error{color:var(--dsw-alias-state-error-primary,#f85149)}
      .igs-actions{display:flex;flex-wrap:wrap;gap:8px;justify-content:flex-end}
      .igs-note{margin:0;font-size:12px;line-height:1.5;color:var(--dsw-alias-label-tertiary,#9aa3b2)}
    `;

    /** The settings-section stylesheet element; React removes it on unmount. */
    function SettingsStyle() {
      return h('style', { 'data-infogen-dsh': 'settings' }, SETTINGS_CSS);
    }

    function WarningIcon() {
      return h('svg', {
        viewBox: '0 0 24 24',
        width: 16,
        height: 16,
        fill: 'none',
        stroke: 'currentColor',
        strokeWidth: 2,
        strokeLinecap: 'round',
        strokeLinejoin: 'round',
        'aria-hidden': true,
      }, h('path', { d: 'M12 9v4' }), h('path', { d: 'M12 17h.01' }),
      h('path', { d: 'M10.3 3.9 1.8 18a2 2 0 0 0 1.7 3h17a2 2 0 0 0 1.7-3L13.7 3.9a2 2 0 0 0-3.4 0Z' }));
    }

    /** Read the settings snapshot (optionally with a fresh probe). */
    async function fetchConfig(probe) {
      const response = await fetch(probe === true ? `${CONFIG_URL}?probe=1` : CONFIG_URL, {
        cache: 'no-store',
        credentials: 'same-origin',
      });
      if (!response.ok) {
        let detail = `HTTP ${String(response.status)}`;
        try {
          const body = await response.json();
          if (body !== null && typeof body === 'object' && typeof body.error?.message === 'string') detail = body.error.message;
        } catch {
          // Keep the status text; the body was not the JSON envelope.
        }
        throw new Error(detail);
      }
      return response.json();
    }

    /** Persist one address and return the refreshed snapshot. */
    async function saveConfig(mcpUrl) {
      const response = await fetch(CONFIG_URL, {
        method: 'POST',
        cache: 'no-store',
        credentials: 'same-origin',
        headers: { 'content-type': 'application/json' },
        body: JSON.stringify({ mcpUrl }),
      });
      let body = null;
      try {
        body = await response.json();
      } catch {
        // Fall through to the status code when the body is not JSON.
      }
      if (!response.ok) {
        const message = body !== null && typeof body === 'object' && typeof body.error?.message === 'string'
          ? body.error.message
          : `HTTP ${String(response.status)}`;
        throw new Error(message);
      }
      return body;
    }

    /** Client-side shape check; the Host validates the same rules authoritatively. */
    function addressProblem(value) {
      if (typeof value !== 'string' || value.trim() === '') return 'empty';
      try {
        const parsed = new URL(value.trim());
        if (parsed.protocol !== 'http:' && parsed.protocol !== 'https:') return 'protocol';
        if (parsed.hostname === '') return 'host';
      } catch {
        return 'invalid';
      }
      return null;
    }

    /**
     * The in-client entry for the MCP address: read, validate, save, probe.
     * @param {{ t: (key: string, params?: object) => string }} props - slot locale seat.
     */
    function InfoGenSettingsSection({ t }) {
      const translate = typeof t === 'function' ? t : (key, params) => {
        const template = zh[key] ?? en[key] ?? key;
        return params === undefined
          ? template
          : template.replace(/\{(\w+)\}/g, (match, name) => (name in params ? String(params[name]) : match));
      };
      const [snapshot, setSnapshot] = React.useState(null);
      const [draft, setDraft] = React.useState('');
      const [dirty, setDirty] = React.useState(false);
      const [busy, setBusy] = React.useState(null);
      const [message, setMessage] = React.useState(null);
      const [probe, setProbe] = React.useState(null);
      const [loadError, setLoadError] = React.useState(null);
      const dirtyRef = React.useRef(false);
      const mountedRef = React.useRef(true);

      React.useEffect(() => {
        mountedRef.current = true;
        return () => {
          mountedRef.current = false;
        };
      }, []);

      // Poll the snapshot while the page is open: monitor state and an address
      // changed elsewhere stay current. The draft is never overwritten while it
      // holds unsaved edits.
      React.useEffect(() => {
        let cancelled = false;
        let timer;
        const load = async () => {
          try {
            const next = await fetchConfig(false);
            if (cancelled) return;
            setSnapshot(next);
            setLoadError(null);
            if (!dirtyRef.current) setDraft(typeof next.mcpUrl === 'string' ? next.mcpUrl : '');
          } catch (error) {
            if (!cancelled) setLoadError(error instanceof Error ? error.message : String(error));
          }
          if (!cancelled) timer = setTimeout(() => { void load(); }, POLL_MS);
        };
        void load();
        return () => {
          cancelled = true;
          clearTimeout(timer);
        };
      }, []);

      const updateDraft = (value) => {
        setDraft(value);
        const nextDirty = value !== (snapshot?.mcpUrl ?? '');
        dirtyRef.current = nextDirty;
        setDirty(nextDirty);
      };

      const save = async () => {
        const value = draft.trim();
        if (addressProblem(value) !== null) {
          setMessage({ kind: 'error', text: translate('error.invalid') });
          return;
        }
        setBusy('save');
        setMessage(null);
        try {
          const next = await saveConfig(value);
          if (!mountedRef.current) return;
          const saved = typeof next?.mcpUrl === 'string' ? next.mcpUrl : value;
          setSnapshot(next);
          setDraft(saved);
          dirtyRef.current = false;
          setDirty(false);
          setProbe(next?.probe ?? null);
          setMessage({ kind: 'ok', text: translate('status.saved', { url: saved }) });
        } catch (error) {
          if (mountedRef.current) {
            setMessage({ kind: 'error', text: translate('error.save', { error: error instanceof Error ? error.message : String(error) }) });
          }
        } finally {
          if (mountedRef.current) setBusy(null);
        }
      };

      const runProbe = async () => {
        setBusy('probe');
        setMessage(null);
        try {
          const next = await fetchConfig(true);
          if (!mountedRef.current) return;
          setSnapshot(next);
          setProbe(next?.probe ?? null);
        } catch (error) {
          if (mountedRef.current) {
            setMessage({ kind: 'error', text: translate('probe.run', { error: error instanceof Error ? error.message : String(error) }) });
          }
        } finally {
          if (mountedRef.current) setBusy(null);
        }
      };

      if (snapshot === null) {
        return h('section', { className: 'igs', 'aria-busy': true },
          h(SettingsStyle),
          h('p', { className: 'igs-intro' }, loadError === null
            ? translate('status.loading')
            : translate('error.load', { error: loadError })));
      }

      const monitor = snapshot.monitor ?? {};
      const credential = snapshot.credential ?? {};
      const state = typeof monitor.state === 'string' ? monitor.state : 'idle';
      const readOnly = snapshot.editable !== true;
      const defaultUrl = typeof snapshot.defaultMcpUrl === 'string' ? snapshot.defaultMcpUrl : '';
      const effectiveUrl = typeof snapshot.mcpUrl === 'string' ? snapshot.mcpUrl : '';
      const source = typeof snapshot.source === 'string' ? snapshot.source : 'config';
      const sourceText = source === 'preset'
        ? translate('source.preset')
        : source === 'config'
          ? translate('source.config')
          : translate(source === 'env' ? 'source.env' : 'source.file', { ref: credential.ref ?? '' });
      const monitorText = state === 'up'
        ? translate('monitor.up')
        : state === 'failed'
          ? translate('monitor.failed', { error: monitor.error ?? '' })
          : state === 'checking'
            ? translate('monitor.checking')
            : translate('monitor.idle');
      const preset = snapshot.preset ?? {};
      const overrideWarning = preset.found === true && preset.mcpRow === true && preset.templated === false;
      const presetMissing = preset.available === true && preset.found === false;
      const probeText = probe === null
        ? null
        : probe.ok === true
          ? translate('probe.ok', { status: String(probe.status ?? '?') })
          : translate('probe.fail', { error: probe.error ?? `HTTP ${String(probe.status ?? 0)}` });

      return h('section', { className: 'igs' },
        h(SettingsStyle),
        h('h2', { className: 'igs-title' }, translate('section.title')),
        h('p', { className: 'igs-intro' }, translate('section.intro')),
        h('div', { className: 'igs-card' },
          h('label', { className: 'igs-label', htmlFor: INPUT_ID }, translate('field.label')),
          h('div', { className: 'igs-field' },
            h('input', {
              id: INPUT_ID,
              className: 'igs-input',
              type: 'url',
              inputMode: 'url',
              spellCheck: false,
              autoComplete: 'off',
              placeholder: translate('field.placeholder'),
              value: draft,
              disabled: readOnly || busy === 'save',
              onChange: event => { updateDraft(event.target.value); },
              onKeyDown: event => {
                if (event.key === 'Enter' && !readOnly && busy === null) {
                  event.preventDefault();
                  void save();
                }
              },
            }),
            h('button', {
              type: 'button',
              className: 'igs-button igs-primary',
              disabled: readOnly || busy !== null || !dirty,
              onClick: () => { void save(); },
            }, busy === 'save' ? translate('action.saving') : translate('action.save'))),
          h('p', { className: 'igs-hint' }, translate('field.hint')),
          h('div', { className: 'igs-meta' },
            h('span', null, translate('meta.effective', { url: effectiveUrl })),
            h('span', null, sourceText)),
          readOnly ? h('p', { className: 'igs-warn' }, translate('readonly')) : null,
          overrideWarning ? h('p', { className: 'igs-warn' }, translate('override.warning')) : null,
          presetMissing ? h('p', { className: 'igs-warn' }, translate('preset.missing')) : null,
          h('div', { className: 'igs-status', role: 'status' },
            h('span', { className: `igs-dot igs-dot-${state}` }),
            h('span', null, monitorText)),
          probeText === null ? null : h('p', { className: `igs-message ${probe?.ok === true ? 'igs-ok' : 'igs-error'}` }, probeText),
          message === null ? null : h('p', { className: `igs-message ${message.kind === 'error' ? 'igs-error' : 'igs-ok'}` }, message.text),
          h('div', { className: 'igs-actions' },
            h('button', {
              type: 'button',
              className: 'igs-button igs-ghost',
              disabled: busy !== null || readOnly,
              onClick: () => { void runProbe(); },
            }, busy === 'probe' ? translate('action.probing') : translate('action.probe')),
            h('button', {
              type: 'button',
              className: 'igs-button igs-ghost',
              disabled: busy !== null || readOnly || defaultUrl === '' || draft === defaultUrl,
              onClick: () => { updateDraft(defaultUrl); },
            }, translate('action.reset')))),
        h('p', { className: 'igs-note' }, translate('note.storage', { ref: credential.ref ?? '' })));
    }

    function InfoGenToast({ t }) {
      const [notice, setNotice] = React.useState(null);
      const [visible, setVisible] = React.useState(false);
      const lastAt = React.useRef(0);

      React.useEffect(() => {
        let cancelled = false;
        let timer;
        const poll = async () => {
          try {
            const response = await fetch(STATUS_URL, { cache: 'no-store' });
            if (response.ok) {
              const status = await response.json();
              const next = status && status.notice;
              if (next && Number(next.at) > lastAt.current) {
                lastAt.current = Number(next.at);
                setNotice({ ...next, mcpUrl: status.mcpUrl, attempts: status.attempts });
              }
            }
          } catch {
            // The Host may be restarting; the next poll retries silently.
          }
          if (!cancelled) timer = setTimeout(poll, 4000);
        };
        void poll();
        return () => {
          cancelled = true;
          clearTimeout(timer);
        };
      }, []);

      React.useEffect(() => {
        if (notice === null) return undefined;
        setVisible(false);
        const show = setTimeout(() => setVisible(true), 20);
        const fade = setTimeout(() => setVisible(false), 7000);
        const clear = setTimeout(() => setNotice(null), 7800);
        return () => {
          clearTimeout(show);
          clearTimeout(fade);
          clearTimeout(clear);
        };
      }, [notice]);

      if (notice === null) return null;

      const params = { url: notice.mcpUrl ?? '', count: notice.attempts ?? 3 };
      const text = typeof t === 'function'
        ? t('toast.unavailable', params)
        : zh['toast.unavailable'].replace('{url}', String(params.url)).replace('{count}', String(params.count));

      return h('div', {
        key: String(notice.seq),
        role: 'alert',
        style: {
          position: 'fixed',
          top: '40px',
          left: '50%',
          transform: 'translateX(-50%)',
          zIndex: 1100,
          pointerEvents: 'none',
          display: 'flex',
          alignItems: 'center',
          gap: '10px',
          width: 'max-content',
          maxWidth: 'min(640px, calc(100vw - 48px))',
          padding: '12px 16px',
          borderRadius: 'var(--dsw-radius-lg, 10px)',
          background: 'var(--dsw-alias-toast-bg, #2b2f38)',
          color: 'var(--dsw-alias-toast-label, #f4f6fb)',
          fontSize: '14px',
          lineHeight: '22px',
          boxShadow: 'var(--dsw-shadow-lv3, 0 8px 24px rgba(0, 0, 0, 0.28))',
          opacity: visible ? 1 : 0,
          transition: 'opacity 600ms ease',
        },
      },
      h('span', {
        style: {
          display: 'grid',
          placeItems: 'center',
          flex: 'none',
          color: 'var(--dsw-alias-state-warn-label, #f3b64b)',
        },
      }, h(WarningIcon)),
      h('span', { style: { minWidth: 0 } }, text));
    }

    return {
      inject: ['slots', 'locale'],
      apply(ctx) {
        ctx.effect(() => ctx.locale.register(NS, { zh, en }), 'infogen-dsh: locale');
        const t = ctx.locale.bind(NS);
        ctx.slots.inject('shell.overlay', () => ctx.slots.register({
          name: 'shell.overlay',
          id: 'infogen-dsh.status',
          locale: NS,
        }, InfoGenToast));
        ctx.slots.inject('settings.section', () => ctx.slots.register({
          name: 'settings.section',
          id: 'infogen-dsh',
          order: 22,
          label: () => t('section.nav'),
          locale: NS,
        }, InfoGenSettingsSection));
      },
    };
  },
});
