#version 450                 
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;

out vec2 outUv;

void main(){    
    outUv = vUv;
    gl_Position = vec4(vPos.x, vPos.y, 0, 1.0);
} 