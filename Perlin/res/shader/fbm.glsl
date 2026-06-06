uniform int u_octaves;
uniform float u_persistence;
uniform float u_frequency;
uniform float u_lacunarity;
uniform float u_scale;
uniform float u_amplitude;
uniform float u_offset;
uniform int u_res;

/**
 * Deterministische Zufallszahl aus einem 2D-Vektor.
 * @param src der Vektor aus dem der Hash berechnet wird
 * @returns Hashwert des uebergebenen Vektors
 */
uvec3 murmurHash33(uvec3 src) {
    const uint M = 0x5bd1e995u;
    uvec3 h = uvec3(1190494759u, 2147483647u, 3559788179u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

/**
 * Konvertiert den uebergebenen Vektor in einen deterministischen Float-Hashwert.
 * @param src der Vektor aus dem der Hash berechnet wird
 * @returns Hashwert des uebergebenen Vektors im Bereich [0, 1)
 */
vec3 hash33(vec3 src) {
    uvec3 h = murmurHash33(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

/**
 * Erstellt einen Noise-Wert anhand des uebergebenen Vektors (Perlin-Noise).
 * @param inp der Vektor anhand die Nosie berechnet wird
 * @returns Noise im Bereich -1 bis 1
 */
float noise(vec2 inp) {
    vec3 x = vec3(inp.x, 0, inp.y);
    vec3 i = floor(x);
    vec3 f = fract(x);
        
    // quintic interpolant
    vec3 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    vec3 du = 30.0*f*f*(f*(f-2.0)+1.0);
        
    // gradients
    vec3 ga = hash33( i+vec3(0.0,0.0,0.0) ) * 2.0 - 1.0;
    vec3 gb = hash33( i+vec3(1.0,0.0,0.0) ) * 2.0 - 1.0;
    vec3 gc = hash33( i+vec3(0.0,1.0,0.0) ) * 2.0 - 1.0;
    vec3 gd = hash33( i+vec3(1.0,1.0,0.0) ) * 2.0 - 1.0;
    vec3 ge = hash33( i+vec3(0.0,0.0,1.0) ) * 2.0 - 1.0;
    vec3 gf = hash33( i+vec3(1.0,0.0,1.0) ) * 2.0 - 1.0;
    vec3 gg = hash33( i+vec3(0.0,1.0,1.0) ) * 2.0 - 1.0;
    vec3 gh = hash33( i+vec3(1.0,1.0,1.0) ) * 2.0 - 1.0;
        
    // projections
    float va = dot( ga, f-vec3(0.0,0.0,0.0) );
    float vb = dot( gb, f-vec3(1.0,0.0,0.0) );
    float vc = dot( gc, f-vec3(0.0,1.0,0.0) );
    float vd = dot( gd, f-vec3(1.0,1.0,0.0) );
    float ve = dot( ge, f-vec3(0.0,0.0,1.0) );
    float vf = dot( gf, f-vec3(1.0,0.0,1.0) );
    float vg = dot( gg, f-vec3(0.0,1.0,1.0) );
    float vh = dot( gh, f-vec3(1.0,1.0,1.0) );

    // interpolations
    vec4 res = vec4(va + u.x*(vb-va) + u.y*(vc-va) + u.z*(ve-va) + u.x*u.y*(va-vb-vc+vd) + u.y*u.z*(va-vc-ve+vg) + u.z*u.x*(va-vb-ve+vf) + (-va+vb+vc-vd+ve-vf-vg+vh)*u.x*u.y*u.z,    // value
                ga + u.x*(gb-ga) + u.y*(gc-ga) + u.z*(ge-ga) + u.x*u.y*(ga-gb-gc+gd) + u.y*u.z*(ga-gc-ge+gg) + u.z*u.x*(ga-gb-ge+gf) + (-ga+gb+gc-gd+ge-gf-gg+gh)*u.x*u.y*u.z +   // derivatives
                du * (vec3(vb,vc,ve) - va + u.yzx*vec3(va-vb-vc+vd,va-vc-ve+vg,va-vb-ve+vf) + u.zxy*vec3(va-vb-ve+vf,va-vb-vc+vd,va-vc-ve+vg) + u.yzx*u.zxy*(-va+vb+vc-vd+ve-vf-vg+vh) ));
    return res.x;
}

/**
 * Deterministische Zufallszahl aus einem 2D-Vektor.
 * @param src der Vektor aus dem der Hash berechnet wird
 * @returns Hashwert des uebergebenen Vektors
 */
uint murmurHash(uvec2 src) {
    const uint M = 0x5bd1e995u;
    uint h = 1190494759u;
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

/**
 * Konvertiert den uebergebenen Vektor in einen deterministischen Float-Hashwert.
 * @param src der Vektor aus dem der Hash berechnet wird
 * @returns Hashwert des uebergebenen Vektors im Bereich [0, 1)
 */
float hash(in vec2 src) {
    uint h = murmurHash(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

/**
 * Erstellt einen Noise-Wert anhand des uebergebenen Vektors (Value-Noise).
 * @param der Vektor anhand die Nosie berechnet wird
 * @returns Noise im Bereich -1 bis 1
 */
float valueNoise(in vec2 p) {
    vec2 i = floor(p);

    vec2 f = fract(p);
    vec2 u = smoothstep(vec2(0.0), vec2(1.0), f);

    float val = mix(mix(hash(i + vec2(0.0, 0.0)), 
                        hash(i + vec2(1.0, 0.0)), u.x),
                    mix(hash(i + vec2(0.0, 1.0)), 
                        hash(i + vec2(1.0, 1.0)), u.x), u.y);
  return val * 2.0 - 1.0;
}

/**
 * Fractal Brownian Motion an der Stelle des uebergebenen Vektors.
 * @param p der Vektor aus dem der FBM berechnet wird
 * @returns fbm-Wert
 */
float fbm(in vec2 p) {
    p /= u_scale;
    p += vec2(u_res / 2);
    float amplitude = u_amplitude;
    float frequency = u_frequency;
    float total = 0.0;

    for (int i = 0; i < u_octaves; ++i) {
        float noiseValue = valueNoise(p * frequency + vec2(u_offset) * frequency);
        total += noiseValue * amplitude;
        amplitude *= u_persistence;
        frequency *= u_lacunarity;
    }

    total = (total + 1) / 2;

    return total;
}
