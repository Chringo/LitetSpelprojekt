cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
	float4x4 World;
};

struct VS_IN
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

VS_OUT main( VS_IN input )
{
	VS_OUT output;

	output.pos = mul(float4(input.pos, 1.0f), WVP);
	output.worldPos = mul(float4(input.pos, 1.0f), World);
	output.normal = mul(input.normal, World);
	output.tex = input.tex;

	return output;
}