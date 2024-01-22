#version 410

in vec3 worldPosition;

out vec4 outColor;

uniform sampler2D fboTex;
uniform mat4 shadowMatrix;
uniform float planeLight;

in vec2 fragUV;
in mat3 TBN;

 uniform vec3 lightDir;
 uniform vec3 lightColor;

 uniform vec3 cameraPosition;

 uniform vec3 pointPos;
 uniform vec3 pointColor;

 uniform samplerCube cubemap;

uniform vec3 ambientColor = vec3(0.1);

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

 uniform samplerCube envMap;

void main()
{
	vec4 shadowCoords = shadowMatrix * vec4(worldPosition, 1.0);

	vec3 texCoords = vec3(0.5) + 0.5 * shadowCoords.xyz / shadowCoords.w;

	vec3 light = max(vec3(0.1), lightColor);
	vec3 shadows = vec3(light * step(texCoords.z,(texture(fboTex, texCoords.xy).r )));

	vec3 finalColor = vec3(0);

	//gamma correction
	vec3 diffColor = pow(texture(diffuseTex, fragUV).rgb, vec3(2.2));

	//normal mapping
	vec3 tsNormal = texture(normalTex, fragUV).xyz *2-1;
	vec3 wsNormal = TBN * tsNormal;

	float nDotL = max(0.0, dot(wsNormal, lightDir));

	//irradience - decoding gamma
	vec3 envIrradience = pow(textureLod(envMap, wsNormal, 99).rgb, vec3(2.2));

	finalColor += ambientColor + envIrradience + lightColor * nDotL;

	outColor = vec4(pow(diffColor*finalColor*shadows, vec3(1.0/2.2)),1.0);
}

