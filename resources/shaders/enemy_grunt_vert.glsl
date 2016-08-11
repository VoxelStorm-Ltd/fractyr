#version 120
#pragma optimize(on)
#pragma debug(off)

attribute vec4 coords;    // we only input a vec3, so w defaults to 1.0
attribute vec3 normal;
attribute vec4 colour;

varying vec3 normal_frag;
varying vec4 colour_frag;
varying vec3 reflectdir_frag;
varying vec3 coords_camlocal_frag;

const vec3 lightdir = vec3(0.267261, 0.534522, 0.801784);

void main() {
  normal_frag = normal;
  colour_frag = colour;

  reflectdir_frag = gl_Normalmatrix * reflect(lightdir, normal);

  gl_Position = gl_ModelViewProjectionmatrix * coords;
  coords_camlocal_frag = (gl_ModelViewmatrix * coords).xyz;
}
