// TRCreator.h: interface for the CTGATR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRCREATOR_H__25590023_1C93_4956_A6DB_B86AB8E264C4__INCLUDED_)
#define AFX_TRCREATOR_H__25590023_1C93_4956_A6DB_B86AB8E264C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define TERRAIN_ERROR_INVALID_PARAM		-5
#define TERRAIN_ERROR_LOADING_IMAGE		-4
#define TERRAIN_ERROR_MEMORY_PROBLEM	-3
#define	TERRAIN_ERROR_NOT_SAVED			-2
#define TERRAIN_ERROR_NOT_INITIALISED	-1
#define TERRAIN_OK						 0


class CTGATR  
{
public:
	CTGATR();
	virtual ~CTGATR();



	int terrainGridWidth ,terrainGridLength;
	float *terrainHeights;
	float *terrainColors;
	float *terrainNormals;
	float terrainStepLength;
	float terrainStepWidth;

	float terrainLightPos[4];
	float terrainDiffuseCol[3];
	float terrainAmbientCol[3];
	int terrainSimLight;

private:

	int terrainLoadFromImage(char *filename, int normals);
	int terrainScale(float min,float max);
	float terrainGetHeight(int x, int z);
	int terrainSimulateLighting(int sim);
	void terrainLightPosition(float x, float y, float z,float w);
	void terrainDiffuseColor(float r, float g, float b);
	void terrainAmbientColor(float r, float g, float b);
	int terrainDim(float stepWidth, float stepLength);
	void terrainComputeNormals();
	void terrainNormalize(float *v);
	float* terrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3);
	void terrainAddVector(float *a, float *b);
	float terrainComputeLightFactor(int i,int j,int offseti, int offsetj);

	bool m_bInit;

public:
	void terrainDestroy();
	void GenerateMeshwithTGA(char* tga_path, int scaleMin=0, int scaleMax=100);
	int terrainCreateDL(float xOffset, float yOffset, float zOffset, int lighting);
	bool IsInit()	{ return m_bInit;	};


};

#endif // !defined(AFX_TRCREATOR_H__25590023_1C93_4956_A6DB_B86AB8E264C4__INCLUDED_)
