#version 410

in vec3 finalColor;

out vec4 outColor;

void main()
{
	outColor = vec4(pow(finalColor, vec3(1.0/2.2)),1.0);
}
