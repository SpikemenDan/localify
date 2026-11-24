# Publishing Localify and installing it as an app

Localify is one HTML file. You can keep opening `localify_offline_mp3_player.html`
straight from disk — that still works exactly as before. Publishing it on GitHub
Pages adds a web address you can **install** on a phone or computer, and it then
opens offline like a normal app.

Your music is never uploaded. The site only contains the player; songs are picked
from your own device each time, and the offline cache stores only the player's own
files (see `sw.js`). Liked songs, settings and profiles stay in your browser on each
device — use **Settings → Your data** to move them between devices.

## How the files fit together

| File | What it's for |
| --- | --- |
| `localify_offline_mp3_player.html` | The whole app. The only file you edit. Works from disk or from the web. |
| `index.html` | A tiny redirect so the site's root address opens the app. |
| `manifest.webmanifest` | Name, colors and icons used when the app is installed. |
| `sw.js` | Service worker: caches the app shell so it opens offline. |
| `icons/` | App icons. Rebuild the PNGs with `python tools/make-icons.py`. |
| `.github/workflows/pages.yml` | Publishes the files above (and only those) to Pages. |

Why the app keeps its long file name instead of becoming `index.html`: it's the file
you already open from disk and the one other branches are changing, so renaming it
would break your shortcut and cause merge conflicts. `index.html` just forwards to it.

## 1. Turn on GitHub Pages (one time)

1. Merge this work into `main` and push it to GitHub.
2. On GitHub, open the repository → **Settings** → **Pages**.
3. Under **Build and deployment**, set **Source** to **GitHub Actions**.
4. Open the **Actions** tab. The **Deploy to GitHub Pages** run starts on the next
   push to `main`; to start it now, pick the workflow and click **Run workflow**.
5. When it finishes, the address is shown on the run and on **Settings → Pages**,
   usually `https://<your-user-name>.github.io/<repository-name>/`.

A public repository is needed for Pages on a free GitHub plan. Anyone with the
address can open the player (it's empty until they pick their own music).

Every later push to `main` publishes again. Installed copies notice the new version
and show **"A new version of Localify is ready — Reload"**.

## 2. Install it

### Android (Chrome)
1. Open the address in Chrome.
2. Tap **Install app** in Localify's profile menu or under **Settings → App**, or use
   Chrome's **⋮ → Install app / Add to Home screen**.
3. Open Localify from the home screen. Use **Add folder / Add files** to pick music.

### iPhone / iPad (Safari)
1. Open the address in **Safari** (other iOS browsers can't install apps).
2. Tap **Share** (the square with an arrow) → **Add to Home Screen** → **Add**.
3. Open Localify from the home screen. Pick songs with **Add files**; iOS doesn't
   let web apps keep access to folders, so you pick them again after a restart.

### Windows (Edge or Chrome)
1. Open the address in Edge or Chrome.
2. Click **Install app** in Localify (profile menu or **Settings → App**), or the
   install icon at the right end of the address bar.
3. Localify gets its own window, Start menu entry and taskbar icon. Uninstall it
   from **Settings → Apps** like any other app.

## Testing locally

Service workers need `http://localhost` (not `file://`):

```bash
python -m http.server 8000
```

Then open <http://localhost:8000/>. Locally the service worker's version is `dev`, which
makes it fetch fresh files first (so a normal reload shows your edits) and fall back to
the cache only when the server is stopped. The deployed version is cache-first instead,
and each deploy gets a new version, which is what triggers the "Reload" bar.
