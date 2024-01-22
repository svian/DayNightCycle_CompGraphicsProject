#version 410

layout (location = 0) in vec3 pos;
layout (location =1 ) in vec3 tangent;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

out vec2 fragUV;
out vec3 fragPos;
out vec3 worldPosition;

out mat3 TBN;
out float light;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 normalMatrix;

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
}
