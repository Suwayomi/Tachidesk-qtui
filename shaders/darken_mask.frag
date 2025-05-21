#version 450

// receive the built-in TexCoord varying:
layout(location = 0) in vec2 qt_TexCoord0;
// write to the first color target:
layout(location = 0) out vec4 fragColor;

// one sampler—let Qt bind your Image here:
layout(binding = 0) uniform sampler2D source;

void main() {
    vec4 c = texture(source, qt_TexCoord0);
    // hard-coded fade: at uv.y=0.5→1.0 go from 1.0→0.4
    float f = smoothstep(0.3, 1.0, qt_TexCoord0.y);
    float d = mix(1.0, 0.2, f);
    fragColor = vec4(c.rgb * d, c.a);
}
