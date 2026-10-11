# Solar System Simulation 3D

An interactive 3D solar-system visualization written in C++ with legacy
OpenGL and GLUT. The application renders the Sun, eight planets, orbital
paths, planet textures, lighting, and a Milky Way background. The camera can
be rotated, zoomed, and focused on individual celestial bodies.

This repository contains the source project, textures, build documentation,
controls documentation, and the original project report.

## Features

- Textured Sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, and
  Neptune.
- Separate orbital and axial rotation state for the celestial bodies.
- Configurable camera yaw, pitch, and distance.
- Mouse-driven camera rotation and zoom.
- Numeric-key selection and focus zoom for the Sun and planets.
- Forward and reverse orbital movement using `+` and `-`.
- Depth testing, lighting, material color, and perspective projection.
- On-screen controls and selected-object status text.
- BMP texture loading with validation and error messages.

## Repository layout

```text
.
├── .gitignore
├── README.md
├── REQUIREMENTS.md
├── docs/
│   ├── keyboard-and-mouse-controls.pdf
│   └── project-report.pdf
└── src/
    ├── opengl.sln
    └── opengl/
        ├── CGproject1.cpp
        ├── opengl.vcxproj
        ├── opengl.vcxproj.filters
        ├── packages.config
        └── texture/
            ├── earth.bmp
            ├── jupiter.bmp
            ├── mars.bmp
            ├── mercury.bmp
            ├── milkyway.bmp
            ├── moon.bmp
            ├── neptune.bmp
            ├── pluto.bmp
            ├── saturn.bmp
            ├── sun.bmp
            ├── uranus.bmp
            └── venus.bmp
```

The `moon.bmp` and `pluto.bmp` assets are retained as project resources. The
current renderer displays the Sun and the eight planets listed above.

## Requirements

See [`REQUIREMENTS.md`](REQUIREMENTS.md) for the complete software, dependency,
hardware, and runtime requirements. In brief:

- Windows 10 or later.
- Visual Studio 2022 with the **Desktop development with C++** workload.
- MSVC v143 build tools and the Windows 10 SDK.
- NuGet package restore enabled.
- A graphics adapter with OpenGL support.
- The two packages declared in
  [`src/opengl/packages.config`](src/opengl/packages.config):
  `nupengl.core` and `nupengl.core.redist`, both version `0.1.0.1`.

## Getting started

### 1. Clone the repository

```powershell
git clone https://github.com/Anzam-SK/Solar_System_Simulation_3D.git
cd Solar_System_Simulation_3D
```

### 2. Restore dependencies

Open [`src/opengl.sln`](src/opengl.sln) in Visual Studio 2022. Visual Studio
should restore the packages from `packages.config` automatically. If restore
is disabled, use **Tools > NuGet Package Manager > Package Manager Settings**,
enable package restore, close and reopen the solution, and restore packages
from the solution context menu.

The restored packages are intentionally not committed. They are downloaded
into the solution's `packages/` directory and are ignored by
[`.gitignore`](.gitignore).

### 3. Select a configuration

Choose one of the configurations supplied by the solution:

- `Debug | x64` for development and debugging.
- `Release | x64` for a production-style build.
- `Debug | x86` or `Release | x86` when a 32-bit build is required.

`x64` is recommended on modern Windows systems.

### 4. Build and run

Set the `opengl` project as the startup project, then select **Build > Build
Solution** or press `Ctrl+Shift+B`. Run with `F5` or `Ctrl+F5`.

The program expects its current working directory to contain the `texture/`
folder. Running from Visual Studio with the project directory as the working
directory preserves the expected paths used by `CGproject1.cpp`, such as
`texture/sun.bmp` and `texture/milkyway.bmp`.

If the application reports that a texture cannot be opened, verify that the
working directory is [`src/opengl`](src/opengl), or copy the generated
executable beside the `texture/` directory before launching it.

## Controls

| Input | Action |
| --- | --- |
| Left mouse button + drag | Rotate the camera horizontally and vertically |
| Mouse wheel up | Zoom in |
| Mouse wheel down | Zoom out |
| `0` | Select the Sun |
| `1` | Select Mercury |
| `2` | Select Venus |
| `3` | Select Earth |
| `4` | Select Mars |
| `5` | Select Jupiter |
| `6` | Select Saturn |
| `7` | Select Uranus |
| `8` | Select Neptune |
| `+` | Move the orbital simulation forward |
| `-` | Move the orbital simulation backward |
| `Esc` | Clean up textures and exit |

Numeric selection is held while the key is pressed. While a body is selected,
the zoom range is constrained according to that body's radius. Camera pitch
is constrained to prevent the view from flipping upside down.

Detailed controls are also available in
[`docs/keyboard-and-mouse-controls.pdf`](docs/keyboard-and-mouse-controls.pdf).

## Technical overview

The main implementation is in
[`src/opengl/CGproject1.cpp`](src/opengl/CGproject1.cpp). It is a single-file
OpenGL application organized around:

1. Global simulation and camera state.
2. BMP texture loading and OpenGL texture setup.
3. Celestial-body and orbit rendering.
4. Lighting, perspective projection, and window resizing.
5. GLUT mouse and keyboard callbacks.
6. Resource cleanup on exit.

The renderer uses immediate-mode OpenGL and GLUT callbacks. Planet positions,
radii, names, and movement speeds are kept in type-safe `std::array` values
near the top of the source file. Textures are loaded from the relative
`texture/` directory during initialization.

## Troubleshooting

### Missing NuGet package or native library

Restore the NuGet packages and rebuild. The Visual Studio project checks for
the native OpenGL package targets before compiling and reports the missing
package path if restoration has not completed.

### Texture loading errors

The application loads textures using relative paths. Start it with
`src/opengl` as the working directory and confirm that the BMP files exist in
[`src/opengl/texture`](src/opengl/texture).

### `GL/glut.h` cannot be found

Confirm that `nupengl.core` was restored for the selected platform and that
the project is being built through [`src/opengl.sln`](src/opengl.sln), rather
than compiling `CGproject1.cpp` without the project include and library
settings.

### x86/x64 linker mismatch

Use the platform that matches the restored native libraries. Prefer `x64` on
64-bit Windows; do not mix x86 output with x64 libraries or vice versa.

### Black or incomplete rendering

Check that the graphics driver supports OpenGL and that the texture files are
available from the process working directory. Run a Debug build from Visual
Studio to see texture-loading errors in the Output window or console.

## Documentation

- [`REQUIREMENTS.md`](REQUIREMENTS.md): complete setup and environment
  requirements.
- [`docs/keyboard-and-mouse-controls.pdf`](docs/keyboard-and-mouse-controls.pdf):
  controls reference.
- [`docs/project-report.pdf`](docs/project-report.pdf): original project
  report.

## Development notes

Generated files such as `.vs/`, `x64/`, `Debug/`, `Release/`, binaries,
debug symbols, logs, and restored NuGet packages are excluded by
[`.gitignore`](.gitignore). Do not commit those files.

When adding a new texture or source file, keep it under `src/opengl/` and
update the Visual Studio project if required. Preserve the relative texture
paths unless the runtime loading strategy is changed as well.

## License and attribution

No separate license file is currently included. The repository is an
academic computer-graphics project. Add licensing information before
redistributing the source or bundled assets.
