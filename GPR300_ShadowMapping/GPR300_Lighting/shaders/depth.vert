#version 450                 
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;

out vec2 outUv;

uniform vec2 positionAt;
uniform float widthAt;
uniform float heightAt;

vec2 getTransformed(vec2 pos, vec2 point, float width, float height)
{
    float xPoint = (pos.x * width) + point.x;
    float yPoint = (pos.y * height) + point.y;
    return vec2(xPoint, yPoint);
}

void main(){    
    outUv = vUv;
    vec2 truPos = getTransformed(vPos.xy, positionAt, widthAt, widthAt);
    gl_Position = vec4(truPos.x, truPos.y, 0, 1.0);
} 