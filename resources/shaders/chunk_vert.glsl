#version 120
#pragma optimize(on)
#pragma debug(off)

attribute vec4 coords;    // we only input a vec3, so w defaults to 1.0
attribute vec3 normal;
attribute vec3 colour;

varying vec3 normal_frag;
varying vec3 colour_frag;
varying vec3 reflectdir_frag;
varying vec3 coords_camlocal_frag;
varying float fog_frag;

void main() {
  const vec3 lightdir = vec3(0.267261, 0.534522, 0.801784);
  reflectdir_frag = gl_Normalmatrix * reflect(lightdir, normal);
  normal_frag = normal;
  colour_frag = colour;

  gl_Position = gl_ModelViewProjectionmatrix * coords;
  coords_camlocal_frag = (gl_ModelViewmatrix * coords).xyz;

  fog_frag = length(coords_camlocal_frag) / 300.0;
}
