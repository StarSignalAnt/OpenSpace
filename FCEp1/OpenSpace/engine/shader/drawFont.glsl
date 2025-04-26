#version 450 core

in vec4 fragColor;      // Interpolated color from vertex shader
in vec2 fragTexCoord;   // Interpolated texture coordinates

out vec4 outColor;      // Final fragment color

uniform sampler2D uTexture; // Texture sampler

void main()
{
    vec4 texColor = texture(uTexture, fragTexCoord); // Sample texture
    texColor.g = texColor.r;
    texColor.b = texColor.r;
    
    if(texColor.r<0.1)
    {
        texColor.a=0.0;
    }else{
        texColor.a=1.0;
        texColor.a = texColor.r;
    }
//    texColor.rgb *= fragColor.rgb;
    texColor.rgb = fragColor.rgb;


    outColor = texColor; // Combine texture, vertex color, and uniform color
}
