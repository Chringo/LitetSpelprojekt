Texture2D objTex : register(t0);
SamplerState objSamp : register(s0);

#define NUMBER_OF_LIGHTS 4

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
	PointLight light[NUMBER_OF_LIGHTS];
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

float4 main(VS_OUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuse = objTex.Sample(objSamp, input.tex);

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	float3 pointColor = float3(0.0f, 0.0f, 0.0f);
	float3 ambient = float3(0.0f, 0.0f, 0.0f);

	float3 dirColor = saturate(dot(dirLight.dir, input.normal) * dirLight.diffuse);

	for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
	{
		ambient += light[i].ambient;

		float3 pLightVec = light[i].pos - input.worldPos;
		float d = length(pLightVec);

		if (d <= light[i].range)
		{
			pLightVec /= d;
			float lightRatio = dot(pLightVec, input.normal);

			if (lightRatio > 0.0f)
			{
				pointColor = lightRatio * diffuse * light[i].diffuse;
				pointColor /= light[i].att[0] + (light[i].att[1] * d) + (light[i].att[2] * (d*d));
				finalColor = saturate(finalColor + pointColor);
			}
		}
	}

	float3 finalAmbient = saturate(diffuse * (ambient + dirLight.ambient / NUMBER_OF_LIGHTS));

	finalColor = saturate(finalColor + dirColor + finalAmbient);

	return float4(finalColor, diffuse.a);
}