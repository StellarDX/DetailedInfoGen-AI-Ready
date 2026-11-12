/**
 * Browser half of InfoGen-DSH.
 *
 * It renders at most one top-of-window toast. The Host monitor only publishes
 * a notice after the user has enabled the Agent and the MCP endpoint has failed
 * `attempts` consecutive probes, so an ordinary session never sees this banner.
 */
window.__ModuleLoader__.load({
  id: 'infogen-dsh',
  factory(require) {
    const React = require('react');
    const h = React.createElement;
    const NS = 'infogen-dsh';

    const zh = {
      'toast.unavailable': 'InfoGen MCP 服务（{url}）不可达，连续 {count} 次探测失败。请启动 InfoGen MCP 服务，Agent 会在后台自动恢复。',
    };
    const en = {
      'toast.unavailable': 'The InfoGen MCP service ({url}) is unreachable after {count} consecutive probes. Start the InfoGen MCP service; the agent reconnects automatically.',
    };

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

    function InfoGenToast({ t }) {
      const [notice, setNotice] = React.useState(null);
      const [visible, setVisible] = React.useState(false);
      const lastAt = React.useRef(0);

      React.useEffect(() => {
        let cancelled = false;
        let timer;
        const poll = async () => {
          try {
            const response = await fetch('/infogen-dsh/status', { cache: 'no-store' });
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
        ctx.slots.inject('shell.overlay', () => ctx.slots.register({
          name: 'shell.overlay',
          id: 'infogen-dsh.status',
          locale: NS,
        }, InfoGenToast));
      },
    };
  },
});
