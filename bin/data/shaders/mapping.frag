#version 150
 
in vec4 v_color;
in vec4 v_position;
in vec2 v_texCoord0;

uniform sampler2DRect u_sampler2d;

out vec4 outputColor;

vec3 rgb2hsv(vec3 c);
vec3 hsv2rgb(vec3 c);

uniform float u_maskFactor;
uniform int u_maskUsePow; // 8.0
uniform int u_enableFilter;
vec3 getMask();
 
void main()
{
    /*
    vec4 color;
    color = texture(u_sampler2d, v_texCoord0);
    if (length(color.rgb) > 0.6)
        outputColor = vec4(color.rgb, 0.1);
    else
        outputColor = vec4(color.rgb, 1.0);
        */

    if (1 == u_enableFilter) {
        outputColor = vec4(getMask(), 1.0);
    } else {
        outputColor = texture(u_sampler2d, v_texCoord0);
    }
}

vec3 getMask() // returns color: whites are objects, blacks are background
{
    vec4 imageColor = texture(u_sampler2d, v_texCoord0);
    vec3 imageHSVColor = rgb2hsv(imageColor.rgb);
    vec3 pastureHSVColor = rgb2hsv(vec3(0, 1.0, 0));
    vec3 diffHSVColor = vec3(
        abs(imageHSVColor.x - pastureHSVColor.x),
        abs(imageHSVColor.y - pastureHSVColor.y),
        abs(imageHSVColor.z - pastureHSVColor.z)
    );
    vec4 maskColor = vec4(hsv2rgb(diffHSVColor) * u_maskFactor, 1.0);
    return vec3(
        pow(maskColor.r, u_maskUsePow),
        pow(maskColor.g, u_maskUsePow),
        pow(maskColor.b, u_maskUsePow)
    );
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}


vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
