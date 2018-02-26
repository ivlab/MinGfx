API - MinGfx Programming Reference Organized by Topic {#api}
==========

# API by Topic

| Application Class |
|-------------------|
| [GraphicsApp](@ref mingfx::GraphicsApp) |


| 3D Models |
|-----------|
| [QuickShapes](@ref mingfx::QuickShapes) |
| [Mesh](@ref mingfx::Mesh)               |


| Color and Textures |
|--------------------|
| [Color](@ref mingfx::Color)         |
| [Texture2D](@ref mingfx::Texture2D) |


| Graphics Math |
|---------------|
| [Matrix4](@ref mingfx::Matrix4)       |
| [Point2](@ref mingfx::Point2)         |
| [Point3](@ref mingfx::Point3)         |
| [Vector2](@ref mingfx::Vector2)       |
| [Vector3](@ref mingfx::Vector3)       |
| [Ray](@ref mingfx::Ray)               |
| [Quaternion](@ref mingfx::Quaternion) |
| [GfxMath](@ref mingfx::GfxMath)       |


| Shader Programs |
|-----------------|
| [DefaultShader](@ref mingfx::DefaultShader) |
|  - [DefaultShader::LightProperties](@ref mingfx::DefaultShader::LightProperties) |
|  - [DefaultShader::MaterialProperties](@ref mingfx::DefaultShader::MaterialProperties) |
| [ShaderProgram](@ref mingfx::ShaderProgram) |


| User Interface |
|----------------|
| [CraftCam](@ref mingfx::CraftCam) |
| [UniCam](@ref mingfx::UniCam) |


| File I/O and System Routines |
|------------------------------|
| [Platform](@ref mingfx::Platform) |




# Coding Style

The library follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html), in part as an example for students, since this style is also used in several courses.  There are several things that programmers who are not familiar with the Google style might find unusual.  These are the most common style rules to note:
- C++ source filenames are all lowercase with underscores, and a .cc extension is used instead of .cpp.
- Variable names are all lowercase with underscores.
- Class member variables are named the same as regular variables but with a trailing _, as in my\_member\_var\_.
- Functions start with capital letters unless they are small getter or setter methods.
- There are many other rules, a solid discussion of pros/cons, and an automated style checker [here](https://google.github.io/styleguide/cppguide.html).

