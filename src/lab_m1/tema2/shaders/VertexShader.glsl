#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texCoord;

float random(vec2 p)
{
    return fract(sin(dot(p, vec2(1.234, 3.789))) * 43758.5453);
}

float fade(float t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float grad(int hash, vec2 p)
{
    int h = hash & 15;
    float u = h < 8 ? p.x : p.y;
    float v = h < 4 ? p.y : (h == 12 || h == 14 ? p.x : 0.0);
    return (h % 2 == 0 ? u : -u) + (h % 4 == 0 ? v : -v);
}

float perlinNoise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    int a = int(i.x) + int(i.y) * 57;
    int b = a + 1;
    int c = a + 57;
    int d = c + 1;

    float fadeX = fade(f.x);
    float fadeY = fade(f.y);

    float aa = grad(a, f);
    float ba = grad(b, f - vec2(1.0, 0.0));
    float ca = grad(c, f - vec2(0.0, 1.0));
    float da = grad(d, f - vec2(1.0, 1.0));

    float x1 = mix(aa, ba, fadeX);
    float x2 = mix(ca, da, fadeX);
    return mix(x1, x2, fadeY);
}

float noise(vec2 p)
{
    return perlinNoise(p * 0.3);
}


void main() {
    float frequency = 0.01;
    float amplitude = 10.0;
    float height = noise(position.xz * frequency) * amplitude;

    vec3 updated_position = position;
    updated_position.y += height;

    frag_position = vec3(Model * vec4(updated_position, 1.0));
    frag_normal = normalize(mat3(transpose(inverse(Model))) * normal);
    frag_texCoord = updated_position.xz;

    gl_Position = Projection * View * Model * vec4(updated_position, 1.0);
}
