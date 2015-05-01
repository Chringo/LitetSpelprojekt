cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
	float4x4 World;
};

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
};

VS_OUT main( VS_IN input )
{
	VS_OUT output;

	output.pos = mul(float4(input.pos, 1.0f), WVP);
	output.normal = mul(input.normal, World);
	output.color = float3(1.0f, 1.0f, 1.0f);

	return output;
}