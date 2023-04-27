#version 400

out vec4 FragColor;

in vec3 fn;

void main()
{
   vec3 n = ((fn + vec3(1.0, 1.0, 1.0)) / 2.0f);

   FragColor = vec4(n, 1.0f);
}