Texture2D objTex : register(t0);
SamplerState objSamp : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

float4 main (VS_OUT input) : SV_TARGET
{
	float4 col = objTex.Sample (objSamp, input.tex);
	return col;
}