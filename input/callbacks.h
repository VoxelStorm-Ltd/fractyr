#pragma once

struct GLFWwindow;

void callback_mousepos(GLFWwindow *thiswindow, double x, double y);
void callback_mousepos_pause(GLFWwindow *thiswindow, double x, double y);
void callback_mousebutton(GLFWwindow *thiswindow, int button, int action, int mods);
void callback_key(GLFWwindow *thiswindow, int key, int scancode, int action, int mods);
void callback_key_pause(GLFWwindow *thiswindow, int key, int scancode, int action, int mods);
void callback_scroll(GLFWwindow *thiswindow, double xoffset, double yoffset);
void callback_windowresize(GLFWwindow *thiswindow, int newwidth, int newheight);
void callback_windowrefresh(GLFWwindow *thiswindow);
void callback_windowrefresh_minimal(GLFWwindow *thiswindow);
void callback_windowclose(GLFWwindow *thiswindow);
void callback_windowfocus(GLFWwindow *thiswindow, int focused);
void callback_windowminimise(GLFWwindow *thiswindow, int minimised);
void callback_textinput(GLFWwindow *thiswindow, unsigned int thischar);
