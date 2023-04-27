#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;
layout (location = 2) in vec2 vTextureCoords;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform vec3 lightDirection = vec3(-1.0, -0.25, 0.0);
uniform int mode;

out vec4 forFragColor;

const vec4 ambientColor = vec4(0.01, 0.0, 0.0, 1.0);
const vec4 diffuseColor = vec4(0.25, 0.0, 0.0, 1.0);
const vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0);
const float shininess = 20.0;
const vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
const float irradiPerp = 1.0;

vec3 phongBRDF(vec3 lightDir, 
vec3 viewDir, 
vec3 normal, 
vec3 phongDiffuseCol, 
vec3 phongSpecularCol, 
float phongShininess) 
{
  vec3 color = phongDiffuseCol;
  vec3 reflectDir = normalize(reflect(-lightDir, normal));
  float specDot = max(dot(reflectDir, viewDir), 0.0);
  color += pow(specDot, phongShininess) * phongSpecularCol;
  return color;
}

void main()
{
   vec4 vertPos4 = ModelViewMatrix * vec4(vPos, 1.0);
   vec3 vertPos = vec3(vertPos4) / vertPos4.w;
   vec3 n = normalize(vec3(NormalMatrix * vNormals));

   vec3 lightDir = normalize(-lightDirection);
   vec3 viewDir = normalize(-vertPos);

   vec3 radiance = ambientColor.rgb;

   float irradiance = max(dot(lightDir, n), 0.0) * irradiPerp;
   if(irradiance > 0.0)
   {
      vec3 brdf = phongBRDF(lightDir, viewDir, n, diffuseColor.rgb, specularColor.rgb, shininess);
      radiance += brdf * irradiance * lightColor.rgb;
   }
   
   radiance = pow(radiance, vec3(1.0 / 2.2)); // gamma correction
   forFragColor = vec4(radiance, 1.0);
   gl_Position = MVP * vec4(vPos, 1.0);
}
