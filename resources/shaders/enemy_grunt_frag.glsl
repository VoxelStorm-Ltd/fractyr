#version 120
#pragma optimize(on)
#pragma debug(off)

varying vec3 normal_frag;
varying vec4 colour_frag;
varying vec3 reflectdir_frag;
varying vec3 coords_camlocal_frag;

const vec3 lightdir = vec3(0.267261, 0.534522, 0.801784);

void main() {
  const vec3 colour_spec = vec3(0.95, 0.92, 0.84) * 0.6;       // 2268 Pearly Gates
  //const vec3 colour_spec = vec3(1.00, 0.97, 0.89);       //  Pearly Gates normalised to 1

  float lambertian = max(dot(lightdir, normalize(normal_frag)), 0.0);
  float specular = 0.0;
  if(lambertian > 0.0) {
    float specangle = max(dot(reflectdir_frag, normalize(coords_camlocal_frag)), 0.0);
    specular = pow(specangle, 10.0);
  }

  gl_FragColor.rgb = (colour_frag.rgb * lambertian) +
                     (colour_spec     * specular);
  gl_FragColor.a = colour_frag.a;
}
