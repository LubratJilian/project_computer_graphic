#version 330 core

in vec3 color;
in  vec2 uv;

out vec4 finalColor;
uniform sampler2D textureSampler;
uniform int texture3DSize;



void main()
{
    finalColor = vec4(color,1.);//vec4(texture(textureSampler,uv).rgb,1.);

}
