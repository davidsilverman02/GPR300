#version 450               
out vec4 FragColor;

in vec2 outUv;

//layout(location = 0) out vec4 FragColor1;
//layout(location = 1) out vec4 FragColor2;

uniform sampler2D screen;
uniform float time;
uniform float frequency;
uniform float amplitude;
uniform float directs;
uniform float strength;
uniform float blurSize;
uniform float width;
uniform float height;


uniform bool processing;
uniform bool inverted;
uniform bool greyed;
uniform bool waving;
uniform bool blurry;


float doubDiam = 6.28318530718;

vec4 inversion(vec4 vec)
{
    return vec4(1.0f - vec.x, 1.0f - vec.y, 1.0f - vec.z, vec.a);
}

vec4 greyscale(vec4 vec)
{
    float grey = (vec.r + vec.g + vec.b) / 3.0f;
    return vec4(grey, grey, grey, vec.a);
}

vec2 wavy(vec2 vec)
{
    vec2 pulsing = sin(time - frequency * vec);
    float dst = 2.0 *length(vec.y - 0.5);
    vec2 other = vec + amplitude * vec2(0.0, pulsing.x);
    return mix(other, vec, dst);
}

vec4 blur(vec4 vec)
{
    vec2 inUV = outUv;
    vec2 scrn = vec2(width, height);
    vec2 radi = blurSize / scrn;
    vec4 col = vec;

    for(float d = 0; d < doubDiam; d += doubDiam/strength)
    {
        for(float i = 1.0/strength; i <= 1.0; i += 1.0/strength)
        {
            col += texture(screen, inUV+vec2(cos(d), sin(d)) * radi * i);
        }
    }

    return col;
}

void main(){  
    vec2 inUV = outUv;
    if(waving && processing)
    {
        inUV = wavy(inUV);
    }
    
    vec4 basis = texture(screen, inUV);

    if(inverted && processing)
    {
        basis = inversion(basis);
    }

    if(greyed && processing)
    {
        basis = greyscale(basis);
    }

    if(blurry && processing)
    {
        basis = blur(basis);
    }

    FragColor = basis;
} 