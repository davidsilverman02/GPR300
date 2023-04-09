#version 450    
out vec4 FragColor;

in vec2 outUv;

uniform sampler2D _ShadowTexture;

void main()
{
	float smDepth = texture(_ShadowTexture, outUv).r;
	FragColor = vec4(smDepth, smDepth, smDepth, 1.0);
}