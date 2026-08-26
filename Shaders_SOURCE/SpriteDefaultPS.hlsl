#include "Samplers.hlsli"
#include "Textures.hlsli"

cbuffer UIBuffer : register(b2) // C++에서 넘긴 슬롯 번호
{
    float hpRatio;
    float3 padding;
};

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutput
{

    float4 pos : SV_Position;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

float4 main(VSOutput input) : SV_Target
{
    float4 color = sprite.Sample(anisotrophicSampler, input.uv);
    
    if (input.uv.x > hpRatio)
    {
        color.a = 0.0f; 
    }
    
    return color;
}