# MinGfx Toolkit

## Quick Links

* [Documentation Main Page][1]
* [libMinGfx Installation Guide][2]
* [Creating Your Own Graphics Program by Copying an Example][3]
* [API Organized by Topic][4]
* [Source on GitHub][5]

[1]: https://ivlab.github.io/MinGfx/html/html/api.html
[2]: https://ivlab.github.io/MinGfx/html/html/installation.html
[3]: https://ivlab.github.io/MinGfx/html/html/installation.html#example
[4]: https://ivlab.github.io/MinGfx/html/html/api.html
[5]: https://github.com/ivlab/MinGfx

## Project Overview

The MinGfx Toolkit is a minimal library for writing cross-platform (Windows,
OSX, linux) graphics programs with modern OpenGL 3.3+ support. Application
programmers should subclass from `MinGfx::GraphicsApp`, which provides callbacks
for several styles of rendering and user interfaces. On-screen graphical user
interfaces can be created with built-in support for NanoGUI. 2D vector graphics
rendering is supported by linking with NanoVG. 3D graphics rendering is
supported via `MinGfx::Mesh`, `MinGfx::Shader`, `MinGfx::Texture2D`, and related
classes. Additionally, application programmers can do whatever custom 2D or 3D
rendering is needed with direct calls to OpenGL.

The library is maintained by Prof. Daniel Keefe (dfk@umn.edu) and is used for
instructional purposes in UMN computer science courses (CSci-4611, CSci-3081W,
...) as well as some research projects. See LICENSE.txt for additional details.
