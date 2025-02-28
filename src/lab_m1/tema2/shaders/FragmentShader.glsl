#version 330 core

in vec3 frag_normal;
in vec3 frag_position;
in vec2 frag_texCoord;

layout(location = 0) out vec4 out_color;

float random(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main()
{
    float frequency = 0.1;

    float noise_value = noise(frag_texCoord * frequency);

    vec3 color1 = vec3(1.0, 0.75, 0.8);
    vec3 color2 = vec3(0.5, 0.0, 0.5);

    vec3 final_color = mix(color1, color2, noise_value);

    out_color = vec4(final_color, 1.0);
}