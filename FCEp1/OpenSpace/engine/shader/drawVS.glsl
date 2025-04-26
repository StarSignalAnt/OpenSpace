#version 450 core

layout (location = 0) in vec3 inPosition;   // Vertex position (x, y, z)
layout (location = 1) in vec2 inTexCoord;      // Vertex color (r, g, b, a)
layout (location = 2) in vec4 inColor;   // Texture coordinates (u, v)
layout (location = 3) in vec3 inRealPos;
layout (location = 4) in vec4 inExtra;
layout (location = 5) in vec4 inView;

out vec4 fragColor;      // Pass color to fragment shader
out vec2 fragTexCoord;   // Pass texture coordinates to fragment shader
out vec4 fragExtra;
out vec4 fragView;
uniform mat4 uProjection; // Orthogonal projection matrix

void main()
{

    

    gl_Position = uProjection * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
      fragExtra = inExtra;
    fragView = inView;
}