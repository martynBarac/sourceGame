#include "cbase.h"
#include "particles_simple.h"
#include "iviewrender.h"
#include "proxyentity.h"
#include "materialsystem/imaterialvar.h"
#include "model_types.h"
#include "engine/ivmodelinfo.h"
#include "clienteffectprecachesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_BreakableHoles : public C_BaseEntity
{

	DECLARE_CLASS(C_BreakableHoles, C_BaseEntity);
	DECLARE_DATADESC();
	DECLARE_CLIENTCLASS();
public:

	C_BreakableHoles();
	~C_BreakableHoles();
public:
	virtual void Spawn();
	virtual void SpawnClientEntity();
	int			DrawModel(int flags);
	virtual void TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator = NULL);
private:
	CMaterialReference	m_WhiteMaterial;
	//C_BaseAnimating *pEnt;
	CUtlVector <C_BaseAnimating *> pEnts;
};
BEGIN_DATADESC(C_BreakableHoles)
END_DATADESC()

int C_BreakableHoles::DrawModel(int flags)
{
	for (int i = 0; i < pEnts.Count(); ++i)
	{
		//C_BaseAnimating *pEnt = pEnts[i];
		if (pEnts[i])
		{
			CMatRenderContextPtr pRenderContext(materials);
			pRenderContext->SetStencilEnable(true);
			pRenderContext->SetStencilFailOperation(STENCILOPERATION_KEEP);
			pRenderContext->SetStencilZFailOperation(STENCILOPERATION_KEEP);
			pRenderContext->SetStencilPassOperation(STENCILOPERATION_REPLACE);

			pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_ALWAYS);
			pRenderContext->SetStencilWriteMask(0xFF);
			pRenderContext->SetStencilReferenceValue(1);

			pRenderContext->DepthRange(0.0f, 0.01f);
			render->SetBlend(0);

			//modelrender->ForcedMaterialOverride(m_WhiteMaterial);

			pEnts[i]->DrawModel(STUDIO_RENDER);



			//modelrender->ForcedMaterialOverride(NULL);

			render->SetBlend(1);
			pRenderContext->DepthRange(0.0f, 1.0f);
			pRenderContext->SetStencilEnable(false);
		}
	}

	//pEnt->Remove();
	
	BaseClass::DrawModel(flags | STUDIO_TRANSPARENCY);
	return 0;
}
C_BreakableHoles::C_BreakableHoles()
{
	/*KeyValues *pVMTKeyValues = new KeyValues("VertexLitGeneric");
	pVMTKeyValues->SetString("$basetexture", "vgui/white");
	pVMTKeyValues->SetInt("$selfillum", 1);
	pVMTKeyValues->SetString("$selfillummask", "vgui/white");
	pVMTKeyValues->SetInt("$vertexalpha", 1);
	m_WhiteMaterial.Init("__geglowwhite", TEXTURE_GROUP_CLIENT_EFFECTS, pVMTKeyValues);
	m_WhiteMaterial->Refresh();*/

}
C_BreakableHoles::~C_BreakableHoles()
{

}
void C_BreakableHoles::Spawn()
{
	BaseClass::Spawn();
}

void C_BreakableHoles::SpawnClientEntity()
{

	BaseClass::SpawnClientEntity();

}
void C_BreakableHoles::TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator)
{


	int idx;
	idx = pEnts.AddToTail();
	pEnts[idx] = new C_BaseAnimating;
	pEnts[idx]->SetEffects(EF_NODRAW);
	pEnts[idx]->SetAbsOrigin(ptr->endpos);
	QAngle orient;
	
	VectorAngles(ptr->plane.normal, orient);
	pEnts[idx]->SetAbsAngles(orient);
	
	pEnts[idx]->SetModel("models/bullethole.mdl");
	//pEnt->Spawn();
	BaseClass::TraceAttack(info, vecDir, ptr, pAccumulator);
}


IMPLEMENT_CLIENTCLASS_DT(C_BreakableHoles, DT_BreakableHoles, CBreakableHoles)
END_RECV_TABLE()