#version 400

out vec4 FragColor;

in vec3 fn;
in vec3 vertPos;
in vec2 uv;

uniform sampler2D Image;

uniform bool text;
uniform vec4 diffuseColor;
uniform vec3 lightDirection = vec3(-1.0, -0.25, 0.0);

const vec4 ambientColor = vec4(0.05, 0.0, 0.0, 1.0);
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
   vec3 reflectDir = reflect(-lightDir, normal);
   float specDot = max(dot(reflectDir, viewDir), 0.0);
   color += pow(specDot, phongShininess) * phongSpecularCol;
   return color;
}

void main()
{
   vec3 lightDir = normalize(-lightDirection);
   vec3 viewDir = normalize(-vertPos);
   vec3 n = normalize(fn);
   
   vec4 tColor = diffuseColor;
   if (text)
   {
      tColor = texture(Image, uv);
      if ((tColor.r < 0.9) && (tColor.g < 0.9) && (tColor.b < 0.9) &&
      (tColor.r > 0.1) && (tColor.g > 0.1) && (tColor.b > 0.1))
      {
         tColor = vec4(
            diffuseColor.r,
            diffuseColor.g,
            diffuseColor.b,
            1.0f
         );
      }
   }
   // was ambientColor
   vec3 radiance = (tColor.rgb * 0.2);

   float irradiance = max(dot(lightDir, n), 0.0) * irradiPerp;
   if(irradiance > 0.0) {
      vec3 brdf = phongBRDF(lightDir, viewDir, n, tColor.rgb, specularColor.rgb, shininess);
      radiance += brdf * irradiance * lightColor.rgb;
   }
   radiance = pow(radiance, vec3(1.0 / 2.2));

   FragColor.rgb = radiance;
   FragColor.a = 1.0;
}
