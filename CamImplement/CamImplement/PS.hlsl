Texture2D objTex : register(t0);
SamplerState objSamp : register(s0);

#define NUMBER_OF_LIGHTS 1

struct DirectionalLight
{
	float3 dir;
	float pad;
	float4 ambient;
	float4 diffuse;
};

struct PointLight
{
	float3 pos;
	float range;
	float3 att;
	float pad;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame : register(b0)
{
	DirectionalLight dirLight;
	PointLight light;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 hue : TEXCOORD1;
};

float4 main(VS_OUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuse = objTex.Sample(objSamp, input.tex);
	diffuse *= input.hue;

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	float3 pointColor = float3(0.0f, 0.0f, 0.0f);
	float3 ambient = float3(0.0f, 0.0f, 0.0f);

	float3 dirColor = saturate(dot(dirLight.dir, input.normal) * dirLight.diffuse);

	float3 pLightVec = light.pos - input.worldPos;
	float d = length(pLightVec);

	if (d <= light.range)
	{
		ambient += light.ambient;
		pLightVec /= d;
		float lightRatio = dot(input.normal, pLightVec);

		if (lightRatio > 0.0f)
		{
			pointColor = lightRatio * diffuse * light.diffuse;
			pointColor /= light.att[0] + (light.att[1] * d) + (light.att[2] * (d*d));
			finalColor += pointColor;
		}
	}

	float3 finalAmbient = saturate(ambient + dirLight.ambient) * diffuse;

	finalColor = saturate((finalColor + dirColor) * diffuse + finalAmbient);

	return float4(finalColor, diffuse.a);
}