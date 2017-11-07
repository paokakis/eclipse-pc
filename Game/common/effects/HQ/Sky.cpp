#include "Sky.h"
#include "../../../Engine/Tools/Log.hpp"
#include "../../../Engine/Tools/Helper.h"

static const char* VERTEX_SHADER =
"#version 150 core\n"
"precision highp float;\n"
"attribute vec4 aPosition;\n"
"uniform mat4 uProjection;\n"
"void main()\n"
"{\n"
" gl_Position = uProjection * aPosition;\n"
"}";

//Sky
static const char* FRAGMENT_SHADER =
"precision highp float;\n"
"uniform vec2 u_resolution;\n"
"uniform float u_time;\n"
"const float cloudscale = 1.1;\n"
"const float speed = 0.03;\n"
"const float clouddark = 0.5;\n"
"const float cloudlight = 0.3;\n"
"const float cloudcover = 0.2;\n"
"const float cloudalpha = 8.0;\n"
"const float skytint = 0.5;\n"
"const vec3 skycolour1 = vec3(0.2, 0.4, 0.6);\n"
"const vec3 skycolour2 = vec3(0.4, 0.7, 1.0);\n"
"const mat2 m = mat2(1.6, 1.2, -1.2, 1.6);\n"
"vec2 hash(vec2 p) {\n"
" p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));\n"
" return -1.0 + 2.0*fract(sin(p)*43758.5453123);\n"
"}\n"
"float noise(in vec2 p) {\n"
" const float K1 = 0.366025404;\n"
" const float K2 = 0.211324865;\n"
" vec2 i = floor(p + (p.x + p.y)*K1);\n"
" vec2 a = p - i + (i.x + i.y)*K2;\n"
" vec2 o = (a.x>a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);\n"
" vec2 b = a - o + K2;\n"
" vec2 c = a - 1.0 + 2.0*K2;\n"
" vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);\n"
" vec3 n = h*h*h*h*vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));\n"
" return dot(n, vec3(70.0));\n"
"}\n"
"float fbm(vec2 n) {\n"
" float total = 0.0, amplitude = 0.1;\n"
" for (int i = 0; i < 7; i++) {\n"
"  total += noise(n) * amplitude;\n"
"  n = m * n;\n"
"  amplitude *= 0.4;\n"
" }\n"
" return total;\n"
"}\n"
"void mainImage(out vec4 fragColor, in vec2 fragCoord) {\n"
" vec2 iResolution = u_resolution;\n"
" float iGlobalTime = u_time;\n"
" vec2 p = fragCoord.xy / iResolution.xy;\n"
" vec2 uv = p*vec2(iResolution.x / iResolution.y, 1.0);\n"
" float time = iGlobalTime * speed;\n"
" float q = fbm(uv * cloudscale * 0.5);\n"
" float r = 0.0;\n"
" uv *= cloudscale;\n"
" uv -= q - time;\n"
" float weight = 0.8;\n"
" for (int i = 0; i<8; i++) {\n"
"  r += abs(weight*noise(uv));\n"
"  uv = m*uv + time;\n"
"  weight *= 0.7;\n"
" }\n"
" float f = 0.0;\n"
" uv = p*vec2(iResolution.x / iResolution.y, 1.0);\n"
" uv *= cloudscale;\n"
" uv -= q - time;\n"
" weight = 0.7;\n"
" for (int i = 0; i<8; i++) {\n"
"  f += weight*noise(uv);\n"
"  uv = m*uv + time;\n"
"  weight *= 0.6;\n"
" }\n"
" f *= r + f;\n"
" float c = 0.0;\n"
" time = iGlobalTime * speed * 2.0;\n"
" uv = p*vec2(iResolution.x / iResolution.y, 1.0);\n"
" uv *= cloudscale*2.0;\n"
" uv -= q - time;\n"
" weight = 0.4;\n"
" for (int i = 0; i<7; i++) {\n"
"  c += weight*noise(uv);\n"
"  uv = m*uv + time;\n"
"  weight *= 0.6;\n"
" }\n"
" float c1 = 0.0;\n"
" time = iGlobalTime * speed * 3.0;\n"
" uv = p*vec2(iResolution.x / iResolution.y, 1.0);\n"
" uv *= cloudscale*3.0;\n"
" uv -= q - time;\n"
" weight = 0.4;\n"
" for (int i = 0; i<7; i++) {\n"
"  c1 += abs(weight*noise(uv));\n"
"  uv = m*uv + time;\n"
"  weight *= 0.6;\n"
" }\n"
" c += c1;\n"
" vec3 skycolour = mix(skycolour2, skycolour1, p.y);\n"
" vec3 cloudcolour = vec3(1.1, 1.1, 0.9) * clamp((clouddark + cloudlight*c), 0.0, 1.0);\n"
" f = cloudcover + cloudalpha*f*r;\n"
" vec3 result = mix(skycolour, clamp(skytint * skycolour + cloudcolour, 0.0, 1.0), clamp(f + c, 0.0, 1.0));\n"
" fragColor = vec4(result, 1.0);\n"
"}\n"
"void main() {\n"
" vec2 inFragCoord;\n"
" vec4 outFragColor;\n"
" inFragCoord.x = gl_FragCoord.x;\n"
" inFragCoord.y = gl_FragCoord.y;\n"
" mainImage(outFragColor, inFragCoord);\n"
" gl_FragColor = outFragColor;\n"
"}";

// Stars!!
static const char* FRAGMENT_SHADER1 =
"precision highp float;\n"
"#define iterations 17\n"
"#define formuparam 0.53\n"
"#define volsteps 20\n"
"#define stepsize 0.1\n"
"#define zoom   3.000\n"
"#define tile   2.000\n" //1.000
"#define speed  0.0001\n" //0.015
"#define brightness 0.0003\n" // 0.0015
"#define darkmatter 0.300\n"
"#define distfading 0.730\n"
"#define saturation 0.850\n"
"uniform vec2 u_resolution;\n"
"uniform float u_time;\n"
"uniform vec3 u_pos;\n" // added by me
"void mainImage(out vec4 fragColor, in vec2 fragCoord) {\n"
" vec2 iResolution = u_resolution;\n"
" float iGlobalTime = u_time;\n"
" vec2 uv = fragCoord.xy / iResolution.xy - 0.5;\n"
" uv.y *= iResolution.y / iResolution.x;\n"
" vec3 dir = vec3(uv*zoom, 1.0);\n"
" float time = iGlobalTime*speed + 0.25;\n"
//mouse rotation
" float a1 = 0.5 - (u_pos.x / iResolution.x*2.0) / 8000.0;\n" //0.5 + iMouse.x / ...
" float a2 = 0.8 - (u_pos.z / iResolution.y*2.0) / 8000.0;\n" //0.5 + iMouse.y / ...
" mat2 rot1 = mat2(cos(a1), sin(a1), -sin(a1), cos(a1));\n"
" mat2 rot2 = mat2(cos(a2), sin(a2), -sin(a2), cos(a2));\n"
" dir.xz *= rot1;\n"
" dir.xy *= rot2;\n"
" vec3 from = vec3(0.0, 0.5, 0.5);\n"
" from += vec3(0.0, time, -2.0);\n"			// direction & speed
" from.xz *= rot1;\n"
" from.xy *= rot2;\n"
	//volumetric rendering
" float s = 0.1, fade = 1.0;\n"
" vec3 v = vec3(0.0);\n"
" for (int r = 0; r<volsteps; r++) {\n"
"  vec3 p = from + s * dir * 0.5;\n"
"  p = abs(vec3(tile) - mod(p, vec3(tile * 2.0)));\n" // tiling fold
"  float pa, a = pa = 0.0;\n"
"  for (int i = 0; i<iterations; i++) {\n"
"   p = abs(p) / dot(p, p) - formuparam;\n" // the magic formula
"   a += abs(length(p) - pa);\n" // absolute sum of average change
"   pa = length(p);\n"
"  }\n"
"  float dm = max(0.0, darkmatter - a * a * 0.001);\n" //dark matter
"  a *= a*a;\n" // add contrast
"  if (r>6) fade *= 1.0 - dm;\n" // dark matter, don't render near
"  v+=vec3(dm,dm*.5,0.);\n" // <---- I can comment this
"  v += fade;\n"
"  v += vec3(s, s*s, s*s*s*s)*a*brightness*fade;\n" // coloring based on distance
"  fade *= distfading;\n" // distance fading
"  s += stepsize;\n"
" }\n"
" v = mix(vec3(length(v)), v, saturation);\n" //color adjust
" fragColor = vec4(v * 0.01, 1.0);\n"
"}\n"
"void main() {\n"
" vec2 inFragCoord;\n"
" vec4 outFragColor;\n"
" inFragCoord.x = gl_FragCoord.x;\n"
" inFragCoord.y = gl_FragCoord.y;\n"
" mainImage(outFragColor, inFragCoord);\n"
" gl_FragColor = outFragColor;\n"
"}";

// fire
static const char* FRAGMENT_SHADER2 =
"precision highp float;\n"
"uniform vec2 u_resolution;\n"
"uniform float u_time;\n"
"vec2 iResolution = u_resolution;\n"
"float iGlobalTime = u_time;\n"
"vec2 hash(vec2 p) {\n"
" p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));\n"
" return -1.0 + 2.0*fract(sin(p)*43758.5453123); }\n"
"float noise(in vec2 p) {\n"
" const float K1 = 0.366025404;\n" // (sqrt(3)-1)/2;
" const float K2 = 0.211324865;\n" // (3-sqrt(3))/6;
" vec2 i = floor(p + (p.x + p.y)*K1);\n"
" vec2 a = p - i + (i.x + i.y)*K2;\n"
" vec2 o = (a.x>a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);\n"
" vec2 b = a - o + K2;\n"
" vec2 c = a - 1.0 + 2.0*K2;\n"
" vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);\n"
" vec3 n = h*h*h*h*vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));\n"
" return dot(n, vec3(70.0)); }\n"
"float fbm(vec2 uv) {\n"
" float f;\n"
" mat2 m = mat2(1.6, 1.2, -1.2, 1.6);\n"
" f = 0.5000*noise(uv); uv = m*uv;\n"
" f += 0.2500*noise(uv); uv = m*uv;\n"
" f += 0.1250*noise(uv); uv = m*uv;\n"
" f += 0.0625*noise(uv); uv = m*uv;\n"
" f = 0.5 + 0.5*f;\n"
" return f;\n"
"}\n"
"void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
"{\n"
" vec2 uv = fragCoord.xy / iResolution.xy;\n"
" vec2 q = uv;\n"
" q.x *= 5.;\n"
" q.y *= 2.;\n"
" float strength = floor(q.x + 1.);\n"
" float T3 = max(3., 1.25*strength)*iGlobalTime;\n"
" q.x = mod(q.x, 1.) - 0.5;\n"
" q.y -= 0.25;\n"
" float n = fbm(strength*q - vec2(0, T3));\n"
" float c = 1. - 16. * pow(max(0., length(q*vec2(1.8 + q.y*1.5, .75)) - n * max(0., q.y + .25)), 1.2);\n"
//	float c1 = n * c * (1.5-pow(1.25*uv.y,4.));
" float c1 = n * c * (1.5 - pow(2.50*uv.y, 4.));\n"
" c1 = clamp(c1, 0., 1.);\n"
" vec3 col = vec3(1.5*c1, 1.5*c1*c1*c1, c1*c1*c1*c1*c1*c1);\n"
"#ifdef BLUE_FLAME\n"
	"col = col.zyx;\n"
"#endif\n"
"#ifdef GREEN_FLAME\n"
	"col = 0.85*col.yxz;\n"
"#endif\n"
" float a = c * (1. - pow(uv.y, 3.));\n"
" fragColor = vec4(mix(vec3(0.), col, a), 1.0);\n"
"}\n"
"void main() {\n"
" vec2 inFragCoord;\n"
" vec4 outFragColor;\n"
" inFragCoord.x = gl_FragCoord.x;\n"
" inFragCoord.y = gl_FragCoord.y;\n"
" mainImage(outFragColor, inFragCoord);\n"
" gl_FragColor = outFragColor;\n"
"}";

// PS lines
static const char* FRAGMENT_SHADER3 =
"#version 150 core\n"
"precision highp float;\n"
"uniform vec2 u_resolution;\n"
"uniform float u_time;\n"
"vec2 iResolution = u_resolution;\n"
"float iGlobalTime = u_time;\n"
"float gradient(float p) {\n"
" vec2 pt0 = vec2(0.00, 0.0);\n"
" vec2 pt1 = vec2(0.86, 0.1);\n"
" vec2 pt2 = vec2(0.955, 0.40);\n"
" vec2 pt3 = vec2(0.99, 1.0);\n"
" vec2 pt4 = vec2(1.00, 0.0);\n"
" if (p < pt0.x) return pt0.y;\n"
" if (p < pt1.x) return mix(pt0.y, pt1.y, (p - pt0.x) / (pt1.x - pt0.x));\n"
" if (p < pt2.x) return mix(pt1.y, pt2.y, (p - pt1.x) / (pt2.x - pt1.x));\n"
" if (p < pt3.x) return mix(pt2.y, pt3.y, (p - pt2.x) / (pt3.x - pt2.x));\n"
" if (p < pt4.x) return mix(pt3.y, pt4.y, (p - pt3.x) / (pt4.x - pt3.x));\n"
" return pt4.y;\n" 
"}\n"
"float waveN(vec2 uv, vec2 s12, vec2 t12, vec2 f12, vec2 h12) {\n"
" vec2 x12 = sin((iGlobalTime * s12 + t12 + uv.x) * f12) * h12;\n"
" float g = gradient(uv.y / (0.5 + x12.x + x12.y));\n"
" return g * 0.27;\n"
"}\n"
"float wave1(vec2 uv) {\n"
" return waveN(vec2(uv.x, uv.y - 0.25), vec2(0.03, 0.06), vec2(0.00, 0.02), vec2(8.0, 3.7), vec2(0.06, 0.05));\n"
"}\n"
"float wave2(vec2 uv) {\n"
" return waveN(vec2(uv.x, uv.y - 0.25), vec2(0.04, 0.07), vec2(0.16, -0.37), vec2(6.7, 2.89), vec2(0.06, 0.05));\n"
"}\n"
"float wave3(vec2 uv) {\n"
" return waveN(vec2(uv.x, 0.75 - uv.y), vec2(0.035, 0.055), vec2(-0.09, 0.27), vec2(7.4, 2.51), vec2(0.06, 0.05));\n"
"}\n"
"float wave4(vec2 uv) {\n"
" return waveN(vec2(uv.x, 0.75 - uv.y), vec2(0.032, 0.09), vec2(0.08, -0.22), vec2(6.5, 3.89), vec2(0.06, 0.05));\n"
"}\n"
"void mainImage(out vec4 fragColor, in vec2 fragCoord) {\n"
" vec2 uv = fragCoord.xy / iResolution.xy;\n"
" float waves = wave1(uv) + wave2(uv) + wave3(uv) + wave4(uv);\n"
" float x = uv.x;\n"
" float y = 1.0 - uv.y;\n"
" vec3 bg = mix(vec3(0.05, 0.05, 0.3), vec3(0.1, 0.65, 0.85), (x + y)*0.55);\n"
" vec3 ac = bg + vec3(1.0, 1.0, 1.0) * waves;\n"
" fragColor = vec4(ac, 1.0);\n"
"}\n"
"void main() {\n"
" vec2 inFragCoord;\n"
" vec4 outFragColor;\n"
" inFragCoord.x = gl_FragCoord.x;\n"
" inFragCoord.y = gl_FragCoord.y;\n"
" mainImage(outFragColor, inFragCoord);\n"
" gl_FragColor = outFragColor;\n"
"}";

static const char* FRAGMENT_SHADER4 = R"glsl(
//const float FLIGHT_SPEED = 8.0;

const float DRAW_DISTANCE = 60.0; // Lower this to increase framerate
const float FADEOUT_DISTANCE = 10.0; // must be < DRAW_DISTANCE    
const float FIELD_OF_VIEW = 1.05;   

const float STAR_SIZE = 0.6; // must be > 0 and < 1
const float STAR_CORE_SIZE = 0.14;

const float CLUSTER_SCALE = 0.02;
const float STAR_THRESHOLD = 0.775;

const float BLACK_HOLE_CORE_RADIUS = 0.2;
const float BLACK_HOLE_THRESHOLD = 0.9995;
const float BLACK_HOLE_DISTORTION = 0.03;

uniform vec2 u_resolution;
uniform float u_time;
uniform float FLIGHT_SPEED;
vec2 iResolution = u_resolution;
float iGlobalTime = u_time;

// http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 getRayDirection(vec2 fragCoord, vec3 cameraDirection) {
    vec2 uv = fragCoord.xy / iResolution.xy;
	
    const float screenWidth = 1.0;
    float originToScreen = screenWidth / 2.0 / tan(FIELD_OF_VIEW / 2.0);
    
    vec3 screenCenter = originToScreen * cameraDirection;
    vec3 baseX = normalize(cross(screenCenter, vec3(0, -1.0, 0)));
    vec3 baseY = normalize(cross(screenCenter, baseX));
    
    return normalize(screenCenter + (uv.x - 0.5) * baseX + (uv.y - 0.5) * iResolution.y / iResolution.x * baseY);
}

float getDistance(ivec3 chunkPath, vec3 localStart, vec3 localPosition) {
    return length(vec3(chunkPath) + localPosition - localStart);
}

void move(inout vec3 localPosition, vec3 rayDirection, vec3 directionBound) {
    vec3 directionSign = sign(rayDirection);
	vec3 amountVector = (directionBound - directionSign * localPosition) / abs(rayDirection);
    
    float amount = min(amountVector.x, min(amountVector.y, amountVector.z));
    
    localPosition += amount * rayDirection;
}

// Makes sure that each component of localPosition is >= 0 and <= 1
void moveInsideBox(inout vec3 localPosition, inout ivec3 chunk, vec3 directionSign, vec3 direcctionBound) {
    const float eps = 0.0000001;
    if (localPosition.x * directionSign.x >= direcctionBound.x - eps) {
        localPosition.x -= directionSign.x;
        chunk.x += int(directionSign.x);
    } else if (localPosition.y * directionSign.y >= direcctionBound.y - eps) {
        localPosition.y -= directionSign.y;
        chunk.y += int(directionSign.y);
    } else if (localPosition.z * directionSign.z >= direcctionBound.z - eps) {
        localPosition.z -= directionSign.z;
        chunk.z += int(directionSign.z);
    }
}

bool hasStar(ivec3 chunk) {
    return texture(iChannel0, mod(CLUSTER_SCALE * (vec2(chunk.xy) + vec2(chunk.zx)) + vec2(0.724, 0.111), 1.0)).r > STAR_THRESHOLD
        && texture(iChannel0, mod(CLUSTER_SCALE * (vec2(chunk.xz) + vec2(chunk.zy)) + vec2(0.333, 0.777), 1.0)).r > STAR_THRESHOLD;
}

bool hasBlackHole(ivec3 chunk) {
    return rand(0.0001 * vec2(chunk.xy) + 0.002 * vec2(chunk.yz)) > BLACK_HOLE_THRESHOLD;
}

vec3 getStarToRayVector(vec3 rayBase, vec3 rayDirection, vec3 starPosition) {
	float r = (dot(rayDirection, starPosition) - dot(rayDirection, rayBase)) / dot(rayDirection, rayDirection);
    vec3 pointOnRay = rayBase + r * rayDirection;
    return pointOnRay - starPosition;
}

vec3 getStarPosition(ivec3 chunk, float starSize) {
    vec3 position = abs(vec3(rand(vec2(float(chunk.x) / float(chunk.y) + 0.24, float(chunk.y) / float(chunk.z) + 0.66)),
                             rand(vec2(float(chunk.x) / float(chunk.z) + 0.73, float(chunk.z) / float(chunk.y) + 0.45)),
                             rand(vec2(float(chunk.y) / float(chunk.x) + 0.12, float(chunk.y) / float(chunk.z) + 0.76))));
    
    return starSize * vec3(1.0) + (1.0 - 2.0 * starSize) * position;
}

vec4 getNebulaColor(vec3 globalPosition, vec3 rayDirection) {
    vec3 color = vec3(0.0);
    float spaceLeft = 1.0;
    
    const float layerDistance = 10.0;
    float rayLayerStep = rayDirection.z / layerDistance;
    
    const int steps = 4;
    for (int i = 0; i <= steps; i++) {
    	vec3 noiseeval = globalPosition + rayDirection * ((1.0 - fract(globalPosition.z / layerDistance) + float(i)) * layerDistance / rayDirection.z);
    	noiseeval.xy += noiseeval.z;
        
        
        float value = 0.06 * texture(iChannel0, fract(noiseeval.xy / 60.0)).r;
         
        if (i == 0) {
            value *= 1.0 - fract(globalPosition.z / layerDistance);
        } else if (i == steps) {
            value *= fract(globalPosition.z / layerDistance);
        }
        
        float hue = mod(noiseeval.z / layerDistance / 34.444, 1.0);
        
        color += spaceLeft * hsv2rgb(vec3(hue, 1.0, value));
        spaceLeft = max(0.0, spaceLeft - value * 2.0);
    }
    return vec4(color, 1.0);
}

vec4 getStarGlowColor(float starDistance, float angle, float hue) {
    float progress = 1.0 - starDistance;
    return vec4(hsv2rgb(vec3(hue, 0.3, 1.0)), 0.4 * pow(progress, 2.0) * mix(pow(sin(angle * 2.5), 6.0), 1.0, progress));
}

float atan2(vec2 value) {
    if (value.x > 0.0) {
        return atan(value.y / value.x);
    } else if (value.x == 0.0) {
    	return 3.14592 * 0.5 * sign(value.y);   
    } else if (value.y >= 0.0) {
        return atan(value.y / value.x) + 3.141592;
    } else {
        return atan(value.y / value.x) - 3.141592;
    }
}

vec3 getStarColor(vec3 starSurfaceLocation, float seed, float viewDistance) {
    const float DISTANCE_FAR = 20.0;
    const float DISTANCE_NEAR = 15.0;
    
    if (viewDistance > DISTANCE_FAR) {
    	return vec3(1.0);
    }
    
    float fadeToWhite = max(0.0, (viewDistance - DISTANCE_NEAR) / (DISTANCE_FAR - DISTANCE_NEAR));
    
    vec3 coordinate = vec3(acos(starSurfaceLocation.y), atan2(starSurfaceLocation.xz), seed);
    
    float progress = pow(texture(iChannel0, fract(0.3 * coordinate.xy + seed * vec2(1.1))).r, 4.0);
    
    return mix(mix(vec3(1.0, 0.98, 0.9), vec3(1.0, 0.627, 0.01), progress), vec3(1.0), fadeToWhite);
}

vec4 blendColors(vec4 front, vec4 back) {
  	return vec4(mix(back.rgb, front.rgb, front.a / (front.a + back.a)), front.a + back.a - front.a * back.a);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec3 movementDirection = normalize(vec3(0.01, 0.0, 1.0));
    
    vec3 rayDirection = getRayDirection(fragCoord, movementDirection);
    vec3 directionSign = sign(rayDirection);
    vec3 directionBound = vec3(0.5) + 0.5 * directionSign;
    
    vec3 globalPosition = vec3(3.14159, 3.14159, 0.0) + (iGlobalTime + 1000.0) * FLIGHT_SPEED * movementDirection;
    ivec3 chunk = ivec3(globalPosition);
    vec3 localPosition = mod(globalPosition, 1.0);
    moveInsideBox(localPosition, chunk, directionSign, directionBound);
    
    ivec3 startChunk = chunk;
    vec3 localStart = localPosition;
    
    fragColor = vec4(0.0);
    
    for (int i = 0; i < 200; i++) {
        move(localPosition, rayDirection, directionBound);
        moveInsideBox(localPosition, chunk, directionSign, directionBound);
        
        if (hasStar(chunk)) {
            vec3 starPosition = getStarPosition(chunk, 0.5 * STAR_SIZE);
			float currentDistance = getDistance(chunk - startChunk, localStart, starPosition);
            if (currentDistance > DRAW_DISTANCE && false) {
                break;
            }
            
            // This vector points from the center of the star to the closest point on the ray (orthogonal to the ray)
            vec3 starToRayVector = getStarToRayVector(localPosition, rayDirection, starPosition);
            // Distance between ray and star
            float distanceToStar = length(starToRayVector);
            distanceToStar *= 2.0;
            
            if (distanceToStar < STAR_SIZE) {
                float starMaxBrightness = clamp((DRAW_DISTANCE - currentDistance) / FADEOUT_DISTANCE, 0.001, 1.0);
            	
                float starColorSeed = (float(chunk.x) + 13.0 * float(chunk.y) + 7.0 * float(chunk.z)) * 0.00453;
                if (distanceToStar < STAR_SIZE * STAR_CORE_SIZE) {
                    // This vector points from the center of the star to the point of the star sphere surface that this ray hits
            		vec3 starSurfaceVector = normalize(starToRayVector + rayDirection * sqrt(pow(STAR_CORE_SIZE * STAR_SIZE, 2.0) - pow(distanceToStar, 2.0)));
					
                    fragColor = blendColors(fragColor, vec4(getStarColor(starSurfaceVector, starColorSeed, currentDistance), starMaxBrightness));                    
                    break;
                } else {
                    float localStarDistance = ((distanceToStar / STAR_SIZE) - STAR_CORE_SIZE) / (1.0 - STAR_CORE_SIZE);
                    vec4 glowColor = getStarGlowColor(localStarDistance, acos(starToRayVector.y / length(starToRayVector)), starColorSeed);
                    glowColor.a *= starMaxBrightness;
                	fragColor = blendColors(fragColor, glowColor);
                }
            }
        } else if (hasBlackHole(chunk)) {
            const vec3 blackHolePosition = vec3(0.5);
			float currentDistance = getDistance(chunk - startChunk, localStart, blackHolePosition);
            float fadeout = min(1.0, (DRAW_DISTANCE - currentDistance) / FADEOUT_DISTANCE);
            	
            // This vector points from the center of the black hole to the closest point on the ray (orthogonal to the ray)
            vec3 coreToRayVector = getStarToRayVector(localPosition, rayDirection, blackHolePosition);
            float distanceToCore = length(coreToRayVector);
            if (distanceToCore < BLACK_HOLE_CORE_RADIUS * 0.5) {
                fragColor = blendColors(fragColor, vec4(vec3(0.0), fadeout));
                break;
            } else if (distanceToCore < 0.5) {
            	rayDirection = normalize(rayDirection - fadeout * (BLACK_HOLE_DISTORTION / distanceToCore - BLACK_HOLE_DISTORTION / 0.5) * coreToRayVector / distanceToCore);
            }
        }
        
        if (length(vec3(chunk - startChunk)) > DRAW_DISTANCE) {
            break;
        }
    }
    
    if (fragColor.a < 1.0) {
    	fragColor = blendColors(fragColor, getNebulaColor(globalPosition, rayDirection));
    }
})glsl";

Sky::Sky(GraphicsManager& pGraphMan, TimeManager& pTimeMan, eBGtype_t type) :
	mGraphicsManager(pGraphMan), mTimeManager(pTimeMan), mType(type),
	aPosition(0), u_time(0), uProjection(0), u_resolution(0), mTime(0), mShaderProgram(0)
{
	Log::info("Sky constructor");
}

Sky::~Sky()
{
	Log::info("Sky destructor");
}

status Sky::load()
{
	Log::info("Loading Sky.");

	mResolution.x = mGraphicsManager.getRenderWidth();
	mResolution.y = mGraphicsManager.getRenderHeight();

	mIndexes.push_back(0); mIndexes.push_back(1);
	mIndexes.push_back(2); mIndexes.push_back(2);
	mIndexes.push_back(1); mIndexes.push_back(3);

	for (int j = 0; j < 4; ++j)
	{
		mVertices.push_back(Vertex());
	}
	
	// Creates and retrieves shader attributes and uniforms.
	switch (mType)
	{
	case BACKGROUND_SKY:
		mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
		break;
	case BACKGROUND_STARS:
		mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER1);
		mPos = glm::vec3(500.f);
		u_pos = glGetUniformLocation(mShaderProgram, "u_pos");
		break;
	case BACKGROUND_OBJECT:
		mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER2);
		break;
	case BACKGROUND_LINES:
		mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER3);
		break;
	case BACKGROUND_STARFIELD:
		mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER4);
		u_flightSpeed = glGetUniformLocation(mShaderProgram, "FLIGHT_SPEED");
		break;
	default:
		break;
	}	
	if (mShaderProgram == 0)
		goto ERROR;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	u_time = glGetUniformLocation(mShaderProgram, "u_time");
	u_resolution = glGetUniformLocation(mShaderProgram, "u_resolution");

	mProjection = mGraphicsManager.getOrthoMVP();

	Log::info("Sky loaded successfuly.");

	return STATUS_OK;
ERROR:
	Log::error("Error loading Sky");

	return STATUS_KO;
}

void Sky::update()
{
	mTime += mTimeManager.elapsed();
}

void Sky::draw()
{
	Vertex* vertices = (&mVertices[0]);
	prepVrtx(vertices);

	glEnable(GL_BLEND);

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &mProjection[0][0]);

	// pass xyz vertex data
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, // Attribute Index
		3, // Size in bytes (x y and z)
		GL_FLOAT, // Data type
		GL_FALSE, // Normalized
		sizeof(Vertex),// Stride
		&(vertices[0].x)); // Location

	glUniform2fv(u_resolution, 1, &mResolution.x);
	if (mType == BACKGROUND_STARS) glUniform3fv(u_pos, 1, &mPos.x);
	if (mType == BACKGROUND_STARFIELD) glUniform1f(u_flightSpeed, mFlightSpeed);
	glUniform1fv(u_time, 1, &mTime);

	glDrawElements(GL_TRIANGLES,
		// Number of indexes
		6,
		// Indexes data type
		GL_UNSIGNED_SHORT,
		// First index
		&mIndexes[0]);

	glUseProgram(0);
	glDisable(GL_BLEND);
}

void Sky::prepVrtx(Vertex pVertices[4])
{
	// Draws selected frame.
	GLfloat posX1 = 0.f;
	GLfloat posY1 = 0.f;
	GLfloat posX2 = posX1 + mGraphicsManager.getRenderWidth();
	GLfloat posY2 = posY1 + mGraphicsManager.getRenderHeight();
	GLfloat posZ1 = 0.f;
	GLfloat posZ2 = 0.f;
	pVertices[0].x = posX2; pVertices[0].y = posY2; pVertices[0].z = posZ2;
	pVertices[1].x = posX2; pVertices[1].y = posY1; pVertices[1].z = posZ1;
	pVertices[2].x = posX1; pVertices[2].y = posY2; pVertices[2].z = posZ2;
	pVertices[3].x = posX1; pVertices[3].y = posY1; pVertices[3].z = posZ1;
}