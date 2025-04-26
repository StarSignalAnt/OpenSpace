#version 450 core

in vec4 fragColor;       
in vec2 fragTexCoord;    

out vec4 outColor;       

uniform float blurAmount;
uniform sampler2D uTexture; 

void main() {
    if (blurAmount <= 0.0) {
        //outColor = fragColor * texture(uTexture, fragTexCoord);
        outColor = vec4(1,0,0,1);
        return;
    }

    vec2 texSize = textureSize(uTexture, 0);
    float effectiveBlur = max(0.001, blurAmount * 0.005); 
    int radius = 6;
    
    vec3 col = vec3(0.0);
    float samples = 0.0;
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            vec2 offset = vec2(float(x), float(y)) * (effectiveBlur / texSize);
            vec2 uv = fragTexCoord + offset;
            if(uv.x>=0 && uv.y>=0 && uv.x<=1.0 && uv.y<=1.0)
            {
                col += texture(uTexture, uv).rgb;
                samples += 1.0;
            }
        }
    }
    
    col = col / samples;
    outColor = fragColor * vec4(col, 1.0);
}