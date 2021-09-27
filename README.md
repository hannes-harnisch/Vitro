# Vitro
This project is an ongoing personal experiment/exercise in low-level GPU programming and designing a scalable cross-platform C++ architecture for a rendering engine. I'm working on it purely for fun.

Vitro is intended to grow into an efficient multiplatform 2D and 3D scene viewer/editor and graphics engine. It relies heavily on C++20 features, especially modules. At the moment, a build of the newest Premake branch is required in order to generate project files.

The graphics API currently used primarily for development is Direct3D 12. A Vulkan backend is being added gradually.

In terms of operating systems, only Windows is currently supported, with Linux support in planning. Mobile platforms may be supported in the future as well.