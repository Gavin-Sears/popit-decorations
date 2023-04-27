#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;
layout (location = 2) in vec2 vUV;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

out vec3 fn;
out vec3 vertPos;
out vec2 uv;
void main()
{
   uv = vUV;
   fn = normalize(NormalMatrix * vNormals);
   vec4 vertPos4 = ModelViewMatrix * vec4(vPos, 1.0);
   vertPos = vec3(vertPos4) / vertPos4.w;
   gl_Position = MVP * vec4(vPos, 1.0);
}
