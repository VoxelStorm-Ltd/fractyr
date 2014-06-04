#version 120
#pragma optimize(on)
#pragma debug(off)

varying vec3 normal_frag;
varying vec3 reflectdir_frag;
varying vec3 coords_camlocal_frag;

void main() {
  const vec3 lightdir = vec3(0.0, 0.8414709848, -0.54030230586);

  float lambertian = dot(lightdir, normalize(normal_frag));

  float specular = 0.0;
  if(lambertian > 0.0) {
    float specangle = max(dot(reflectdir_frag, normalize(coords_camlocal_frag)), 0.0);
    specular = pow(specangle, 50.0);
  }

  gl_FragColor.rgb = (vec3(1.0, 1.0, 1.0) * lambertian) +
                     (vec3(1.0, 1.0, 1.0) * specular);
  gl_FragColor.a = 1.0;
}
