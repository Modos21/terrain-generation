uniform int u_octaves;
uniform float u_persistence;
uniform float u_frequency;
uniform float u_lacunarity;
uniform float u_scale;
uniform float u_amplitude;
uniform float u_offset;
uniform int u_res;

/**
 * Calculates a pseudo-random gradient for a given vector
 */
vec2 grad2D(in ivec2 v) {
    int h = (v.x * 1619 + v.y * 31337) & 255;
    float angle = h * (3.14159265 * 2.0 / 256.0);
    return vec2(cos(angle), sin(angle));
}

float perlin2D(in vec2 v) {
    // the closest grid-point rounded down
    ivec2 i = ivec2(floor(v));
    // position of v within 4 grid-points (from [0,0] to [1,1])
    vec2  f = fract(v);
    // smoothstep interpolation function
    vec2  u = f * f * (3.0 - 2.0 * f);

    // dot products of grid-point gradients with the vectors from f to those grid-points
    float a = dot(grad2D(i + ivec2(0, 0)), f - vec2(0, 0));
    float b = dot(grad2D(i + ivec2(1, 0)), f - vec2(1, 0));
    float c = dot(grad2D(i + ivec2(0, 1)), f - vec2(0, 1));
    float d = dot(grad2D(i + ivec2(1, 1)), f - vec2(1, 1));

    // interpolate a-b and c-d along the x-axis, then interpolate those results along the y-axis
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

/**
 * Generates layered 2D perlin noise with the result values ranging from -1.0 to 1.0
 */
float fbm2D(in vec2 v) {
    v /= u_scale;
    v += vec2(u_res / 2);
    float amplitude = u_amplitude;
    float frequency = u_frequency;
    float total = 0.0;

    for (int i = 0; i < u_octaves; ++i) {
        float noiseValue = perlin2D((v + vec2(u_offset) * frequency) * frequency);
        total += noiseValue * amplitude;
        amplitude *= u_persistence;
        frequency *= u_lacunarity;
    }

    return total;
}

/**
 * Overload method that calls fbm2D([x,y])
 */
float fbm2D(in float x, in float y) {
    return fbm2D(vec2(x, y));
}

vec3 grad3D(in ivec3 v) {
    int n = v.x * 1619 + v.y * 31337 + v.z * 6971;
    n = (n << 13) ^ n;
    n = n * (n * n * 15731 + 789221) + 1376312589;

    float x = float((n >>  0) & 256) / 255.0;
    float y = float((n >>  8) & 256) / 255.0;
    float z = float((n >> 16) & 256) / 255.0;

    return normalize(vec3(x, y, z) * 2.0 - 1.0);
}

float perlin3D(in vec3 v) {
    ivec3 i = ivec3(floor(v));
    vec3  f = fract(v);

    // 5th degree smoothstep
    vec3 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

    // 8 gradients for all grid-points around v
    float n000 = dot(grad3D(i + ivec3(0, 0, 0)), f - vec3(0, 0, 0));
    float n100 = dot(grad3D(i + ivec3(1, 0, 0)), f - vec3(1, 0, 0));
    float n010 = dot(grad3D(i + ivec3(0, 1, 0)), f - vec3(0, 1, 0));
    float n110 = dot(grad3D(i + ivec3(1, 1, 0)), f - vec3(1, 1, 0));
    float n001 = dot(grad3D(i + ivec3(0, 0, 1)), f - vec3(0, 0, 1));
    float n101 = dot(grad3D(i + ivec3(1, 0, 1)), f - vec3(1, 0, 1));
    float n011 = dot(grad3D(i + ivec3(0, 1, 1)), f - vec3(0, 1, 1));
    float n111 = dot(grad3D(i + ivec3(1, 1, 1)), f - vec3(1, 1, 1));

    // 4 interpolations along x
    float x00 = mix(n000, n100, u.x);
    float x10 = mix(n010, n110, u.x);
    float x01 = mix(n001, n101, u.x);
    float x11 = mix(n011, n111, u.x);

    // interpolate x-results along y
    float y0 = mix(x00, x10, u.y);
    float y1 = mix(x01, x11, u.y);

    // interpolate y-results along z
    return mix(y0, y1, u.z);
}

float fbm3D(in vec3 v) {
    v /= u_scale;
    v += vec3(u_res / 2);
    float amplitude = u_amplitude;
    float frequency = u_frequency;
    float total = 0.0;

    for (int i = 0; i < u_octaves; ++i) {
        float noiseValue = perlin3D((v + vec3(u_offset) * frequency) * frequency);
        total += noiseValue * amplitude;
        amplitude *= u_persistence;
        frequency *= u_lacunarity;
    }

    return total;
}

float fbm3D(in float x, in float y, in float z) {
    return fbm3D(vec3(x, y, z));
}