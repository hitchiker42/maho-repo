#ifndef __GL_UTIL_H__
#define __GL_UTIL_H__
#include "gl_types.h"

#if (defined DEBUG) && !(defined NDEBUG)
#define DEBUG_PRINTF(msg,args...) fprintf(stderr, msg, ##args)
#define IF_DEBUG(code) code
#else
#define DEBUG_PRINTF(msg,args...)
#define IF_DEBUG(code)
#endif
/*
  Initialize glfw, create a window and opengl context, then
  initialize glew and insure opengl3.3 is supported
*/
GLFWwindow* init_gl_context(int w, int h, const char* name);
/*
  Arguments need to be null terminated strings.
  Could be eaisly modified to take more arguments.
*/
GLuint create_shader_program(const char *vertex_shader_source,
                             const char *fragment_shader_source);
void glfw_main_loop(GLFWwindow *win, void(*draw)(void*), void *userdata);
//create a buffer on the gpu and bind the given data to it
GLuint make_data_buffer(void *data, size_t size, int usage);
/*
   the opengl equlivent of this takes the offset as a pointer, not a pointer
   to an int, but the offset itself is a void*, which is weird.

   stride is the size of the structure the attributes are stored in.
*/
void bind_vertex_attrib(GLuint buffer, GLint loc, int size, GLenum type,
                        int normalized, size_t stride, size_t offset);
//this is a pretty trivial wrapper functione
static inline void unbind_vertex_attrib(GLint loc){
  glDisableVertexAttribArray(loc);
}
#define unbind_vertex_attrib_2(loc1, loc2)      \
  unbind_vertex_attrib(loc1);                   \
  unbind_vertex_attrib(loc2);
#define unbind_vertex_attrib_3(loc1, loc2, loc3)        \
  unbind_vertex_attrib_2(loc1, loc2);                   \
  unbind_vertex_attrib(loc3);
#endif