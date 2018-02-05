MinGfx Toolkit Documentation {#mainpage}
==========

## Getting Started
1. [libMinGfx Installation Guide](@ref installation)
2. [Create Your Own Graphics Program by Copying an Example](@ref example)
3. [Bookmark the API - The MinGfx Programming Reference Organized by Topic](@ref api)

## Programming Guide
* [MinGfx Programming Reference](@ref api)
* Adding NanoGUI Elements (buttons, sliders, other GUI widgets)
	- [NanoGUI Documentation](https://nanogui.readthedocs.io/en/latest/)
	- [NanoGUI GitHub Repo](https://github.com/wjakob/nanogui)
* Drawing Using NanoVG (for 2D vector graphics)
	- [NanoVG Documentation](https://github.com/memononen/NanoVG)
	- [nanovg.h -- the best place to see all the possible commands](https://github.com/memononen/nanovg/blob/master/src/nanovg.h)
* Drawing Using OpenGL (for 3D graphics)
    - [learnopengl.com](https://learnopengl.com/Introduction)
    - [Google for OpenGL 3.3 Guide](https://www.google.com/search?q=opengl+3.3+guide)


## Overview

The MinGfx Toolkit is a minimal library for writing cross-platform (Windows, OSX, linux) graphics programs with modern OpenGL 3.3+ support.  Application programmers should subclass from MinGfx::GraphicsApp, which provides callbacks for several styles of rendering and user interfaces.  On-screen graphical user interfaces can be created with built-in support for NanoGUI.  2D vector graphics rendering is supported by linking with NanoVG.  3D graphics rendering is supported via MinGfx::Mesh, MinGfx::Shader, MinGfx::Texture2D, and related classes.  Additionally, application programmers can do whatever custom 2D or 3D rendering is needed with direct calls to OpenGL.

The library is maintained by Prof. Daniel Keefe (dfk@umn.edu) and is used for instructional purposes in UMN computer science courses (CSci-4611, CSci-3081W, ...) as well as some research projects.  See LICENSE.txt for additional details.

