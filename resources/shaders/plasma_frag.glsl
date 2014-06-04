#version 120
#pragma optimize(on)
#pragma debug(off)

varying vec4 colour_frag;

void main() {
  gl_FragColor = colour_frag;
}
