#version 450 core

in vec4 fragColor;      // Interpolated color from vertex shader
in vec2 fragTexCoord;   // Interpolated texture coordinates
in vec4 fragExtra;
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

    //
    float alpha = texture(uTexture,fragTexCoord).a;

    float col =0;

    if(alpha>0.1){
        col = 0;
    }else{
        col = 1;

    }
     

    outColor = vec4(col,col,col,1); // Combine texture, vertex color, and uniform color
}
