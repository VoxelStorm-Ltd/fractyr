#version 120
#pragma optimize(on)
#pragma debug(off)

attribute vec4 coords;    // we only input a vec3, so w defaults to 1.0
attribute vec3 normal;

varying vec3 normal_frag;
varying vec3 reflectdir_frag;
varying vec3 coords_camlocal_frag;

void main() {
  const vec3 lightdir = vec3(0.267261, 0.534522, 0.801784);
  reflectdir_frag = gl_NormalMatrix * reflect(lightdir, normal);
  normal_frag = normal;

  gl_Position = gl_ModelViewProjectionMatrix * coords;
  coords_camlocal_frag = (gl_ModelViewMatrix * coords).xyz;
}
