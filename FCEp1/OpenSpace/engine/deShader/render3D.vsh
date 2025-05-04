cbuffer Constants
{
    float4x4 g_World;
    float4x4 g_View;
    float4x4 g_Proj;
    float4 lightPos;
    float4 lightCol;
    float4 lightInfo;
    float4   g_CameraPosition;
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
    float4 bone_ids : ATTRIB6;
    float4 bone_weights : ATTRIB7;
};

struct PSInput 
{
    float4 Pos       : SV_POSITION;
    float3 WorldPos  : TEXCOORD0;
    float3 Normal    : TEXCOORD1;
    float3 Tangent   : TEXCOORD2;
    float3 BiNormal  : TEXCOORD3;
    float3 UV        : TEXCOORD4;
    float3 lightPos : TEXCOORD5;
    float3 lightDiff : TEXCOORD6;
    float4 lightInfo : TEXCOORD7;
    float4 Color : TEXCOORD8;
    float3 camPos : TEXCOORD9;
};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in  VSInput VSIn,
          out PSInput PSIn)
{


      //PSInput output;

    // Transform position to world space and clip space
    float4 worldPos4 = mul(float4(VSIn.Pos, 1.0f), g_World);
    PSIn.Pos = mul(mul(mul(float4(VSIn.Pos, 1.0), g_World), g_View), g_Proj);
    PSIn.WorldPos = worldPos4.xyz;
    // Transform and normalize tangent-space basis to world space
    float3x3 worldMat3 = (float3x3)g_World; // assume no non-uniform scale
    PSIn.Normal   = normalize(mul(VSIn.Normal, worldMat3));
    PSIn.Tangent  = normalize(mul(VSIn.Tangent, worldMat3));
    PSIn.BiNormal = normalize(mul(VSIn.BiNormal, worldMat3));
    PSIn.UV       = VSIn.UV;
    PSIn.Color = VSIn.Color;
    PSIn.camPos = g_CameraPosition;
    PSIn.lightPos = lightPos;
    PSIn.lightDiff = lightCol.xyz;
    PSIn.lightInfo = lightInfo;

    ///--------
    /*
    PSIn.Color = VSIn.Color;
    PSIn.UV  = VSIn.UV;
    PSIn.Normal = VSIn.Normal;
    PSIn.Tangent = VSIn.Tangent;
    PSIn.BiNormal = VSIn.BiNormal;
    PSIn.lightDiff = float3(lightCol.x, lightCol.y, lightCol.z);
    PSIn.lightPos = float3(lightPos.x, lightPos.y, lightPos.z);
    PSIn.lightInfo = lightInfo;
    */

}