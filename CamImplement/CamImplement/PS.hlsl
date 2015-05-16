Texture2D objTex : register(t0);
SamplerState objSamp : register(s0);

struct DirectionalLight
{
	float3 dir;
	float pad;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame : register(b0)
{
	DirectionalLight dirLight;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

float4 main(VS_OUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuse = objTex.Sample(objSamp, input.tex);

	float3 finalColor;

	finalColor = diffuse * dirLight.ambient;
	finalColor += saturate(dot(dirLight.dir, input.normal) * dirLight.diffuse);

	return float4(finalColor, diffuse.a);
}