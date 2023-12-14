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
#include "convar.h"
#include <windows.h>

// We're going to be making a screenspace effect. Therefore, we need the
// screenspace vertex shader.
//#include "SDK_screenspaceeffect_vs20.inc"
//#include "lightmappedgeneric_vs20.inc"
// We also need to include the pixel shader for our own shader.
// Note that the shader compiler generates both 2.0 and 2.0b versions.
// Need to include both.
#include "textureshader_ps20.inc"
#include "textureshader_vs20.inc"
//#include "textureshader_ps20b.inc"
// ----------------------------------------------------------------------------
// This macro defines the start of the shader. Effectively, every shader is
// 
// ----------------------------------------------------------------------------
BEGIN_SHADER(MyTextureShader, "Help for my shader.")

// ----------------------------------------------------------------------------
// This block is where you'd define inputs that users can feed to your
// shader.
// ----------------------------------------------------------------------------
BEGIN_SHADER_PARAMS

END_SHADER_PARAMS
SHADER_INIT_PARAMS()
{

}
// ----------------------------------------------------------------------------
// This is the shader initialization block. This disgusting macro defines
// a bunch of ick that makes this shader work.
// ----------------------------------------------------------------------------
SHADER_INIT
{
	/* Load texture */
	LoadTexture(BASETEXTURE);
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
		
		pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);
		pShaderShadow->VertexShaderVertexFormat(fmt, 2, 0, 0);
		// We don't need to write to the depth buffer.
		// Precache and set the screenspace shader.
		DECLARE_STATIC_VERTEX_SHADER(textureshader_vs20);
		SET_STATIC_VERTEX_SHADER(textureshader_vs20);

		DECLARE_STATIC_PIXEL_SHADER(textureshader_ps20);
		SET_STATIC_PIXEL_SHADER(textureshader_ps20);

		DefaultFog();
		
	}

		// ----------------------------------------------------------------------------
		// This section is called every frame.
		// ----------------------------------------------------------------------------
		DYNAMIC_STATE
	{
		BindTexture(SHADER_SAMPLER0, BASETEXTURE, FRAME);
		float sin_time;
		sin_time = sin((timeGetTime() / 1000.0f));
		pShaderAPI->SetVertexShaderConstant(VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, &sin_time);
		

		DECLARE_DYNAMIC_VERTEX_SHADER(textureshader_vs20);
		SET_DYNAMIC_VERTEX_SHADER(textureshader_vs20);

		// Use our custom pixel shader.
		DECLARE_DYNAMIC_PIXEL_SHADER(textureshader_ps20);
		SET_DYNAMIC_PIXEL_SHADER(textureshader_ps20);


	}

		// NEVER FORGET THIS CALL! This is what actually
		// draws your shader!
	Draw();
}

END_SHADER