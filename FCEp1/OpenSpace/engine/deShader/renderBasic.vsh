cbuffer Constants
{
    float4x4 g_World;
    float4x4 g_View;
    float4x4 g_Proj;
};

// Vertex shader takes two inputs: vertex position and color.
// By convention, Diligent Engine expects vertex shader inputs to be 
// labeled 'ATTRIBn', where n is the attribute number.
struct VSInput
{
    float3 Pos   : ATTRIB0;
    float3 UV  : ATTRIB1;
    float4 Color : ATTRIB2;
    float3 Normal  : ATTRIB3;    // World position or some custom 3D value
    float3 Tangent    : ATTRIB4;    // Custom data (e.g. lighting, effects, etc.)
    float3 BiNormal    : ATTRIB5; 
};

struct PSInput 
{
    float4 Pos   : SV_POSITION;
    float3 UV  : TEXCOORD0;
    float4 Color : COLOR0; 
    float3 Normal  : TEXCOORD1;    // World position or some custom 3D value
    float3 Tangent    : TEXCOORD2;    // Custom data (e.g. lighting, effects, etc.)
    float3 BiNormal     : TEXCOORD3;  
};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in  VSInput VSIn,
          out PSInput PSIn)
{
    PSIn.Pos = mul(mul(mul(float4(VSIn.Pos, 1.0), g_World), g_View), g_Proj);
    PSIn.Color = VSIn.Color;
    PSIn.UV  = VSIn.UV;
    PSIn.Normal = VSIn.Normal;
    PSIn.Tangent = VSIn.Tangent;
    PSIn.BiNormal = VSIn.BiNormal;

}