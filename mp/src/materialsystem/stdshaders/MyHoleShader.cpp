// ----------------------------------------------------------------------------
// MYSHADER.CPP
//
// This file defines the C++ component of the example shader.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

// Must include this. Contains a bunch of macro definitions along with the
// declaration of CBaseShader.
#include "BaseVSShader.h"

// We're going to be making a screenspace effect. Therefore, we need the
// screenspace vertex shader.
//#include "SDK_screenspaceeffect_vs20.inc"
//#include "lightmappedgeneric_vs20.inc"
// We also need to include the pixel shader for our own shader.
// Note that the shader compiler generates both 2.0 and 2.0b versions.
// Need to include both.
#include "my_pixelshader_ps20.inc"
#include "my_pixelshader_vs20.inc"
#include "my_pixelshader_ps20b.inc"
// ----------------------------------------------------------------------------
// This macro defines the start of the shader. Effectively, every shader is
// 
// ----------------------------------------------------------------------------
BEGIN_SHADER(MyHoleShader, "Help for my hole shader.")

// ----------------------------------------------------------------------------
// This block is where you'd define inputs that users can feed to your
// shader.
// ----------------------------------------------------------------------------
BEGIN_SHADER_PARAMS
SHADER_PARAM(MYCOLOR, SHADER_PARAM_TYPE_COLOR, "[0 0 0 0]", "What color to render with.")
END_SHADER_PARAMS
SHADER_INIT_PARAMS()
{
	// Ensure we've recieved a color.
	if (!params[MYCOLOR]->IsDefined())
	{
		// Set to black error color.
		params[MYCOLOR]->SetVecValue(0.0f, 0.0f, 0.0f, 1.0f);
	}
}
// ----------------------------------------------------------------------------
// This is the shader initialization block. This disgusting macro defines
// a bunch of ick that makes this shader work.
// ----------------------------------------------------------------------------
SHADER_INIT
{

}

// ----------------------------------------------------------------------------
// We want this shader to operate on the frame buffer itself. Therefore,
// we need to set this to true.
// ----------------------------------------------------------------------------
bool NeedsFullFrameBufferTexture(IMaterialVar **params, bool bCheckSpecificToThisFrame /* = true */) const
{
	return true;
}

// ----------------------------------------------------------------------------
// This block should return the name of the shader to fall back to if
// we fail to bind this shader for any reason.
// ----------------------------------------------------------------------------
SHADER_FALLBACK
{
	// Requires DX9 + above
	if (g_pHardwareConfig->GetDXSupportLevel() < 90)
	{
		Assert(0);
		return "Wireframe";
	}
	return 0;
}

// ----------------------------------------------------------------------------
// This implements the guts of the shader drawing code.
// ----------------------------------------------------------------------------
SHADER_DRAW
{
	// ----------------------------------------------------------------------------
	// This section is called when the shader is bound for the first time.
	// You should setup any static state variables here.
	// ----------------------------------------------------------------------------
	SHADOW_STATE
	{
		// Setup the vertex format.
		int fmt = VERTEX_POSITION;
		pShaderShadow->VertexShaderVertexFormat(fmt, 1, 0, 0);

		// We don't need to write to the depth buffer.
		pShaderShadow->EnableDepthWrites(false);
		pShaderShadow->EnableDepthTest(false);
		//pShaderShadow->DepthFunc(SHADER_DEPTHFUNC_NEVER);
		
		//pShaderShadow->EnableAlphaTest(true);
		//pShaderShadow->AlphaFunc(SHADER_ALPHAFUNC_NEVER, 0);
		//pShaderShadow->EnableAlphaWrites(true);

		pShaderShadow->EnableBlending(true);
		pShaderShadow->BlendFunc(SHADER_BLEND_ZERO, SHADER_BLEND_DST_COLOR);
		pShaderShadow->BlendFunc(SHADER_BLEND_ZERO, SHADER_BLEND_DST_ALPHA);
		pShaderShadow->EnablePolyOffset(SHADER_POLYOFFSET_DECAL);
		//pShaderShadow->
		//pShaderShadow->EnableSRGBWrite(false);
		//pShaderShadow->EnableVertexBlend(false);
		//pShaderShadow->Enab
		//pShaderShadow->BlendFunc(SHADER_BLEND_ZERO, SHADER_BLEND_ZERO);
		//pShaderShadow->EnableAlphaWrites(true);
		
		// Precache and set the screenspace shader.
		DECLARE_STATIC_VERTEX_SHADER(my_pixelshader_vs20);
		SET_STATIC_VERTEX_SHADER(my_pixelshader_vs20);
		// Precache and set the example shader.
		if (g_pHardwareConfig->SupportsPixelShaders_2_b())
		{
			DECLARE_STATIC_PIXEL_SHADER(my_pixelshader_ps20b);
			SET_STATIC_PIXEL_SHADER(my_pixelshader_ps20b);
		}
		else
		{
			DECLARE_STATIC_PIXEL_SHADER(my_pixelshader_ps20);
			SET_STATIC_PIXEL_SHADER(my_pixelshader_ps20);
		}
	}

		// ----------------------------------------------------------------------------
		// This section is called every frame.
		// ----------------------------------------------------------------------------
		DYNAMIC_STATE
	{
		
		pShaderAPI->SetStencilEnable(true);
		pShaderAPI->SetStencilFailOperation(STENCILOPERATION_KEEP);
		pShaderAPI->SetStencilZFailOperation(STENCILOPERATION_KEEP);
		pShaderAPI->SetStencilPassOperation(STENCILOPERATION_REPLACE);
		pShaderAPI->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_ALWAYS);
		pShaderAPI->SetStencilWriteMask(0xFF);
		pShaderAPI->SetStencilReferenceValue(1);
		
		
		/*int w;
		int h;
		pShaderAPI->GetBackBufferDimensions(w, h);
		w /= 3;
		h /= 3;
		int x_start = w;
		int y_start = h;
		int x_end = x_start + w;
		int y_end = y_start + h;
		pShaderAPI->ClearStencilBufferRectangle(x_start, y_start, x_end, y_end, 1);
		pShaderAPI->ClearStencilBufferRectangle(0, 0, 25, 25, 1);*/

		// Pixel shader constant register 0 will contain the
		// color passed in by the material.

		float c0[4];
		params[MYCOLOR]->GetVecValue(c0, 4);
		pShaderAPI->SetPixelShaderConstant(0, c0, ARRAYSIZE(c0) / 4);
		// Use the sdk_screenspaceeffect_vs20 vertex shader.
		DECLARE_DYNAMIC_VERTEX_SHADER(my_pixelshader_vs20);
		SET_DYNAMIC_VERTEX_SHADER(my_pixelshader_vs20);

		// Use our custom pixel shader.
		if (g_pHardwareConfig->SupportsPixelShaders_2_b())
		{
			DECLARE_DYNAMIC_PIXEL_SHADER(my_pixelshader_ps20b);
			SET_DYNAMIC_PIXEL_SHADER(my_pixelshader_ps20b);
		}
		else
		{
			DECLARE_DYNAMIC_PIXEL_SHADER(my_pixelshader_ps20);
			SET_DYNAMIC_PIXEL_SHADER(my_pixelshader_ps20);
		}


	}

		// NEVER FORGET THIS CALL! This is what actually
		// draws your shader!

	Draw();
	if (pShaderAPI)
	{
		pShaderAPI->SetStencilEnable(false);
	}
}

END_SHADER