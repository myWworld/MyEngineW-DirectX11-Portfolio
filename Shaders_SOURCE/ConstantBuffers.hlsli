cbuffer Transform : register(b0)
{
    row_major matrix WorldMatrix;
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
}

cbuffer Animation : register(b1)
{
    row_major matrix BoneMatrices[256];
}

cbuffer UI : register(b2)
{
    float hpRatio;
    float3 padding;
}