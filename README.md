# clipgrab (v4.0.0)

A customized, updated version of ClipGrab (http://clipgrab.org/) ported to **Qt 6** with **native Apple Silicon (arm64)** support on macOS.

ClipGrab is a beautiful, easy-to-use GUI frontend for [yt-dlp](https://github.com/yt-dlp/yt-dlp) / [youtube-dl](https://youtube-dl.org).

---

## Special Thanks
Heartfelt thanks to the original author, **Philipp Schmieder**, and the ClipGrab project team for creating and maintaining this wonderful open-source downloader. This fork builds on their excellent work to keep it running smoothly on modern macOS architectures.

---

## Features in v4.0.0 (Apple Silicon Fork)
- **Qt 6 Migration:** Ported from Qt 5 to Qt 6, using the `core5compat` module.
- **Native Apple Silicon (arm64):** Compiled natively to run directly on M1/M2/M3/M4 Macs without needing Rosetta 2 translation.
- **Bundled arm64 Dependencies:** Automatically packages native `arm64` builds of `ffmpeg` and `deno` inside the `.app` bundle.
- **Python 3 Compatibility:** Corrected the interpreter search logic to check for `python3` first, ensuring it runs on modern macOS where Python 2 has been removed.

---

## Compiling ClipGrab

### macOS (Apple Silicon arm64)
We have provided an automated build script [build_arm64.sh](build_arm64.sh) that manages dependencies via Homebrew and packages a native application bundle:

```sh
./build_arm64.sh
```
This will generate `ClipGrab.app` natively. To replace your system-wide installation, run:
```sh
mv ClipGrab.app /Applications/ClipGrab.app
```

### Linux / Windows (Standard Qt Build)
To compile ClipGrab manually:

1. **Install Qt 6 developer libraries** (including `qtwebengine` and `qt5compat`).
2. **Ensure python3 and ffmpeg are in your system PATH.**
3. **Run qmake and make:**
   ```sh
   qmake clipgrab.pro && make
   ```
