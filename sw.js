/*
  Localify service worker — caches the APP SHELL ONLY so the player opens offline.

  What it never touches:
  - Your music. Songs are read from your device as blob:/file handles; those requests
    never reach a service worker, and anything not in SHELL below is passed straight
    to the network without being stored.
  - Your data. Likes, settings and profiles live in localStorage/IndexedDB, not here.

  Updates: VERSION is replaced with the commit SHA by the Pages workflow, so every deploy
  installs a new worker. It waits until the page shows "A new version is ready" and the
  user taps Reload (the page then posts SKIP_WAITING), so a song is never cut off.
*/
const VERSION = 'dev';
const CACHE = 'localify-shell-' + VERSION;
const SHELL = [
  './',
  './index.html',
  './localify_offline_mp3_player.html',
  './manifest.webmanifest',
  './icons/icon.svg',
  './icons/icon-192.png',
  './icons/icon-512.png',
  './icons/icon-maskable-512.png',
  './icons/apple-touch-icon.png'
];
const SHELL_URLS = new Set(SHELL.map((p) => new URL(p, self.registration.scope).href));

self.addEventListener('install', (event) => {
  // cache: 'reload' skips the HTTP cache so a new version never stores stale files.
  event.waitUntil(
    caches.open(CACHE).then((c) => c.addAll(SHELL.map((p) => new Request(p, { cache: 'reload' }))))
  );
});

self.addEventListener('activate', (event) => {
  event.waitUntil(
    caches.keys().then((keys) =>
      Promise.all(keys.filter((k) => k.startsWith('localify-shell-') && k !== CACHE).map((k) => caches.delete(k)))
    )
  );
});

self.addEventListener('message', (event) => {
  if (event.data && event.data.type === 'SKIP_WAITING') self.skipWaiting();
});

self.addEventListener('fetch', (event) => {
  const req = event.request;
  if (req.method !== 'GET') return;
  const url = new URL(req.url);
  url.search = '';
  url.hash = '';
  if (!SHELL_URLS.has(url.href)) return;   // not ours: let the browser handle it, uncached
  if (VERSION === 'dev') {
    // Local testing (not stamped by the deploy workflow): network first, so edits show up
    // on a normal reload; the cached copy is only used when the server is unreachable.
    event.respondWith(
      fetch(req).then((res) => {
        if (res.ok) { const copy = res.clone(); caches.open(CACHE).then((c) => c.put(url.href, copy)); }
        return res;
      }).catch(() => caches.open(CACHE).then((c) => c.match(req, { ignoreSearch: true })))
    );
    return;
  }
  // Deployed: cache-first for the shell (instant, works offline); ignoreSearch so "?x"
  // variants still match. New files arrive with the next VERSION.
  event.respondWith(
    caches.open(CACHE).then((c) =>
      c.match(req, { ignoreSearch: true }).then((hit) => hit || fetch(req))
    )
  );
});
