/**
 * @file gl_headers.h
 * @brief OpenGL headers.
 *
 */




#ifndef GL_HEADERS_H
#define	GL_HEADERS_H




/**
 * @brief Enabled GL EXT if needed.
 *
 */
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif


/**
 * @brief Enabled GLX EXT if needed.
 *
 */
#ifndef GLX_GLXEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES 1
#endif




#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/freeglut.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>




#endif	/* GL_HEADERS_H */
