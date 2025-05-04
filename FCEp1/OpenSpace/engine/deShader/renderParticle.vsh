cbuffer Constants
{
    float4x4 g_ViewProj;

};

// Vertex shader takes two inputs: vertex position and color.
// By convention, Diligent Engine expects vertex shader inputs to be 
// labeled 'ATTRIBn', where n is the attribute number.
struct VSInput
{
    float3 Pos   : ATTRIB0;
    float3 UV  : ATTRIB1;
    float4 Color : ATTRIB2;
   // float3 Normal  : ATTRIB3;    // World position or some custom 3D value
   // float3 Tangent    : ATTRIB4;    // Custom data (e.g. lighting, effects, etc.)
   // float3 BiNormal    : ATTRIB5; 
    float4 MtrxRow0 : ATTRIB3;
    float4 MtrxRow1 : ATTRIB4;
    float4 MtrxRow2 : ATTRIB5;
    float4 MtrxRow3 : ATTRIB6;
};

struct PSInput 
{
    float4 Pos       : SV_POSITION;
    float3 UV        : TEXCOORD0;
  float4 Color : TEXCOORD1;

};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in  VSInput VSIn,
          out PSInput PSIn)
{

  float4x4 InstanceMatr = MatrixFromRows(VSIn.MtrxRow0, VSIn.MtrxRow1, VSIn.MtrxRow2, VSIn.MtrxRow3);
      //PSInput output;

    // Transform position to world space and clip space

     float4 tp = mul(float4(VSIn.Pos,1.0), InstanceMatr);
   
   PSIn.Pos = tp;


    PSIn.UV       = VSIn.UV;
    PSIn.Color = VSIn.Color;


 
}