# Load & Lock

`Load & Lock` is a C++17 Sokoban game with a shared game core and multiple front ends.

![Load & Lock](L&L.png)

Play in browser:

https://h8man.github.io/Load-n-Lock/

## Project layout

```text
src/
├── assets/    # level loading
├── audio/     # console beep or raylib audio
├── core/      # application loop
├── game/      # gameplay and cutscene state
├── input/     # console or raylib input
└── renderer/  # console or raylib rendering

assets/
└── levels/    # .sok level files
```

## Native build

The Visual Studio project uses shared MSBuild properties from `Directory.Build.props`.

### Build properties

- `LoadAndLockRenderer`
  - `Raylib`
  - `Console`
- `LoadAndLockSubsystem`
  - `Windows`
  - `Console`

### Visual Studio / MSBuild examples

Raylib windowed build:

```powershell
msbuild ".\Load & Lock.sln" /t:Build /p:Configuration=Debug /p:Platform=x64 /p:LoadAndLockRenderer=Raylib /p:LoadAndLockSubsystem=Windows
```

Console build:

```powershell
msbuild ".\Load & Lock.sln" /t:Build /p:Configuration=Debug /p:Platform=x64 /p:LoadAndLockRenderer=Console /p:LoadAndLockSubsystem=Console
```

## WebAssembly build with Emscripten

A Linux-oriented `Makefile` is included for `emsdk` builds.

### Requirements

- `emsdk` activated in the shell
- `em++`
- `emrun`
- Emscripten raylib port support via `--use-port=raylib`

### Build

Release build:

```bash
make
```

Debug build:

```bash
make debug
```

The output is written to:

```text
build/web/index.html
```

### Run locally

```bash
make serve
```

This uses:

```text
emrun --no_browser --port 8080 build/web/index.html
```

## License

This project is licensed under the MIT License.
