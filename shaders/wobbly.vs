#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;
layout (location = 2) in vec2 vTextureCoords;

uniform float time;
uniform float maxRange;
uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

out vec3 fn;
out vec3 vertPos;
void main()
{
   fn = normalize(NormalMatrix * vNormals);
   vec4 vertPos4 = ModelViewMatrix * vec4(vPos, 1.0);
   vertPos = vec3(vertPos4) / vertPos4.w;
   vec3 norm = (fn + vec3(1.0, 1.0, 1.0) / 2.0);
   float posDis = (vPos.y) * (1000 / maxRange);
   float displace = sin((time * 20.0) + posDis) * (maxRange / 20) + (maxRange / 40);
   gl_Position = MVP * vec4(vPos + (norm * 0.1 * displace), 1.0);
}