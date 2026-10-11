# Solar System Simulation 3D

An interactive 3D solar-system simulation built with C++, OpenGL, and GLUT.
The application renders the Sun, planets, moons, orbital paths, and a
space-background texture. Camera movement is controlled with the mouse.

## Repository layout

```text
.
├── docs/
│   ├── keyboard-and-mouse-controls.pdf
│   └── project-report.pdf
├── src/
│   ├── opengl.sln
│   └── opengl/
│       ├── CGproject1.cpp
│       ├── opengl.vcxproj
│       └── texture/
├── .gitignore
└── README.md
```

## Requirements

- Windows 10 or later
- Visual Studio 2022 with the **Desktop development with C++** workload
- NuGet package restore enabled

The project uses the following NuGet packages:

- `nupengl.core` `0.1.0.1`
- `nupengl.core.redist` `0.1.0.1`

## Build and run

1. Clone the repository:

   ```powershell
   git clone https://github.com/Anzam-SK/Solar_System_Simulation_3D.git
   ```

2. Open [`src/opengl.sln`](src/opengl.sln) in Visual Studio.
3. Allow Visual Studio/NuGet to restore the packages listed in
   [`src/opengl/packages.config`](src/opengl/packages.config).
4. Select the `x64` platform and the `Debug` or `Release` configuration.
5. Build and run the `opengl` project.

Build output and Visual Studio-generated files are intentionally excluded by
[`.gitignore`](.gitignore).

## Controls

| Input | Action |
| --- | --- |
| Left mouse button + drag | Rotate the camera |
| Mouse wheel | Zoom in or out |
| `Esc` | Exit the program |

Additional interaction details are available in
[`docs/keyboard-and-mouse-controls.pdf`](docs/keyboard-and-mouse-controls.pdf).

## Project documentation

The original project report is available at
[`docs/project-report.pdf`](docs/project-report.pdf).
