#version 410

layout (location = 0) in vec3 pos;
layout (location =1 ) in vec3 tangent;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

out vec3 finalColor;

out vec2 fragUV;
out vec3 fragPos;
out vec3 worldPosition;
out vec4 shadowFinal;

out mat3 TBN;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 normalMatrix;

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

uniform sampler2D fboTex;
uniform mat4 shadowMatrix;

uniform float isShadows = 0;

void main()
{
    gl_Position = mvp * vec4(pos, 1.0);
    worldPosition = (model * vec4(pos, 1.0)).xyz;
    fragPos = pos;

    fragUV = vec2(uv.x, 1.0-uv.y);

    vec3 T = normalize(normalMatrix * tangent.xyz);
    vec3 B = normalize(normalMatrix * cross(tangent.xyz, normal));
    vec3 N = normalize(normalMatrix * normal);

    TBN = mat3(T, B, N);

	if(isShadows == 0){
	    //Lighting calc
    finalColor = vec3(0);

	//gamma correction
	vec3 diffColor = pow(texture(diffuseTex, fragUV).rgb, vec3(2.2));

	//normal mapping
	vec3 tsNormal = texture(normalTex, fragUV).xyz *2-1;
	vec3 wsNormal = TBN * tsNormal;

	float nDotL = max(0.0, dot(wsNormal, lightDir));

	//irradience - decoding gamma
	vec3 envIrradience = pow(textureLod(envMap, wsNormal, 99).rgb, vec3(2.2));

	vec3 light = max(vec3(0.1), lightColor);
	finalColor += envIrradience * light * nDotL;
    
    finalColor = diffColor * finalColor;

	} else {
	//shadow lighting calcs

	vec3 diffColor = pow(texture(diffuseTex, fragUV).rgb, vec3(2.2));

	vec4 shadowCoords = shadowMatrix * vec4(worldPosition, 1.0);
	vec3 texCoords = vec3(0.5) + 0.5 * shadowCoords.xyz / shadowCoords.w;

	vec3 light = max(vec3(0.1), lightColor);
	vec3 shadows = vec3(light * step(texCoords.z,(texture(fboTex, texCoords.xy).r )));

	finalColor = vec3(0);

	//gamma correction
	 diffColor = pow(texture(diffuseTex, fragUV).rgb, vec3(2.2));

	//normal mapping
	 vec3 tsNormal = texture(normalTex, fragUV).xyz *2-1;
	 vec3 wsNormal = TBN * tsNormal;

	 float nDotL = max(0.0, dot(wsNormal, lightDir));

	//irradience - decoding gamma
	 vec3 envIrradience = pow(textureLod(envMap, wsNormal, 99).rgb, vec3(2.2));

	finalColor += ambientColor + envIrradience + lightColor * nDotL;
    shadowFinal = vec4(pow(diffColor*finalColor*shadows, vec3(1.0/2.2)),1.0);
	}

}
