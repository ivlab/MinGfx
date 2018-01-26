



// We often use the code that is commented out below to load opengl headers in a cross-platform way,
// but since nanogui uses glad internally, we will just use their approach to load opengl headers
// so that everything is consistent.

#ifdef WIN32
#define NANOGUI_GLAD
#endif

#include <nanogui/opengl.h>


/*** Our typical appraoch:

// GLEW is needed on Windows and Linux
#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

***/