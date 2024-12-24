#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;

// Output data, to be interpolated for each fragment
out vec4 WorldPosition;
out vec3 worldNormal;
out vec2 UV;

uniform mat4 MVP;
uniform mat4 jointMatrices[25];
uniform mat4 model;

void main() {
    // Transform vertex
    //vec4 skinMatrix = a_weight.x * jointMatrices[int(a_joint.x)] + a_weight.y * jointMatrices[int(a_joint.y)] + a_weight.z * jointMatrices[int(a_joint.z)] + a_weight.w * jointMatrices[int(a_joint.w)];
    //gl_Position = MVP * skinMatrix * vec4(vertexPosition, 1.0);
    mat4 skinnedPosition;
    vec4 a_weight2 = vec4(1,0,0,0);
    for (int i = 0; i < 4; i++) { skinnedPosition += a_weight[i] * jointMatrices[int(a_joint[i])]; }
    //skinnedPosition = a_weight.x*jointMatrices[int(a_joint.x)]+a_weight.y*jointMatrices[int(a_joint.y)]+ a_weight.z*jointMatrices[int(a_joint.z)]+ a_weight.w*jointMatrices[int(a_joint.w)];

    gl_Position = MVP *model* skinnedPosition * vec4(vertexPosition, 1.0);


    //gl_Position =  MVP * vec4(vertexPosition, 1.0);

    // World-space geometry 
    WorldPosition =  model*(skinnedPosition * vec4(vertexPosition, 1.0));

    mat3 skinnedPosition2;
    for (int i = 0; i < 4; i++) { skinnedPosition2 += a_weight[i] * mat3(jointMatrices[int(a_joint[i])]); }

    worldNormal = skinnedPosition2 * vertexNormal;
    UV = vertexUV;
}