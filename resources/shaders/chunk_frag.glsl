#version 120
#pragma optimize(on)
#pragma debug(off)

varying vec3 normal_frag;
varying vec3 reflectdir_frag;
varying vec3 coords_camlocal_frag;

void main() {
  // Colour charts: http://www.materials-world.com/paint-colors/pratt_and_lambert/calibrated/pratt-and-lambert-calibrated-index.htm
  //const vec3 colour_lamb = vec3(0.60, 0.62, 0.58);       // 2245 Winter Cloud
  //const vec3 colour_lamb = vec3(0.98, 1.09, 0.96);       // 2245 Winter Cloud normalised to 1
  //const vec3 colour_lamb = vec3(0.80, 0.80, 0.77);       // 2244 Pearl White
  //const vec3 colour_lamb = vec3(1.00, 1.00, 0.96);       // 2244 Pearl White normalised to 1
  //const vec3 colour_lamb = vec3(0.74, 0.77, 0.80);       // 2329 Kid Glove
  const vec3 colour_lamb = vec3(0.92, 0.95, 1.00);       // 2329 Kid Glove normalised to 1
  const vec3 colour_spec = vec3(0.95, 0.92, 0.84) * 0.6;       // 2268 Pearly Gates
  //const vec3 colour_spec = vec3(1.00, 0.97, 0.89);       //  Pearly Gates normalised to 1

  const vec3 lightdir = vec3(0.267261, 0.534522, 0.801784);

  float lambertian = dot(lightdir, normalize(normal_frag));

  float specular = 0.0;
  if(lambertian > 0.0) {
    float specangle = max(dot(reflectdir_frag, normalize(coords_camlocal_frag)), 0.0);
    specular = pow(specangle, 10.0);
  }

  gl_FragColor.rgb = (colour_lamb * lambertian) +
                     (colour_spec * specular);
  gl_FragColor.a = 1.0;
}
