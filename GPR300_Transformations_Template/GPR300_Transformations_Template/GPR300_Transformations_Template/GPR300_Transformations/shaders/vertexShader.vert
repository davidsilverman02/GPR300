#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;

uniform float _Time;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

out vec3 Normal;

void main(){ 
    Normal = vNormal;
    //_Projection * _View *
    gl_Position = _Projection * _View * _Model * vec4(vPos,1);
    //gl_Position = vec4(vPos,1);
    //gl_Position = vec4((vPos.x * cos(_Time)) - (vPos.z * sin(_Time)), .4 * sin(_Time) + vPos.y, (vPos.x * sin(_Time)) + (vPos.z * cos(_Time)), 1.0);
}
