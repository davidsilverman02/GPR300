#version 450   

in mat3 TBN;
in mat3 Locator;

uniform vec3 _LightPos;
uniform vec3 _CameraPos;

in struct Vertex
{
    vec3 WorldPos;
    vec3 WorldNormal;
    vec2 UV;
    vec3 Tangent;
}vs_out;

void main()
{

}