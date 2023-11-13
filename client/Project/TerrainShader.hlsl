#include "Light.hlsl"

struct VS_Terrain_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct VS_Terrain_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float4 colorw : COLOR;
};

VS_Terrain_OUTPUT VSTerrainLighting(VS_Terrain_INPUT input)
{
    VS_Terrain_OUTPUT output;
    //output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    //output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    //output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.colorw = input.color;
    return (output);
}

float4 PSTerrainLighting(VS_Terrain_OUTPUT output) : SV_TARGET
{
    //output.normalW = normalize(output.normalW);
    //float4 color = ColorLighting(output.positionW, output.normalW, output.color);
    float4 color = output.colorw;
    return (color);
}