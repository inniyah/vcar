uniform sampler2D        texture0; // billboard texture
uniform sampler2D        texture1; // light texture
uniform sampler2DShadow  texture2; // shadow map

varying vec4 diffuseColor, ambientColor, specularColor;
varying vec3 surfaceNormal, lightDir, halfVector;

void main(void)
{
  vec4 col0 = texture2D(texture0, gl_TexCoord[0].xy);
  vec4 col1 = texture2DProj(texture1, gl_TexCoord[1]) * diffuseColor;

#if 1
  vec4 col2 = shadow2DProj(texture2, gl_TexCoord[2]);
#else

  vec4 cx = gl_TexCoord[2] / gl_TexCoord[2].q;

  vec4 c0 = cx + vec4(-0.001, 0.000, 0.0, 0.0);
  vec4 c1 = cx + vec4( 0.001, 0.000, 0.0, 0.0);
  vec4 c2 = cx + vec4( 0.000,-0.001, 0.0, 0.0);
  vec4 c3 = cx + vec4( 0.000, 0.001, 0.0, 0.0);

  vec4 col2 = 
    shadow2DProj(texture2, c0) +
    shadow2DProj(texture2, c1) +
    shadow2DProj(texture2, c2) +
    shadow2DProj(texture2, c3);
  col2 = col2 / 4.0;
#endif

  vec4 emissionColor = gl_FrontMaterial.emission;

  if (emissionColor.g > 0.5)
  {
    gl_FragColor = col0.a * (col0 + gl_FrontMaterial.emission);
    return;
  }

  vec4 color = ambientColor + emissionColor;

  vec3 n = normalize(surfaceNormal);
  float NdotL = max( dot( n, normalize(lightDir) ), 0.0 );

  if (NdotL > 0.0) 
  {
    vec4 Kd = diffuseColor * NdotL;

    color += 0.35 * Kd + col2 * col1 * Kd;

// We disable specular computation. It is too expensive.
//    vec3 halfV = normalize(halfVector);
//    float NdotHV = max(dot(n,halfV),0.0);
//     color += gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess) * col1;
  }

  gl_FragColor = color + 0.01*col0;
}

