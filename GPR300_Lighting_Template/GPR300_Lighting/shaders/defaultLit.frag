#version 450                          
out vec4 FragColor;

in vec3 Normal;

uniform vec3 _LightPos;
uniform vec3 _CameraPos;

const int LIGHTS = 8;

struct Material{
    vec3 _Color;
    float _AmbientK;
    float _DiffuseK;
    float _SpecularK;
    float _Shininess;
};

struct DirectionalLight{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight{
    vec3 position;
    vec3 color;
    float intensity;
    float radius;  
};

float attenuation(vec3 frag, PointLight pl)
{
    return clamp(pow((1 - (length(pl.position - frag) / pl.radius)), 4), 0, 1);
};

struct ConeLight
{
    vec3 direction;
    vec3 position;
    vec3 color;
    float intensity;
    float minAngle;
    float maxAngle;
    float fallOff;
    float range;
};

float cAttenuation(vec3 frag, ConeLight pl)
{
    return clamp(pow((1 - (length(pl.position - frag) / pl.range)), 4), 0, 1);
};

float coneAttenuation(vec3 frag, ConeLight cl)
{
    float angl = dot(normalize(cl.direction), normalize(frag - cl.position));
    return clamp(dot(((angl - cos(radians(cl.maxAngle)))/(cos(radians(cl.minAngle)) - cos(radians(cl.maxAngle)))), cl.fallOff), 0, 1);
}


in struct Vertex
{
    vec3 WorldPos;
    vec3 WorldNormal;
}vs_out;

uniform DirectionalLight light;

uniform Material material;
uniform DirectionalLight[LIGHTS] dLights;
uniform PointLight[LIGHTS] pLights;
uniform ConeLight[LIGHTS] cLights;

vec3 getAmbient()
{
    vec3 toRetu;

    for(int i = 0; i < LIGHTS; i++)
    {
        toRetu += material._AmbientK * dLights[i].color * dLights[i].intensity;
        toRetu += material._AmbientK * pLights[i].color * pLights[i].intensity;
        toRetu += material._AmbientK * cLights[i].color * cLights[i].intensity;
    }

    return toRetu;
}

vec3 getDiffuse()
{
    vec3 toRetu;

    for(int i = 0; i < LIGHTS; i++)
    {
        toRetu += material._DiffuseK * dot(normalize(vs_out.WorldNormal), normalize(-dLights[i].direction)) * dLights[i].color * dLights[i].intensity;
        toRetu += material._DiffuseK * dot(normalize(vs_out.WorldNormal), normalize(pLights[i].position -  vs_out.WorldPos)) * pLights[i].color * pLights[i].intensity * attenuation(vs_out.WorldPos, pLights[i]);
        toRetu += material._DiffuseK * dot(normalize(vs_out.WorldNormal), normalize(cLights[i].position -  vs_out.WorldPos)) * cLights[i].color * cLights[i].intensity * cAttenuation(vs_out.WorldPos, cLights[i]) * coneAttenuation(vs_out.WorldPos, cLights[i]);
    }

    return toRetu;
}

vec3 getSpecular()
{
    vec3 toRetu;

    float bf = dot(normalize(vs_out.WorldNormal), normalize(_CameraPos - vs_out.WorldPos));

    if(bf <= 0)
    {
        bf = 0;
    }

    for(int i = 0; i < LIGHTS; i++)
    {
        toRetu += material._SpecularK * pow(bf, material._Shininess) * dLights[i].color * dLights[i].intensity;
        toRetu += material._SpecularK * pow(bf, material._Shininess) * pLights[i].color * pLights[i].intensity;
        toRetu += material._SpecularK * pow(bf, material._Shininess) * cLights[i].color * cLights[i].intensity;
    }

    return toRetu;
}

void main(){ 
    vec3 blinnPhong = getAmbient() + getDiffuse() + getSpecular();
    FragColor = vec4(blinnPhong * material._Color, 1.0f);
}
