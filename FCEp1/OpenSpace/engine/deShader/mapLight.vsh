cbuffer Constants
{
    float4x4 g_WorldViewProj;
    float4 screenSize;
    float4 lightPos;
    float4 lightActual;
    float4 lightRange;
    float4 camRot;
    float4 camZoom;
    float4 lightDiffuse;
    float4 mapSize;
    float4 topY;
};

// Vertex shader takes two inputs: vertex position and color.
// By convention, Diligent Engine expects vertex shader inputs to be 
// labeled 'ATTRIBn', where n is the attribute number.
struct VSInput
{
    float3 Pos   : ATTRIB0;
    float2 UV  : ATTRIB1;
    float4 Color : ATTRIB2;
    float3 RealPos  : ATTRIB3;    // World position or some custom 3D value
    float4 Extra    : ATTRIB4;    // Custom data (e.g. lighting, effects, etc.)
    float4 View     : ATTRIB5; 
};

struct PSInput 
{
    float4 Pos   : SV_POSITION;
    float2 UV  : TEXCOORD0;
    float4 Color : COLOR0; 
    float3 RealPos  : TEXCOORD1;    // World position or some custom 3D value
    float4 Extra    : TEXCOORD2;    // Custom data (e.g. lighting, effects, etc.)
    float4 View     : TEXCOORD3;  
    float2 ScreenSize : TEXCOORD4;
    float3 LightPos : TEXCOORD5;
    float3 LightActual : TEXCOORD6;
    float LightRange : TEXCOORD7;
    float CamRot : TEXCOORD8;
    float CamZoom : TEXCOORD9;
    float3 LightDiffuse : TEXCOORD10;
    float2 MapSize : TEXCOORD11;
    float TopY : TEXCOORD12;
};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in  VSInput VSIn,
          out PSInput PSIn)
{
    PSIn.Pos   = mul(float4(VSIn.Pos, 1.0), g_WorldViewProj);
    PSIn.Color = VSIn.Color;
    PSIn.UV  = VSIn.UV;
    PSIn.View = VSIn.View;
    PSIn.Extra = VSIn.Extra;
    PSIn.RealPos = VSIn.RealPos;
    PSIn.ScreenSize = float2(screenSize.x,screenSize.y);
    PSIn.LightPos = float3(lightPos.x,lightPos.y,lightPos.z);
    PSIn.LightActual = float3(lightActual.x,lightActual.y,lightActual.z);
    PSIn.LightRange = lightRange.x;
    PSIn.CamRot = camRot.x;
    PSIn.CamZoom = camZoom.x;
    PSIn.LightDiffuse = float3(lightDiffuse.x,lightDiffuse.y,lightDiffuse.z);
    PSIn.MapSize = float2(mapSize.x,mapSize.y);
    PSIn.TopY = topY.x;

}