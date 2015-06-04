Texture2D objTex : register(t0);
Texture2D gShadowMap : register(t1);
SamplerState objSamp : register(s0);
SamplerState pointSampler : register(s1);

#define MAX_NUMBER_OF_LIGHTS 10

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
	int nLights;
	float pad1, pad2, pad3;
};

cbuffer cbPointLight : register(b1)
{
	PointLight light[MAX_NUMBER_OF_LIGHTS];
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 hue : TEXCOORD1;
	float4 shadowPos : TEXCOORD2;
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

	float3 pLightVec;
	float d;

	for (int i = 0; i < nLights; i++)
	{
		pLightVec = light[i].pos - input.worldPos;
		d = length(pLightVec);

		if (d <= light[i].range)
		{
			ambient += light[i].ambient;
			pLightVec /= d;
			float lightRatio = dot(input.normal, pLightVec);

			if (lightRatio > 0.0f)
			{
				pointColor = lightRatio * diffuse * light[i].diffuse;
				pointColor /= light[i].att[0] + (light[i].att[1] * d) + (light[i].att[2] * (d*d));
				finalColor += pointColor;
			}
		}
	}

	float3 finalAmbient = saturate(ambient + dirLight.ambient) * diffuse;

	// Check if pixel is shadowed.
	float4 shadowMapSamplePosition;
	float shadowMapValue;
	float depth;

	shadowMapSamplePosition = input.shadowPos * 0.5f + 0.5f;	// Convert from world to texture space.
	shadowMapSamplePosition.y = 1 - shadowMapSamplePosition.y;	// This seem to correct upside down texture.
	shadowMapValue = gShadowMap.Sample(pointSampler, shadowMapSamplePosition.xy);
	depth = (input.shadowPos.xyz / input.shadowPos.w).z;

	if (shadowMapValue < depth)
		return float4(0, 0, 0, 1);
	
	finalColor = saturate((finalColor + dirColor) * diffuse + finalAmbient);

	return float4(finalColor, diffuse.a);
}