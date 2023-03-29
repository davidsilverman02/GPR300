#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

out mat3 TBN;
out mat3 Locator;

out struct Vertex
{
    vec3 WorldPos;
    vec3 WorldNormal;
    vec2 UV;
    vec3 Tangent;
}vs_out;

void main(){    
    vs_out.WorldPos = vec3(_Model * vec4(vPos, 1.0));
    vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;
    vs_out.UV = vUv;
    vs_out.Tangent = vTangent;
    vec3 bitangent = cross(vNormal, vs_out.Tangent);
    TBN[0] = vs_out.Tangent;
    TBN[1] = bitangent;
    TBN[2] = vNormal;
    TBN = transpose(inverse(mat3(_Model))) * TBN;
    gl_Position = _Projection * _View * _Model * vec4(vPos,1);
}
