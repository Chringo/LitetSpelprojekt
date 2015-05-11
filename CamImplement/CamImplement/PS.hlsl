struct DirectionalLight
{
	float3 dir;
	float pad;
	float4 ambient;
	float4 diffuse;
	float4 color;
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

	float3 finalColor;

	finalColor = dirLight.ambient;
	finalColor += saturate(dot(dirLight.dir, input.normal) * dirLight.diffuse);

	return float4(input.tex, 1.0f, 1.0f);
}