#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

uniform mat4 _NormalMatrix;

out vec3 Normal;

out struct Vertex
{
    vec3 WorldPos;
    vec3 WorldNormal;
    vec2 UV;
}vs_out;

void main(){    
    
    vs_out.WorldPos = vec3(_Model * vec4(vPos, 1.0));
    vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;
    vs_out.UV = vUv;
    gl_Position = _Projection * _View * _Model * vec4(vPos,1);
}
