#version 450 core

in vec4 fragColor;
in vec2 fragTexCoord;
in vec2 fragRealPos;
in vec4 fragExtra;
in vec4 fragView;
out vec4 outColor;

uniform sampler2D uTexture;
uniform sampler2D feNormalMap;
uniform sampler2D feShadowMap;

uniform vec2 feScreenSize;
uniform vec3 feLightPos;
uniform vec2 feLightActual;
uniform float feLightRange;
uniform float feCamRot;
uniform float feCamZoom;
uniform vec3 feLightDiffuse;
uniform vec2 feMapSize;
uniform float TopY;
const vec2 offsets[9] = vec2[](
    vec2(-12, -12), vec2(  0, -12), vec2( 12, -12),
    vec2(-12,   0), vec2(  0,   0), vec2( 12,   0),
    vec2(-12,  12), vec2(  0,  12), vec2( 12,  12)
);
const float sampleCount = 9.0;

vec2 rotate2D(vec2 point, float angleDegrees) {
    float angle = radians(angleDegrees);
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c) * point;
}

float getShadow(vec2 start, vec2 end) {
   vec2 dir = end - start;
    float dist = length(dir);
    
    // Early return if distance is negligible
    if (dist < 0.001) return 1.0;
    
    // Normalize direction and calculate step count
    vec2 stepDir = dir / dist;
    float stepSize = 12.0;
    int steps = int(dist / stepSize);
    
    // Use normalized coordinates directly
    vec2 uvScale = vec2(1.0 / feMapSize.x, -1.0 / feMapSize.y);
    vec2 uvOffset = vec2(0.0, 1.0);
    
    // Skip first position (start)
    vec2 pos = start + stepDir * stepSize;
    
    // Unroll small loops if your hardware supports it
    for (int i = 0; i < steps; ++i) {
        vec2 sUV = pos * uvScale + uvOffset;
        if (texture(feShadowMap, sUV).r < 0.1)
            return 0.0;
            
        pos += stepDir * stepSize;
    }
    
    return 1.0;
}

void main() {


    float pixX = gl_FragCoord.x;
    float pixY = TopY-gl_FragCoord.y;
        

    if(pixX<fragView.x || pixX>fragView.x+fragView.z || pixY<fragView.y || pixY>fragView.y+fragView.w)
    {
        discard;
    }

    vec2 fragPos = gl_FragCoord.xy;
    fragPos.y = feScreenSize.y - fragPos.y;

    vec2 delta = feLightPos.xy - fragPos;
    //float dis = dot(delta, delta); // Faster than sqrt()
    
    //if (dis > feLightRange * feLightRange) {
     //   discard;
       // return;
    //}

    float dis = sqrt(delta.x*delta.x+delta.y*delta.y);

    float light = dis / feLightRange;
  
    if(light>1.0){
        light = 1.0;
    }


    light = 1 - light;


    vec3 norm = texture(feNormalMap, fragTexCoord).rgb * 2.0 - 1.0;
    norm.y = -norm.y; // Flip Y-axis
    
    vec3 lp = feLightPos;
    lp.z *= feCamZoom;

    vec3 light_Dir = normalize(lp - vec3(fragPos, 0));
    norm.xy = rotate2D(norm.xy, feCamRot);

    float diffuse = max(dot(norm, light_Dir), 0.0);


   float shadow = 0.0;
    for (int i = 0; i < 9; i++)
        shadow += getShadow(fragRealPos+offsets[i], feLightActual+offsets[i]);

    shadow /= sampleCount;



    if (fragExtra.y < 0.1) shadow = 1.0;
    if (fragExtra.z <= 0.1) {
        light = 1.0;
        diffuse = 1.0;
    }

    vec4 texColor = texture(uTexture, fragTexCoord);
    vec4 finalColor = fragColor * texColor * light * diffuse * shadow;
    finalColor.a = texColor.a;

    finalColor.rgb *= feLightDiffuse;

    outColor = finalColor;
}