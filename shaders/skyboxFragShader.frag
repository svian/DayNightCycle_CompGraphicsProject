#version 410

in vec3 fragPos;

out vec4 outColor;

 uniform samplerCube cubemap;
 uniform vec3 lightColor;

void main()
{

	vec3 light = max(vec3(0.1), lightColor);
	outColor = vec4(texture(cubemap, fragPos).rgb * light , 1.0);
}
