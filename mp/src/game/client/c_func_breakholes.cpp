#include "cbase.h"
#include "particles_simple.h"
#include "iviewrender.h"
#include "proxyentity.h"
#include "materialsystem/imaterialvar.h"
#include "model_types.h"
#include "engine/ivmodelinfo.h"
#include "clienteffectprecachesystem.h"
#include "polyslicer\polygon.h"




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
	virtual void		OnDataChanged(DataUpdateType_t updateType);
	int			DrawModel(int flags);
	virtual void TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator = NULL);
	bool		RenderBrushModelSurface(IClientEntity* pBaseEntity, IBrushSurface* pBrushSurface);

	//Polygon pPolygon;
private:
	Vector m_vLLVertex;
	Vector m_vULVertex;
	Vector m_vLRVertex;
	Vector m_vURVertex;

	Vector m_vLLVertexT;
	Vector m_vULVertexT;
	Vector m_vLRVertexT;
	Vector m_vURVertexT;
	Vector mvNormal;
	Vector lasthitpos;
	matrix3x4_t matOrient;
	matrix3x4_t matOrientInv;


	int m_nQuadError;
	CMaterialReference	m_WhiteMaterial;
	//C_BaseAnimating *pEnt;
	CUtlVector <C_BaseAnimating *> pEnts;
	CUtlVector <C_BaseAnimating *> pEnts2;
	CUtlVector <C_BaseAnimating *> pEnts3;
	Vector vQuads[4];
	std::vector<std::vector<Vector>> vtriangles;
	Polygon pPolygon;
	Polygon * pFirsthole;
	bool globalOnce = false;
	std::vector<Polygon*> pHoles;
	bool isClockwise(Vector a, Vector b, Vector c);
};
BEGIN_DATADESC(C_BreakableHoles)
END_DATADESC()

bool C_BreakableHoles::isClockwise(Vector a, Vector b, Vector c)
{
	float det = a.y*(b.z - c.z) - a.z*(b.y - c.y) + b.y*c.z - b.z*c.y;
	return det > 0;
}

int C_BreakableHoles::DrawModel(int flags)
{

	CMatRenderContextPtr pRenderContext(materials);
	
	
	/*pRenderContext->SetStencilEnable(true);
	pRenderContext->SetStencilFailOperation(STENCILOPERATION_KEEP);
	pRenderContext->SetStencilZFailOperation(STENCILOPERATION_KEEP);
	pRenderContext->SetStencilPassOperation(STENCILOPERATION_REPLACE);

	pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_ALWAYS);
	pRenderContext->SetStencilWriteMask(0xFF);
	pRenderContext->SetStencilReferenceValue(1);*/

	

	
	/*CMeshBuilder	pMeshBuilderr;
	CMeshBuilder*	pMeshBuilder = &pMeshBuilderr;
	IMesh*			pMesh = NULL;
	pRenderContext->Flush(false);
	pRenderContext->Bind(m_WhiteMaterial, (IClientRenderable*)this);
	pMesh = pRenderContext->GetDynamicMesh();
	
	pMeshBuilder->Begin(pMesh, MATERIAL_QUADS, 1);
	pMeshBuilder->Position3f(vQuads[0].x, vQuads[0].y, vQuads[0].z);
	pMeshBuilder->AdvanceVertex();

	pMeshBuilder->Position3f(vQuads[1].x, vQuads[1].y, vQuads[1].z);
	pMeshBuilder->AdvanceVertex();

	pMeshBuilder->Position3f(vQuads[2].x, vQuads[2].y, vQuads[2].z);
	pMeshBuilder->AdvanceVertex();

	pMeshBuilder->Position3f(vQuads[3].x, vQuads[3].y, vQuads[3].z);
	pMeshBuilder->AdvanceVertex();

	pMeshBuilder->End();

	pRenderContext->OverrideAlphaWriteEnable(true, true);
	pRenderContext->OverrideDepthEnable(true, false);
	pRenderContext->DepthRange(0.0f, 0.01f);
	render->SetBlend(0);
	pMesh->Draw();*/

	
	
	
	


	//render->SetBlend(1);
	//pRenderContext->DepthRange(0.0f, 1.0f);
	//pRenderContext->OverrideDepthEnable(false, false);

	//pRenderContext->OverrideAlphaWriteEnable(false, false);

	/*for (int i = 0; i < pEnts3.Count(); ++i)
	{
		if (pEnts3[i])
		{
			pRenderContext->SetStencilFailOperation(STENCILOPERATION_KEEP);
			pRenderContext->SetStencilZFailOperation(STENCILOPERATION_KEEP);
			pRenderContext->SetStencilPassOperation(STENCILOPERATION_REPLACE);

			pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_ALWAYS);
			pRenderContext->SetStencilWriteMask(0xFF);
			pRenderContext->SetStencilReferenceValue(1);

			pRenderContext->DepthRange(0.0f, 0.01f);
			render->SetBlend(0);

			//modelrender->ForcedMaterialOverride(m_WhiteMaterial);

			pEnts3[i]->DrawModel(STUDIO_RENDER);
			render->SetBlend(1);

		}
	}
	//modelrender->ForcedMaterialOverride(NULL);

	
	pRenderContext->DepthRange(0.0f, 1.0f);
	for (int i = 0; i < pEnts2.Count(); ++i)
	{
		if (pEnts2[i])
		{

			pRenderContext->SetStencilReferenceValue(0);
			pRenderContext->SetStencilTestMask(0xFF);
			pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_EQUAL);
			pRenderContext->SetStencilPassOperation(STENCILOPERATION_ZERO);
			pRenderContext->SetStencilZFailOperation(STENCILOPERATION_KEEP);
			pEnts2[i]->DrawModel(STUDIO_RENDER);

			
			
		}
	}
	pRenderContext->ClearStencilBufferRectangle(0, 0, ScreenWidth(), ScreenHeight(), 0);
	for (int i = 0; i < pEnts.Count(); ++i)
	{
		//C_BaseAnimating *pEnt = pEnts[i];
		if (pEnts[i])
		{

			
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



		}
	}*/
	//pEnt->Remove();
	
	//pRenderContext->SetStencilEnable(true);
	//pRenderContext->SetStencilReferenceValue(0);
	//pRenderContext->SetStencilTestMask(0xFF);
	//pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_EQUAL);
	//pRenderContext->SetStencilPassOperation(STENCILOPERATION_ZERO);
	

	
	//BaseClass::DrawModel(flags | STUDIO_TRANSPARENCY);
//	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->Flush(false);
	pRenderContext->Bind(m_WhiteMaterial, (IClientRenderable*)this);
	CMeshBuilder	pMeshBuilderr;
	CMeshBuilder*	pMeshBuilder = &pMeshBuilderr;
	IMesh*			pMesh = NULL;
	pMesh = pRenderContext->GetDynamicMesh();
	for (size_t i = 0; i < vtriangles.size(); i++)
	{
		pMeshBuilder->Begin(pMesh, MATERIAL_TRIANGLES, 1);
		pMeshBuilder->Position3f(vtriangles[i][0].x, vtriangles[i][0].y, vtriangles[i][0].z);
		pMeshBuilder->Color4ub(255, 255, 255, 255);
		//pMeshBuilder->Normal3fv(mvNormal.Base());
		pMeshBuilder->TexCoord2f(0, vtriangles[i][3].z, vtriangles[i][3].y);
		pMeshBuilder->AdvanceVertex();

		pMeshBuilder->Position3f(vtriangles[i][1].x, vtriangles[i][1].y, vtriangles[i][1].z);
		pMeshBuilder->Color4ub(255, 255, 255, 255);
		//pMeshBuilder->Normal3fv(mvNormal.Base());
		pMeshBuilder->TexCoord2f(0, vtriangles[i][4].z, vtriangles[i][4].y);
		pMeshBuilder->AdvanceVertex();

		pMeshBuilder->Position3f(vtriangles[i][2].x, vtriangles[i][2].y, vtriangles[i][2].z);
		pMeshBuilder->Color4ub(255, 255, 255, 255);
		//pMeshBuilder->Normal3fv(mvNormal.Base());
		pMeshBuilder->TexCoord2f(0, vtriangles[i][5].z, vtriangles[i][5].y);
		pMeshBuilder->AdvanceVertex();
		pMeshBuilder->End();
		
		pMesh->Draw();
	}
	/*for (size_t i = 0; i < vtriangles.size(); i++)
	{
		pMeshBuilder->Begin(pMesh, MATERIAL_TRIANGLES, 1);
		pMeshBuilder->Position3f(vtriangles[i][2].x, vtriangles[i][2].y, vtriangles[i][2].z);
		pMeshBuilder->Color4ub(255, 255, 255, 255);
		//pMeshBuilder->Normal3fv(mvNormal.Base());
		pMeshBuilder->AdvanceVertex();

		pMeshBuilder->Position3f(vtriangles[i][1].x, vtriangles[i][1].y, vtriangles[i][1].z);
		pMeshBuilder->Color4ub(255, 255, 255, 255);
		//pMeshBuilder->Normal3fv(mvNormal.Base());
		pMeshBuilder->AdvanceVertex();

		pMeshBuilder->Position3f(vtriangles[i][0].x, vtriangles[i][0].y, vtriangles[i][0].z);
		pMeshBuilder->Color4ub(255, 255, 255, 255);
		//pMeshBuilder->Normal3fv(mvNormal.Base());
		pMeshBuilder->AdvanceVertex();
		pMeshBuilder->End();

		pMesh->Draw();
	}*/
	//}

	
	


	//pRenderContext->ClearStencilBufferRectangle(0, 0, ScreenWidth(), ScreenHeight(), 0);
	//pRenderContext->SetStencilEnable(false);
	return 0;
}

C_BreakableHoles::C_BreakableHoles()
{
	//KeyValues *pVMTKeyValues = new KeyValues("UnLitGeneric");
	//pVMTKeyValues->SetString("$basetexture", "vgui/white");
//	pVMTKeyValues->SetInt("$selfillum", 1);
//	pVMTKeyValues->SetString("$selfillummask", "vgui/white");
	//pVMTKeyValues->SetInt("$alpha", 0);
	//pVMTKeyValues->SetInt("translucent", 1);
	//pVMTKeyValues->SetInt("$alphatest", 1);
	//pVMTKeyValues->SetInt("$alphatestreference", 1);
	//m_WhiteMaterial.Init("__geglowwhite", TEXTURE_GROUP_CLIENT_EFFECTS, pVMTKeyValues);
	//m_WhiteMaterial->Refresh();
	

	

}

void C_BreakableHoles::OnDataChanged(DataUpdateType_t updateType)
{
	C_BaseEntity::OnDataChanged(updateType);
	if (updateType == DATA_UPDATE_CREATED)
	{
		IMaterial* pMaterial;
		modelinfo->GetModelMaterials(const_cast<model_t*>(GetModel()), 1, &pMaterial);
		KeyValues *pVMTKeyValues = new KeyValues("UnLitGeneric");
		bool foundVar;
		pVMTKeyValues->SetString("$basetexture", (pMaterial->FindVar("$basetexture", &foundVar, false))->GetStringValue());
		pVMTKeyValues->SetInt("$selfillum", 0);
		//pVMTKeyValues->SetString("$selfillummask", "vgui/white");
		//pVMTKeyValues->SetInt("$alpha", 0);
		//pVMTKeyValues->SetInt("translucent", 1);
		//pVMTKeyValues->SetInt("$alphatest", 1);
		pVMTKeyValues->SetInt("$alphatestreference", 1);
		m_WhiteMaterial.Init("__geglowwhite", TEXTURE_GROUP_CLIENT_EFFECTS, pVMTKeyValues);
		m_WhiteMaterial->Refresh();
		
	}
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
	// FIXME: Bad way to handle adding holes
	/* 
	To connect holes:
	Generate circle mesh with CMeshBuilder
	Union operation on circles: https://wrfranklin.org/Research/Short_Notes/pnpoly.html
	//PNPOLY - Point Inclusion in Polygon Test
	W. Randolph Franklin (WRF) 
	Extrude operation to create walls
	*/
	
	if (!globalOnce)
	{

		cplane_t myPlane;
		Vector vecOrigin;

		Vector vNormal = CrossProduct(m_vLLVertex - m_vULVertex, m_vURVertex - m_vULVertex);
		mvNormal = vNormal;
		QAngle orient;
		matrix3x4_t matOrient;
		matrix3x4_t matOrientInv;
		VectorAngles(vNormal, orient);
		AngleMatrix(-orient, matOrient);
		AngleMatrix(orient, matOrientInv);
		Vector vStartPosT;
		


		
		Vector vStartPos = ptr->endpos;
		VectorTransform(vStartPos, matOrient, vStartPosT);

		//Project to 2D co-ordinate space (Should be z and y)
		VectorTransform(m_vLLVertex, matOrient, m_vLLVertexT);
		VectorTransform(m_vULVertex, matOrient, m_vULVertexT);
		VectorTransform(m_vLRVertex, matOrient, m_vLRVertexT);
		VectorTransform(m_vURVertex, matOrient, m_vURVertexT);

		Vector2f m_vLLVertex2d = Vector2f(m_vLLVertexT.y, m_vLLVertexT.z);
		Vector2f m_vULVertex2d = Vector2f(m_vULVertexT.y, m_vULVertexT.z);
		Vector2f m_vLRVertex2d = Vector2f(m_vLRVertexT.y, m_vLRVertexT.z);
		Vector2f m_vURVertex2d = Vector2f(m_vURVertexT.y, m_vURVertexT.z);
		pPolygon.points.push_back(m_vLLVertex2d);
		pPolygon.points.push_back(m_vULVertex2d);
		
		pPolygon.points.push_back(m_vURVertex2d);
		pPolygon.points.push_back(m_vLRVertex2d);
		pPolygon.generateDcel();
		pPolygon.sortEdgeList();
		Polygon * newHole = new Polygon;
		
		newHole->points.push_back(Vector2f(-1, 5));
		newHole->points.push_back(Vector2f(0, 0));
		newHole->points.push_back(Vector2f(5, 2));
		newHole->points.push_back(Vector2f(10.001, 0));
		newHole->points.push_back(Vector2f(9, 5));
		newHole->points.push_back(Vector2f(10, 10.01));
		newHole->points.push_back(Vector2f(5, 8));
		newHole->points.push_back(Vector2f(0, 10));

		
		
		
		
		newHole->generateDcel();
		newHole->markAsHole();
		newHole->sortEdgeList();

		newHole->translate(Vector2f(vStartPosT.y-10, vStartPosT.z));
		pFirsthole = newHole;
		//pPolygon.addHole(*newHole,0,0);
		//pPolygon.findIntersections();
		if (!(pPolygon.shouldDelete() || pPolygon.start_edge == nullptr))
		{
			pPolygon.sortEdgeList();
			pPolygon.generateBst();
			pPolygon.makeMonotone();


			Polygon* newPoly = nullptr;
			std::vector< std::vector<Vector2f>> triangles = pPolygon.triangulateMonotone(pPolygon.start_edge, newPoly);
			//Msg("HG3\n");
			if (newPoly != nullptr)
			{
				delete newPoly;
			}
			for (size_t j = 0; j < triangles.size(); j++)
			{

				std::vector<Vector> vtriangle;

				Vector point1 = Vector(m_vLRVertexT.x, triangles[j][0].x, triangles[j][0].y);
				Vector point2 = Vector(m_vLRVertexT.x, triangles[j][1].x, triangles[j][1].y);
				Vector point3 = Vector(m_vLRVertexT.x, triangles[j][2].x, triangles[j][2].y);

				Vector _point1;
				Vector _point2;
				Vector _point3;


				VectorTransform(point1, matOrientInv, _point1);
				VectorTransform(point2, matOrientInv, _point2);
				VectorTransform(point3, matOrientInv, _point3);

				if (isClockwise(point1, point2, point3))
				{
					vtriangle.push_back(_point1);
					vtriangle.push_back(_point2);
					vtriangle.push_back(_point3);
				}
				else
				{
					vtriangle.push_back(_point3);
					vtriangle.push_back(_point2);
					vtriangle.push_back(_point1);
				}
				//vtriangles.push_back(vtriangle);
			}

			for (size_t k = 0; k < pPolygon.start_edges.size(); k++)
			{
				Polygon* newPoly = nullptr;
				std::vector< std::vector<Vector2f>> triangles = pPolygon.triangulateMonotone(pPolygon.start_edges[k], newPoly);
				//Msg("HG3\n");
				if (newPoly != nullptr)
				{
					delete newPoly;
				}
				for (size_t j = 0; j < triangles.size(); j++)
				{

					std::vector<Vector> vtriangle;

					Vector point1 = Vector(m_vLRVertexT.x, triangles[j][0].x, triangles[j][0].y);
					Vector point2 = Vector(m_vLRVertexT.x, triangles[j][1].x, triangles[j][1].y);
					Vector point3 = Vector(m_vLRVertexT.x, triangles[j][2].x, triangles[j][2].y);

					Vector _point1;
					Vector _point2;
					Vector _point3;


					VectorTransform(point1, matOrientInv, _point1);
					VectorTransform(point2, matOrientInv, _point2);
					VectorTransform(point3, matOrientInv, _point3);
			
					if (isClockwise(point1, point2, point3))
					{
						vtriangle.push_back(_point1);
						vtriangle.push_back(_point2);
						vtriangle.push_back(_point3);
					}
					else
					{
						vtriangle.push_back(_point3);
						vtriangle.push_back(_point2);
						vtriangle.push_back(_point1);
					}
					//vtriangles.push_back(vtriangle);
				}
			}
			
		}
		
		
	}
	








	Vector vNormal = CrossProduct(m_vLLVertex - m_vULVertex, m_vURVertex - m_vULVertex);
	vNormal.x = vNormal.x / vNormal.Length();
	vNormal.y = vNormal.y / vNormal.Length();
	vNormal.z = vNormal.z / vNormal.Length();
	QAngle orient;
	matrix3x4_t matOrient;
	matrix3x4_t matOrientInv;
	VectorAngles(vNormal, orient);
	AngleMatrix(-orient, matOrient);
	AngleMatrix(orient, matOrientInv);
	Vector vStartPosT;




	Vector vStartPos = ptr->endpos;
	VectorTransform(vStartPos, matOrient, vStartPosT);
	
	if (globalOnce && vStartPosT != lasthitpos)
	{
		Polygon* _newPoly = new Polygon;
		_newPoly->duplicate(pPolygon);
		_newPoly->sortEdgeList();
		Polygon * newHole = new Polygon;

		newHole->points.push_back(Vector2f(-1, 5));
		newHole->points.push_back(Vector2f(0, 0));
		newHole->points.push_back(Vector2f(5, 2));
		newHole->points.push_back(Vector2f(10.001, 0));
		newHole->points.push_back(Vector2f(9, 5));
		newHole->points.push_back(Vector2f(10, 10.01));
		newHole->points.push_back(Vector2f(5, 8));
		newHole->points.push_back(Vector2f(0, 10));
		

		newHole->generateDcel();
		newHole->markAsHole();
		newHole->sortEdgeList();
		//Msg("STartpos: %f, %f, %f \n", vStartPosT.x, vStartPosT.y, vStartPosT.z);
		//Msg("hitpos: %f, %f, %f \n", vStartPosT.x, vStartPosT.y, vStartPosT.z);
		newHole->translate(Vector2f(vStartPosT.y - 5, vStartPosT.z-5));
		//pHoles.push_back(newHole);
		//Polygon* tempHole;
		pFirsthole->addHole(*newHole, 0, 0, false);
		//firstHole->generateBst();
		//firstHole->sortEdgeList();
		//drawPolygon(*tempHole, gxpan, gypan, gscale, window);


		pFirsthole->findIntersectionsHoles();
		//drawPolygon(*firstHole, gxpan, gypan, gscale, window);
		//int key = cv::waitKey(0);
		pFirsthole->sortEdgeList();
		//Msg("FIRSTHOLE: %f, %f, %f \n", pFirsthole->start_edge->orig.x, pFirsthole->start_edge->orig.y, vStartPosT.z);
		if (pFirsthole!=nullptr)
		{
			Polygon* tempHole = new Polygon;
			tempHole->duplicate(*pFirsthole);
			//tempHole->generateDcel();
			tempHole->markAsHole();
			tempHole->sortEdgeList();
			//polygons[i]->sortEdgeList();

			//Msg("tempHole: %f, %f, %f \n", tempHole->start_edge->orig.x, tempHole->start_edge->orig.y, vStartPosT.z);
			_newPoly->addHole(*tempHole, 0, 0);
			_newPoly->findIntersections();
		}

		if (!(_newPoly->shouldDelete() || _newPoly->start_edge == nullptr))
		{

			
		vtriangles.clear();
		_newPoly->sortEdgeList();
		for (int i = 0; i < _newPoly->edges.size(); i++)
		{
			Vector2f p = _newPoly->edges[i]->orig;
			Vector2f pNext = _newPoly->edges[i]->next->orig;
			Vector _p = Vector(m_vLRVertexT.x, p.x, p.y);
			Vector _pNext = Vector(m_vLRVertexT.x, pNext.x, pNext.y);;
			Vector __p;
			Vector __pNext;
			VectorTransform(_p, matOrientInv, __p);
			VectorTransform(_pNext, matOrientInv, __pNext);
			std::vector<Vector> vtriangleSide;
			std::vector<Vector> vtriangleSide2;
			vtriangleSide.push_back(__p);
			vtriangleSide.push_back(__pNext);
			vtriangleSide.push_back(__pNext+vNormal*8);

			vtriangleSide.push_back(Vector(0, 0.2, 0.2));
			vtriangleSide.push_back(Vector(0, 0, 0.2));
			vtriangleSide.push_back(Vector(0, 0, 0));
			
			vtriangleSide2.push_back(__p );
			vtriangleSide2.push_back(__pNext + vNormal * 8);
			vtriangleSide2.push_back(__p + vNormal * 8);

			vtriangleSide2.push_back(Vector(0, 0.2, 0.2));
			vtriangleSide2.push_back(Vector(0, 0, 0.2));
			vtriangleSide2.push_back(Vector(0, 0, 0));

			vtriangles.push_back(vtriangleSide);
			vtriangles.push_back(vtriangleSide2);
		}
		

		_newPoly->generateBst();
		_newPoly->makeMonotone();


		Polygon* newPoly = nullptr;
		std::vector< std::vector<Vector2f>> triangles = _newPoly->triangulateMonotone(_newPoly->start_edge, newPoly);
		//Msg("HG3\n");
		if (newPoly != nullptr)
		{
		delete newPoly;
		}
		
		for (size_t j = 0; j < triangles.size(); j++)
		{

		std::vector<Vector> vtriangle;
		std::vector<Vector> vtriangle2;

		Vector point1 = Vector(m_vLRVertexT.x, triangles[j][0].x, triangles[j][0].y);
		Vector point2 = Vector(m_vLRVertexT.x, triangles[j][1].x, triangles[j][1].y);
		Vector point3 = Vector(m_vLRVertexT.x, triangles[j][2].x, triangles[j][2].y);

		Vector _point1;
		Vector _point2;
		Vector _point3;

		VectorTransform(point1, matOrientInv, _point1);
		VectorTransform(point2, matOrientInv, _point2);
		VectorTransform(point3, matOrientInv, _point3);

		Vector __point1 = _point1;
		Vector __point2 = _point2;
		Vector __point3 = _point3;

		__point1 = __point1 + vNormal * 8;
		__point2 = __point2 + vNormal * 8;
		__point3 = __point3 + vNormal * 8;

		//Msg("%f, %f, %f \n", _point1.x, _point1.y, _point1.z);
		//Msg("%f, %f, %f \n", _point2.x, _point2.y, _point2.z);
		//Msg("%f, %f, %f \n", _point3.x, _point3.y, _point3.z);
		Vector h = m_vURVertexT - m_vLRVertexT;
		Vector point1TC = point1 / (h.Length());
		Vector point2TC = point2 / (h.Length());
		Vector point3TC = point3 / (h.Length());
		//Msg("\n");
		if (isClockwise(point1, point2, point3))
		{
			vtriangle.push_back(_point1);
			vtriangle.push_back(_point2);
			vtriangle.push_back(_point3);
			vtriangle.push_back(point1TC);
			vtriangle.push_back(point2TC);
			vtriangle.push_back(point3TC);

			vtriangle2.push_back(__point3);
			vtriangle2.push_back(__point2);
			vtriangle2.push_back(__point1);
			vtriangle2.push_back(point3TC);
			vtriangle2.push_back(point2TC);
			vtriangle2.push_back(point1TC);

		}
		else
		{
			vtriangle.push_back(_point3);
			vtriangle.push_back(_point2);
			vtriangle.push_back(_point1);
			vtriangle.push_back(point3TC);
			vtriangle.push_back(point2TC);
			vtriangle.push_back(point1TC);

			vtriangle2.push_back(__point1);
			vtriangle2.push_back(__point2);
			vtriangle2.push_back(__point3);
			vtriangle2.push_back(point1TC);
			vtriangle2.push_back(point2TC);
			vtriangle2.push_back(point3TC);


		}
		vtriangles.push_back(vtriangle);
		vtriangles.push_back(vtriangle2);
		}

		for (size_t k = 0; k < _newPoly->start_edges.size(); k++)
		{
		Polygon* newPoly = nullptr;
		std::vector< std::vector<Vector2f>> triangles = _newPoly->triangulateMonotone(_newPoly->start_edges[k], newPoly);
		//Msg("HG3\n");
		if (newPoly != nullptr)
		{
		delete newPoly;
		}
		for (size_t j = 0; j < triangles.size(); j++)
		{

		std::vector<Vector> vtriangle;
		std::vector<Vector> vtriangle2;
		Vector point1 = Vector(m_vLRVertexT.x, triangles[j][0].x, triangles[j][0].y);
		Vector point2 = Vector(m_vLRVertexT.x, triangles[j][1].x, triangles[j][1].y);
		Vector point3 = Vector(m_vLRVertexT.x, triangles[j][2].x, triangles[j][2].y);

		Vector _point1;
		Vector _point2;
		Vector _point3;


		VectorTransform(point1, matOrientInv, _point1);
		VectorTransform(point2, matOrientInv, _point2);
		VectorTransform(point3, matOrientInv, _point3);

		Vector __point1 = _point1;
		Vector __point2 = _point2;
		Vector __point3 = _point3;

		__point1 = __point1 + vNormal * 8;
		__point2 = __point2 + vNormal * 8;
		__point3 = __point3 + vNormal * 8;
		Vector h = m_vURVertexT - m_vLRVertexT;
		Vector point1TC = point1 / (h.Length());
		Vector point2TC = point2 / (h.Length());
		Vector point3TC = point3 / (h.Length());
		//Msg("%f, %f, %f \n", _point1.x, _point1.y, _point1.z);
		//Msg("%f, %f, %f \n", _point2.x, _point2.y, _point2.z);
		//Msg("%f, %f, %f \n", _point3.x, _point3.y, _point3.z);
		Msg("Height: %f, %f, %f \n", h.x, h.y, h.z);
		//Msg("\n");
		if (isClockwise(point1, point2, point3))
		{
			vtriangle.push_back(_point1);
			vtriangle.push_back(_point2);
			vtriangle.push_back(_point3);
			vtriangle.push_back(point1TC);
			vtriangle.push_back(point2TC);
			vtriangle.push_back(point3TC);

			vtriangle2.push_back(__point3);
			vtriangle2.push_back(__point2);
			vtriangle2.push_back(__point1);
			vtriangle2.push_back(point3TC);
			vtriangle2.push_back(point2TC);
			vtriangle2.push_back(point1TC);

		}
		else
		{
			vtriangle.push_back(_point3);
			vtriangle.push_back(_point2);
			vtriangle.push_back(_point1);
			vtriangle.push_back(point3TC);
			vtriangle.push_back(point2TC);
			vtriangle.push_back(point1TC);

			vtriangle2.push_back(__point1);
			vtriangle2.push_back(__point2);
			vtriangle2.push_back(__point3);
			vtriangle2.push_back(point1TC);
			vtriangle2.push_back(point2TC);
			vtriangle2.push_back(point3TC);


		}
		vtriangles.push_back(vtriangle);
		vtriangles.push_back(vtriangle2);
		}
		}

		}
		
		lasthitpos = vStartPosT;
		//delete newHole;
		delete _newPoly;
	}

	globalOnce = true;

	
	for (size_t i = 0; i < vtriangles.size(); i++)
	{
		//DebugDrawLine(vtriangles[i][0], vtriangles[i][1], 255, 0, 0, true, 1.0f);
		//DebugDrawLine(vtriangles[i][1], vtriangles[i][2], 255, 0, 0, true, 1.0f);
		//DebugDrawLine(vtriangles[i][2], vtriangles[i][0], 255, 0, 0, true, 1.0f);


		//Msg("%v\n", vtriangles[i][0]);
		//Msg("%v\n", vtriangles[i][1]);
		//Msg("%v\n", vtriangles[i][2]);
		//Msg("\n");
	}

	
	Vector vStartPosT1 = vStartPosT;
	vStartPosT1.z += 10;
	Vector vStartPosT2 = vStartPosT1;
	vStartPosT2.y += 10;
	Vector vStartPosT3 = vStartPosT2;
	vStartPosT3.z -= 10;
	Vector vStartPos1;
	Vector vStartPos2;
	Vector vStartPos3;
	VectorTransform(vStartPosT1, matOrientInv, vStartPos1);
	VectorTransform(vStartPosT2, matOrientInv, vStartPos2);
	VectorTransform(vStartPosT3, matOrientInv, vStartPos3);

	vQuads[0] = vStartPos;
	vQuads[1] = vStartPos1;
	vQuads[2] = vStartPos2;
	vQuads[3] = vStartPos3;

	int idx;	
	idx = pEnts.AddToTail();
	pEnts[idx] = new C_BaseAnimating;
	pEnts[idx]->SetEffects(EF_NODRAW);
	pEnts[idx]->SetAbsOrigin(ptr->endpos);
	
	pEnts[idx]->SetAbsAngles(orient);
	
	pEnts[idx]->SetModel("models/outerhole.mdl");


	idx = pEnts2.AddToTail();
	pEnts2[idx] = new C_BaseAnimating;
	pEnts2[idx]->SetEffects(EF_NODRAW);
	pEnts2[idx]->SetAbsOrigin(ptr->endpos);
	VectorAngles(ptr->plane.normal, orient);
	pEnts2[idx]->SetAbsAngles(orient);
	pEnts2[idx]->SetModel("models/innerhole.mdl");

	idx = pEnts3.AddToTail();
	pEnts3[idx] = new C_BaseAnimating;
	pEnts3[idx]->SetEffects(EF_NODRAW);
	pEnts3[idx]->SetAbsOrigin(ptr->endpos);
	VectorAngles(ptr->plane.normal, orient);
	pEnts3[idx]->SetAbsAngles(orient);
	pEnts3[idx]->SetModel("models/bullethole.mdl");
	//pEnt->Spawn();
	BaseClass::TraceAttack(info, vecDir, ptr, pAccumulator);
}


IMPLEMENT_CLIENTCLASS_DT(C_BreakableHoles, DT_BreakableHoles, CBreakableHoles)
	RecvPropVector(RECVINFO(m_vLLVertex)),
	RecvPropVector(RECVINFO(m_vULVertex)),
	RecvPropVector(RECVINFO(m_vLRVertex)),
	RecvPropVector(RECVINFO(m_vURVertex)),
	RecvPropInt(RECVINFO(m_nQuadError)),
END_RECV_TABLE()