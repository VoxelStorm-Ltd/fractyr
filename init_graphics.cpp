#include "universe.h"
#include <iostream>
#include "get_version.h"
#include "platform_defines.h"
#include "oculusstorm/oculusstorm.h"
#include "input/callbacks.h"

// callbacks
void callback_error(int error, char const *description);

// globals
extern GLFWwindow *window_main;
extern oculusstorm *oculus;

void universe::init_graphics(vector2i &windowsize) {
  std::cout << "Initialising graphics..." << std::endl;
  // initialise the opengl window
  if(glfwInit() != GL_TRUE) {
    std::cout << "ERROR: glfwInit() failed" << std::endl;
    _Exit(EXIT_FAILURE);
  }
  glfwSetErrorCallback(callback_error);                                         // set the error callback first

  if(!oculus) {
    oculus = new oculusstorm(60000.0, 50.0);                                    // initialise the oculus rift before graphics init
  }
  int nummonitors = 0;
  GLFWmonitor **monitor_list = glfwGetMonitors(&nummonitors);
  GLFWmonitor *monitor_primary = glfwGetPrimaryMonitor();
  GLFWmonitor *oculusmonitor = NULL;
  GLFWvidmode *videomode_primary = nullptr;
  std::cout << "Monitors: " << nummonitors << std::endl;
  for(int monitornum = 0; monitornum != nummonitors; ++monitornum) {
    GLFWmonitor *thismonitor = monitor_list[monitornum];
    int physicalwidth  = 0;
    int physicalheight = 0;
    int xpos = 0;
    int ypos = 0;
    glfwGetMonitorPhysicalSize(thismonitor, &physicalwidth, &physicalheight);
    glfwGetMonitorPos(thismonitor, &xpos, &ypos);
    GLFWvidmode const *videomode = glfwGetVideoMode(thismonitor);
    if(!videomode_primary) {
      videomode_primary = const_cast<GLFWvidmode*>(videomode);
    }

    std::cout << "Monitor " << monitornum;
    if(thismonitor == monitor_primary) {
      std::cout << " (primary)";
    }
    std::cout << std::endl;
    std::cout << "  Name: " << glfwGetMonitorName(thismonitor) << std::endl;
    std::cout << "  Physical size: " << physicalwidth << " " << physicalheight << std::endl;
    std::cout << "  Position: " << xpos << " " << ypos << std::endl;
    std::cout << "  Mode: " << videomode->width << " " << videomode->height << " " << videomode->refreshRate << std::endl;

    // try to determine if this monitor is the Oculus Rift's display
    if(static_cast<unsigned int>(videomode->width)  == oculus->hmdinfo.HResolution &&
       static_cast<unsigned int>(videomode->height) == oculus->hmdinfo.VResolution &&
       thismonitor != monitor_primary) {
      std::cout << "  (Oculus Rift candidate)" << std::endl;
      oculusmonitor = thismonitor;
    }
  }
  if(oculus->enabled) {
    // rift is available
    std::cout << "Oculus Rift mode enabled." << std::endl;
    windowsize.x = oculus->hmdinfo.HResolution;
    windowsize.y = oculus->hmdinfo.VResolution;
    if(oculusmonitor == NULL) {
      // assume we're running in mirrored mode, and force the primary to full screen
      oculusmonitor = monitor_primary;
    }
    // DEBUG ONLY: disable oculus display running on its own monitor:
    //oculusmonitor = NULL;
  } else {
    // no functional oculus rift
    if(fullscreen) {
      oculusmonitor = monitor_primary;
      windowsize.x = videomode_primary->width;
      windowsize.y = videomode_primary->height;
      glfwWindowHint(GLFW_REFRESH_RATE, videomode_primary->refreshRate);
    } else {
      oculusmonitor = NULL;                                                     // windowed mode
    }
  }
  // set up window hints in advance
  //glfwWindowHint(GLFW_RED_BITS,   state->videomode->redBits);
  //glfwWindowHint(GLFW_GREEN_BITS, state->videomode->greenBits);
  //glfwWindowHint(GLFW_BLUE_BITS,  state->videomode->blueBits);
  glfwWindowHint(GLFW_DEPTH_BITS, 32);
  glfwWindowHint(GLFW_STENCIL_BITS, 0);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);                         // forward compat disables all deprecated functions - we don't want that
  //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, antialiasing);
  #ifndef PLATFORM_MACOS
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_RESET_NOTIFICATION);
  #endif
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
  window_main = glfwCreateWindow(windowsize.x,
                                 windowsize.y,
                                 "Fractyr",
                                 oculusmonitor,                                 // NULL here means run windowed
                                 NULL);
  if(!window_main) {
    // exit if this didn't work
    std::cout << "ERROR: glfwOpenWindow returned NULL; exiting." << std::endl;
    _Exit(EXIT_FAILURE);
  }
  if(windowsize.x > videomode_primary->width) {                                 // make sure the window isn't bigger than the screen
    windowsize.x = videomode_primary->width;
  }
  if(windowsize.y > videomode_primary->height) {
    windowsize.y = videomode_primary->height;
  }
  if(!fullscreen) {
    glfwSetWindowPos(window_main, (videomode_primary->width  / 2) - (windowsize.x / 2), // try to centre the window
                     (videomode_primary->height / 2) - (windowsize.y / 2));
  }
  glfwMakeContextCurrent(window_main);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glfwShowWindow(window_main);                                                  // only display the window once in position
  glfwSetWindowTitle(window_main, "Fractyr: Loading...");

  // callbacks
  glfwSetCursorPosCallback(    window_main, callback_mousepos);
  glfwSetMouseButtonCallback(  window_main, callback_mousebutton);
  glfwSetKeyCallback(          window_main, callback_key);
  glfwSetScrollCallback(       window_main, callback_scroll);
  glfwSetWindowRefreshCallback(window_main, callback_windowrefresh_minimal);
  glfwSetWindowCloseCallback(  window_main, callback_windowclose);
  glfwSetWindowFocusCallback(  window_main, callback_windowfocus);
  glfwSetWindowIconifyCallback(window_main, callback_windowminimise);
  glfwSetWindowSizeCallback(   window_main, callback_windowresize);

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK) {
    std::cout << "ERROR: GLEW returned " << glewInit() << std::endl;
    _Exit(EXIT_FAILURE);
  }
  glewExperimental = GL_TRUE;
  std::cout << "GL_VERSION:                  " << glGetString(GL_VERSION)  << std::endl;
  std::cout << "GL_VENDOR:                   " << glGetString(GL_VENDOR)   << std::endl;
  std::cout << "GL_RENDERER:                 " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "GL_SHADING_LANGUAGE_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "GL version major " << glfwGetWindowAttrib(window_main, GLFW_CONTEXT_VERSION_MAJOR)
            << " minor "           << glfwGetWindowAttrib(window_main, GLFW_CONTEXT_VERSION_MINOR)
            << " revision "        << glfwGetWindowAttrib(window_main, GLFW_CONTEXT_REVISION)
            << " API "             << glfwGetWindowAttrib(window_main, GLFW_CLIENT_API)
            << " profile  "        << glfwGetWindowAttrib(window_main, GLFW_OPENGL_PROFILE) << std::endl;;
  if(!GLEW_ARB_vertex_array_object) {
    std::cout << "GLEW_ARB_vertex_array_object not available..." << std::endl;
    if(!GLEW_ATI_vertex_array_object) {
      if(!GLEW_APPLE_vertex_array_object) {
        //config->hasvao = false;
      }
    }
  }
  if(glGenVertexArrays == 0) {
    std::cout << "glGenVertexArrays does not appear to be supported." << std::endl;
    //config->hasvao = false;
  }
  GLint antialiasing_buffers, antialiasing_samples, depthbits;
  glGetIntegerv(GL_SAMPLE_BUFFERS, &antialiasing_buffers);
  glGetIntegerv(GL_SAMPLES, &antialiasing_samples);
  glGetIntegerv(GL_DEPTH_BITS, &depthbits);
  std::cout << "GL antialiasing mode: " << antialiasing << " buffers: " << antialiasing_buffers << ", samples: " << antialiasing_samples << std::endl;
  std::cout << "GL depth buffer: " << depthbits << " bits" << std::endl;

  glFrontFace(GL_CCW);                                                          // set up counter-clockwise polygon winding
  glCullFace(GL_BACK);                                                          // may be redundant to cull back-faces
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);                                                      // go on, use the zbuffer
  glDisable(GL_DITHER);                                                         // may marginally increase shading quality, but may be ignored in 24/32bit colour mode by the driver, has performance penalty
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glEnable(GL_MINMAX);                                                          // allow min and max colour tables for HDR effects
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                                    // GL_FILL or GL_LINE
  glShadeModel(GL_SMOOTH);                                                      // SMOOTH or FLAT
  glDisable(GL_NORMALIZE);
  //glDisable(GL_RESCALE_NORMALS);
  //glEnable(GL_TEXTURE);
  //glEnable(GL_TEXTURE_2D);
  //glClientActiveTexture(GL_TEXTURE0);                                           // for subsequent glTexCoordPointer calls, see http://stackoverflow.com/a/2227825/1678468

  // smoothing setup
  glDisable(GL_POLYGON_SMOOTH);                                                 // these are generally obsolete and break with some blending; antialiasing should be used instead
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_LINE_SMOOTH);                                                    // with shaders, line smoothing grinds to a halt on ATI OpenGL2.1 hardware, see http://lists.apple.com/archives/mac-opengl/2008/Apr/msg00043.html
  glHint(GL_LINE_SMOOTH_HINT,    GL_FASTEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  glLineWidth(1.0);                                                             // any other value than 1.0 will break with shaders on ATI OpenGL2.1 hardware, see http://lists.apple.com/archives/mac-opengl/2008/Apr/msg00043.html

  // fog settings
  glDisable(GL_FOG);

  glDisable(GL_COLOR_MATERIAL);                                                 // needs to come afetr glColorMaterial

  if(oculus->enabled) {
    // cache the matrics
    oculus->cachematrices();
    // set any other settings necessary for operation
    glEnable(GL_SCISSOR_TEST);                                                  // needed for glClear to work in multiple viewports without overwriting whole screen
  }

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  std::stringstream newtitle;
  newtitle << "Fractyr " << get_version();
  glfwSetWindowTitle(window_main, newtitle.str().c_str());

  std::cout << "Graphics initialised." << std::endl;
}
