#include "ConstantBuffers.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
    float2 uv : TEXCOORD;
};

struct VSOutput
{

    float4 pos : SV_Position;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0.0f;
    
 
    float4 pos = mul(float4(input.pos, 1.0f), WorldMatrix);
    float4 viewPos = mul(pos, ViewMatrix);
    float4 projPos = mul(viewPos, ProjectionMatrix);
    
    output.pos = projPos;
    output.color = input.color;
    output.normal = normalize(mul(input.normal, (float3x3) WorldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3) WorldMatrix));
    output.uv = input.uv;
    return output;
}