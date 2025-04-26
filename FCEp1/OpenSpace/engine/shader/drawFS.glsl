#version 450 core

in vec4 fragColor;      // Interpolated color from vertex shader
in vec2 fragTexCoord;   // Interpolated texture coordinates
in vec4 fragView;

out vec4 outColor;      // Final fragment color

uniform sampler2D uTexture; // Texture sampler
uniform float TopY;

void main()
{

    float pixX = gl_FragCoord.x;
    float pixY = TopY-gl_FragCoord.y;
        

    if(pixX<fragView.x || pixX>fragView.x+fragView.z || pixY<fragView.y || pixY>fragView.y+fragView.w)
    {
        discard;
    }

    vec4 texColor = texture(uTexture, fragTexCoord); // Sample texture
    if(texColor.a<0.1)
    {
        discard;
        return;
    }
    outColor = fragColor*texColor; // Combine texture, vertex color, and uniform color
}