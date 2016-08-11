#version 120
#pragma optimize(on)
#pragma debug(off)

attribute vec4 coords;    // we only input a vec3, so w defaults to 1.0
attribute vec4 colour;

varying vec4 colour_frag;

void main() {
  colour_frag = colour;
  gl_Position = gl_ModelViewProjectionmatrix * coords;
}
