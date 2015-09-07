#include "gl_util.h"
/*
  Basic error handling function to help diganose glfw errors.
*/
static void handle_error(int err_code, const char *err_string){
  fprintf(stderr,"Glfw error %d:\n%s\n",err_code, err_string);
  return;
}
GLFWwindow* init_gl_context(int w, int h, const char* name){
  if(!glfwInit()){
    fprintf(stderr, "Error, failed to initialize glfw\n");
    exit(EXIT_FAILURE);
  }
  atexit(glfwTerminate);
  glfwSetErrorCallback(handle_error);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
  GLFWwindow *win = glfwCreateWindow(w, h, name, NULL, NULL);
  if(!win){
    fprintf(stderr, "Error creating window\n");
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(win);
  //  glfwSwapInterval(1);
  glewExperimental = 1;
  GLenum err = glewInit();
  if(err != GLEW_OK){
    fprintf(stderr, "Error, glew init failure\n");
    exit(EXIT_FAILURE);
  }
  if(!(GLEW_VERSION_3_3)){
    fprintf(stderr, "Error, OpenGL 3.3 not supported\n");
    exit(EXIT_FAILURE);
  }
  return win;
}
static inline __attribute__((pure))
char *shader_type_to_string(GLenum type){
  switch(type){
    case(GL_FRAGMENT_SHADER):
      return "fragment shader";
    case(GL_VERTEX_SHADER):
      return "vertex shader";
    default:
      return "";
  }
}
//doesn't print empty logs
#define print_gl_info_log(objtype, obj, msg)                    \
  ({GLint log_size;                                             \
    glGet##objtype##iv(obj, GL_INFO_LOG_LENGTH, &log_size);     \
    if(log_size > 0){                                           \
      char *buf = alloca(log_size*sizeof(char));                \
      glGet##objtype##InfoLog(obj, log_size, NULL, buf);        \
      fprintf(stderr, "%s"#objtype" info log:\n%s", msg, buf);  \
    };})
#define print_shader_info_log(shader, msg)\
  print_gl_info_log(Shader, shader, msg)
#define print_program_info_log(program, msg)\
  print_gl_info_log(Program, program, msg)

static inline GLuint compile_shader(GLenum type, const char *source, int len){
  GLint status;
  GLuint shader = glCreateShader(type);
#if (defined DEBUG) && !(defined NDEBUG)
  DEBUG_PRINTF("Compiling %s\n", shader_type_to_string(type));
#endif
  if(len == 0){
    len = strlen(source);
  }
  glShaderSource(shader, 1, &source, &len);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if(!status){    
    print_shader_info_log(shader, "Error when compiling shader.\n");
    exit(EXIT_FAILURE);
  }
  //always print shader info when debbuing is enabled#
#if (defined DEBUG) && !(defined NDEBUG)
  print_shader_info_log(shader, "");
#endif
  return shader;
}
//IDEA: change this to also create the program and return it as the return value
static inline void link_program(GLuint program, GLuint *shaders, int nshaders){
  int i, status;
  //the compiler should be able to unroll this loop
  for(i=0;i<nshaders;i++){
    glAttachShader(program, shaders[i]);
  }
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if(!status){
    print_program_info_log(program, "Error when linking program.\n");
    exit(EXIT_FAILURE);
  }
  IF_DEBUG(print_program_info_log(program, "");)

  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
  if(!status){
    print_program_info_log(program, "Error when validating program.\n");
  }
  IF_DEBUG(print_program_info_log(program, "");)
  for(i=0;i<nshaders;i++){
    glDetachShader(program, shaders[i]);
  }
  return;
}
  
GLuint create_shader_program(const char *vertex_shader_source,
                             const char *fragment_shader_source){
  DEBUG_PRINTF("creating shaders\n");

  GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER,
                                        vertex_shader_source, 0);
  GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER,
                                          fragment_shader_source, 0);
  GLuint program = glCreateProgram();
  //this should be optimized out, but I may need to rewrite things to
  //just use this array if not
  GLuint shaders[2] = {vertex_shader, fragment_shader};

  link_program(program, shaders, 2);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return program;
}
void glfw_main_loop(GLFWwindow *window,
                    void(*draw)(void*), void *userdata){
  while(!glfwWindowShouldClose(window)){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear the old bufer
    draw(userdata);
    glfwSwapBuffers(window);//switch buffers, we're using double buffering
    glfwPollEvents();
  }
}

void bind_vertex_attrib(GLuint buffer, GLint loc, int size, GLenum type,
                        int normalized, size_t stride, size_t offset){
  glEnableVertexAttribArray(loc);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glVertexAttribPointer(loc, size, type, normalized, stride, (void*)offset);
}
GLuint make_data_buffer(void *data, size_t size, int usage){
  GLuint buffer;
  glGenBuffers(1,&buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);//make buffer current
  glBufferData(GL_ARRAY_BUFFER, size, data, usage);
  return buffer;
}