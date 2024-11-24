#version 330 core

in vec3 color;

// TODO: To add UV input to this fragment shader 
in  vec2 uv;
// TODO: To add the texture sampler

out vec4 finalColor;
uniform sampler2D textureSampler;


void main()
{

    finalColor = vec4(texture(textureSampler,uv).rgb,1.);

}
