#version 400

out vec4 FragColor;

in vec3 fn;
in vec3 vertPos;

uniform int mode;
uniform vec3 lightDirection = vec3(-1.0, -0.25, 0.0);

const vec4 ambientColor = vec4(0.075, 0.025, 0.0, 1.0);
const vec4 diffuseColor = vec4((0.25 * 0.75), (0.25 * 0.25), 0.0, 1.0);
const vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0);
const float shininess = 20.0;
const vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
const float irradiPerp = 1.0;

vec3 toonBRDF(vec3 lightDir, 
vec3 viewDir, 
vec3 normal, 
vec3 phongDiffuseCol, 
vec3 phongSpecularCol, 
float phongShininess) 
{
   vec3 color = phongDiffuseCol;
   vec3 reflectDir = reflect(-lightDir, normal);
   float specDot = max(dot(reflectDir, viewDir), 0.0);
   if (specDot > 0.9)
   {
        specDot = 1.0;
        color += pow(specDot, phongShininess) * phongSpecularCol;
   }
   return color;
}

void main()
{
   vec3 lightDir = normalize(-lightDirection);
   vec3 viewDir = normalize(-vertPos);
   vec3 n = normalize(fn);

   vec3 radiance = ambientColor.rgb;

   float irradiance = dot(lightDir, n);
   vec3 brdf = toonBRDF(lightDir, viewDir, n, diffuseColor.rgb, specularColor.rgb, shininess);
   if(irradiance > 0.0) {
        
      radiance += brdf * 0.5 * lightColor.rgb;
   }
   else if (irradiance < -0.85)
   {
      radiance = brdf + 0.5 * lightColor.rgb;
   }
   radiance = pow(radiance, vec3(1.0 / 2.2));
   FragColor.rgb = radiance;
   FragColor.a = 1.0;
}