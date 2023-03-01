#version 460

#extension GL_ARB_shading_language_include : require

#include "common.glsl"
#include "pbr.glsl"
#include "shadow.glsl"
#include "postprocess.glsl"
#include "transparent.glsl"

in VertexData
{
	vec3 position;
	vec3 normal;
	vec3 color;
    flat int instanceID;
} fs_in;

// material properties
uniform float uMetallic;
uniform float uRoughness;
uniform float uAlpha;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out ivec4 fragIndices;

subroutine void RenderPass(void);
layout(location = 0) subroutine uniform RenderPass renderPass;

vec3 GetViewDir()
{
	// orthogonal projection
	if(uTransform.proj[3][3] == 1.0)
		return vec3(0, 0, 1);
	// perspective projection
	return normalize(-fs_in.position);
}

vec3 Shade()
{
	vec3 N = normalize(fs_in.normal);
	vec3 V = GetViewDir();

	float dotNV = dot(N, V);
	if (dotNV < 0.0)	N = -N;
	
	vec3 Lo = vec3(0);

	// for main directional light
	{
		vec3 L = normalize(uLight.direction.xyz);
	
		// evaluate BRDF
		vec3 brdf = EvalPBR(fs_in.color, uMetallic, uRoughness, N, V, L);

		// do not consider attenuation
		vec3 radiance = uLight.intensity.rgb * uLight.intensity.a;

		// shadow
		vec3 shadowFactor = vec3(1);
		if (uLight.direction.w != 0)
			shadowFactor = GetShadowFactor(fs_in.position);

		Lo += shadowFactor * radiance * brdf;
	}
	
	// for a simple camera light
	{
		// evaluate BRDF
		vec3 brdf = EvalPBR(fs_in.color, uMetallic, uRoughness, N, V, V);

		// do not consider attenuation
		vec3 radiance = uLight.camera.rgb * uLight.camera.a;

		// no shadow...
		Lo += radiance * brdf;
	}

	// ambient light
	vec3 ambient = uLight.ambient.rgb * uLight.ambient.a * fs_in.color;

	// final color
	vec3 color = ambient + Lo;
	color = ReinhardTonemap(color);
	color = GammaCorrect(color);

	return color;
}


vec3 ShadeTransparency()
{
	vec3 N = normalize(fs_in.normal);
	vec3 V = GetViewDir();

	float dotNV = dot(N, V);
	if (dotNV < 0.0)	N = -N;
	
	vec3 Lo = vec3(0);

	// for main directional light
	{
		vec3 L = normalize(uLight.direction.xyz);
	
		// evaluate BRDF
		vec3 brdf = EvalPBR(fs_in.color, uMetallic, uRoughness, N, V, L);

		// do not consider attenuation
		vec3 radiance = uLight.intensity.rgb * uLight.intensity.a;

		// shadow
		vec3 shadowFactor = vec3(1);
//		if (uLight.direction.w != 0)
//			shadowFactor = GetShadowFactor(fs_in.position);

		Lo += shadowFactor * radiance * brdf;
	}
	
	// for a simple camera light
	{
		// evaluate BRDF
		vec3 brdf = EvalPBR(fs_in.color, uMetallic, uRoughness, N, V, V);

		// do not consider attenuation
		vec3 radiance = uLight.camera.rgb * uLight.camera.a;

		// no shadow...
		Lo += radiance * brdf;
	}

	// ambient light
	vec3 ambient = uLight.ambient.rgb * uLight.ambient.a * fs_in.color;

	// final color
	vec3 color = ambient + Lo;
	color = ReinhardTonemap(color);
	color = GammaCorrect(color);

	return color;
}

layout(index = 0) subroutine(RenderPass) void ColorPass(void)
{
	fragColor.rgb = Shade();
	fragColor.a = 1.0;
	
	// store index
	fragIndices.r = uVars.index;
	fragIndices.g = fs_in.instanceID;
}

layout(index = 1) subroutine(RenderPass) void ShadowPass(void)
{
	fragColor = vec4(GetShadowMoments(), 0.0, 0.0);
}


layout(index = 2) subroutine(RenderPass) void TransparencyLinkedList(void)
{
	// Get the index of the next free node in the buffer.
	uint freeNodeIndex = atomicCounterIncrement(u_freeNodeIndex);

	// Check, if still space in the buffer.
	if (freeNodeIndex < uMaxNodes)
	{
		// Replace new index as the new head and gather the previous head, which will be the next index of this entry.
		uint nextIndex = imageAtomicExchange(u_headIndex, ivec2(gl_FragCoord.xy), freeNodeIndex);

		// Store the color, depth and the next index for later resolving.
		nodes[freeNodeIndex].color = vec4(ShadeTransparency(), uAlpha);
		nodes[freeNodeIndex].depth = gl_FragCoord.z;
		nodes[freeNodeIndex].nextIndex = nextIndex;
		nodes[freeNodeIndex].geometryID = uVars.index;
		nodes[freeNodeIndex].instanceID = fs_in.instanceID;
	}
	// No output to the framebuffer.
}


void main(void) { 
	renderPass();
} 