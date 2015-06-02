cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
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
	float2 tex : TEXCOORD;
};

VS_OUT main (VS_IN input)
{
	VS_OUT output;

	output.pos = mul (float4(input.pos, 1.0f), WVP);
	output.tex = input.tex;

	return output;
}