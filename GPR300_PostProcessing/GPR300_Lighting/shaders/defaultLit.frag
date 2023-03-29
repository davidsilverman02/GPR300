#version 450                          
out vec4 FragColor;

in mat3 TBN;
in mat3 Locator;

uniform vec3 _LightPos;
uniform vec3 _CameraPos;

uniform mat4 _NormalMatrix;
uniform sampler2D _SandTexture;
uniform sampler2D _BrickTexture;
uniform sampler2D _MesoTexture;

const int LIGHTS = 8;

struct Material{
    vec3 _Color;
    float _AmbientK;
    float _DiffuseK;
    float _SpecularK;
    float _Shininess;
    float _normalMapIntensity;
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

// M is the model matrix, t stands for transpose
in struct Vertex
{
    vec3 WorldPos;
    vec3 WorldNormal;
    vec2 UV;
    vec3 Tangent;
}vs_out;

uniform DirectionalLight light;

uniform Material material;
uniform DirectionalLight[LIGHTS] dLights;
uniform PointLight[LIGHTS] pLights;
uniform ConeLight[LIGHTS] cLights;

vec3 calcENormal(vec3 smoothNormal, vec3 bumpyNormal)
{
    return mix(smoothNormal, bumpyNormal, material._normalMapIntensity);
}

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

vec3 getDiffuse(vec3 norm)
{
    vec3 toRetu;

    for(int i = 0; i < LIGHTS; i++)
    {
        toRetu += material._DiffuseK * clamp(dot(normalize(norm), normalize(-dLights[i].direction)), 0, 1) * dLights[i].color * dLights[i].intensity;
        toRetu += material._DiffuseK * clamp(dot(normalize(norm), normalize(pLights[i].position -  vs_out.WorldPos)), 0, 1) * pLights[i].color * pLights[i].intensity * attenuation(vs_out.WorldPos, pLights[i]);
        toRetu += material._DiffuseK * clamp(dot(normalize(norm), normalize(cLights[i].position -  vs_out.WorldPos)), 0, 1) * cLights[i].color * cLights[i].intensity * cAttenuation(vs_out.WorldPos, cLights[i]) * coneAttenuation(vs_out.WorldPos, cLights[i]);
    }

    return toRetu;
}

vec3 getSpecular(vec3 norm)
{
    vec3 toRetu;

    vec3 V = normalize(_CameraPos - vs_out.WorldPos);
    vec3 L;

    float bf;

    for(int i = 0; i < LIGHTS; i++)
    {
        L = normalize(-dLights[i].direction);
        bf = clamp(dot(normalize(norm), normalize(V + L)), 0, 1);
        toRetu += material._SpecularK * pow(bf, material._Shininess) * dLights[i].color * dLights[i].intensity;
        L = normalize(pLights[i].position -  vs_out.WorldPos);
        bf = clamp(dot(normalize(norm), normalize(V + L)), 0, 1);
        toRetu += material._SpecularK * pow(bf, material._Shininess) * pLights[i].color * pLights[i].intensity;
        L = normalize(cLights[i].position -  vs_out.WorldPos);
        bf = clamp(dot(normalize(norm), normalize(V + L)), 0, 1);
        toRetu += material._SpecularK * pow(bf, material._Shininess) * cLights[i].color * cLights[i].intensity;
    }

    return toRetu;
}

void main(){
    vec3 mesoNormal = texture(_MesoTexture, vs_out.UV).xyz;
    mesoNormal = mesoNormal * 2.0f - 1.0f;
    mesoNormal = calcENormal(vec3(0, 0, 1), mesoNormal);
    mesoNormal = normalize(TBN * mesoNormal);
    vec4 tex1 = texture(_SandTexture, vs_out.UV);
    vec4 tex2 = texture(_BrickTexture, vs_out.UV);
    vec3 vecA = vec3(tex1 * tex2).rgb;
    vec3 blinnPhong = getAmbient() + getDiffuse(mesoNormal) + getSpecular(mesoNormal);
    FragColor = vec4(vecA * blinnPhong * material._Color, 1.0f);
    //FragColor = vec4(mesoNormal, 1.0f);
}
