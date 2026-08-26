#include "Samplers.hlsli"
#include "Textures.hlsli"

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

float4 main(VSOutput input) : SV_Target
{
    float4 albedo_color = albedo.Sample(anisotrophicSampler, input.uv);
    
    float3 normal_map = normal.Sample(anisotrophicSampler, input.uv).xyz * 2.0f - 1.0f;
    
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = cross(N, T);
    
    float3x3 TBN = float3x3(T, B, N);
    
    float3 finalNormal = normalize(mul(normal_map, TBN));
    
    // 가상의 태양빛 방향
    float3 lightDir = normalize(float3(1.0f, -1.0f, 1.0f));
    
    // 빛의 세기(Diffuse) 계산
    // 빛의 방향(lightDir)과 노멀이 정반대로 마주쳐야 가장 밝으므로 -lightDir과 내적
    float diffuseLight = max(0.0f, dot(finalNormal, -lightDir));
    
    // 주변광(Ambient Light) 추가 (빛을 직접 받지 않는 그림자 진 곳도 완전히 까매지지 않도록 기본 밝기 보장)
    float ambientLight = 0.1f;
    
    // 최종 빛의 밝기 = 직사광선 + 주변광
    float finalLightIntensity = diffuseLight + ambientLight;
    
    // 알베도(색상)에 빛의 밝기를 곱해서 최종 픽셀 색상 결정
    float4 finalColor = albedo_color * finalLightIntensity;
    
    // 알파(투명도)는 원래 알베도의 알파값을 유지
    finalColor.a = albedo_color.a;
    return finalColor;
   // return finalColor;
}