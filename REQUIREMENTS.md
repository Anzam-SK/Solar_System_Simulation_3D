# Requirements

This document describes the environment required to build and run Solar
System Simulation 3D.

## Operating system

- Windows 10 or later.
- A 64-bit Windows installation is recommended.
- Administrator access may be needed when installing Visual Studio components
  or native dependencies.

## Development environment

- Visual Studio 2022.
- The **Desktop development with C++** workload.
- MSVC v143 C++ build tools.
- A Windows 10 SDK installed through Visual Studio Installer.
- NuGet package restore enabled.

The solution is located at
[`src/opengl.sln`](src/opengl.sln). The Visual Studio project supports these
configurations:

- Debug | Win32
- Release | Win32
- Debug | x64
- Release | x64

Use an x64 configuration on a 64-bit machine unless a 32-bit executable is
specifically required.

## C++ and graphics dependencies

The program includes the GLUT/OpenGL headers and links against native
libraries supplied by these packages:

| Package | Version | Purpose |
| --- | --- | --- |
| `nupengl.core` | `0.1.0.1` | OpenGL/GLUT headers, libraries, and build targets |
| `nupengl.core.redist` | `0.1.0.1` | Native runtime redistributables |

The exact dependency declarations are in
[`src/opengl/packages.config`](src/opengl/packages.config). Do not commit the
restored `packages/` directory; it is intentionally ignored by Git.

## Runtime requirements

- A graphics adapter with OpenGL support and a current Windows driver.
- A display capable of opening a GLUT window.
- Access to the BMP files in [`src/opengl/texture`](src/opengl/texture).
- The application's working directory must be `src/opengl`, or otherwise
  contain a `texture/` directory beside the executable.

The application loads these runtime textures:

- `sun.bmp`
- `mercury.bmp`
- `venus.bmp`
- `earth.bmp`
- `mars.bmp`
- `jupiter.bmp`
- `saturn.bmp`
- `uranus.bmp`
- `neptune.bmp`
- `milkyway.bmp`

`moon.bmp` and `pluto.bmp` are also included as project assets for future
extensions.

## Recommended machine profile

The project is small and does not require specialized hardware. A practical
development setup is:

- 64-bit Windows 10/11.
- 4 GB or more of RAM.
- A modern integrated or dedicated GPU with OpenGL support.
- At least 500 MB of free disk space for Visual Studio components, NuGet
  packages, and build output.
- A mouse with a scroll wheel for the intended camera controls.

These are recommendations rather than hard limits; Visual Studio's own
system requirements may be higher.

## Build prerequisites checklist

Before building, verify that:

1. Visual Studio 2022 is installed with the required C++ workload.
2. The Windows SDK and MSVC v143 toolset are installed.
3. The solution opens without a project-load error.
4. NuGet restore has completed successfully.
5. `src/opengl/texture/` contains the required BMP files.
6. The selected platform matches the native library architecture.
7. The working directory exposes the relative `texture/` path.
