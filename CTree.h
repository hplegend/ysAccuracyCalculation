#ifndef CTREE_H
#define CTREE_H

#include "Vector3D.h"
#include "Matrix3D.h"
#include "LineDistence.h"


#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <queue>

using namespace std;

const int LEAF_NUM = 8;

struct CBranch
{
	CVector3D vs, ve; //start and end position of a cylinder
	double ds, de; //diameter of the cylinder
	int level;
	int pid; // parent index
	int cnum; //number of children's index
	int *cid; // children's index
	bool flag ;

	bool Np;
	CVector3D Bn,Tn,Nn;

	CBranch()
	{
	    ds = 0.0;
	    pid = -1;
		cnum = 0;
		level = -1;
		cid = NULL;
	}
	CBranch(const CBranch &branch)
	{
		vs = branch.vs;
		ve = branch.ve;
		ds = branch.ds;
        pid= branch.pid;
		cid = branch.cid;
		cnum = branch.cnum;
		level = branch.level;
	}
	void operator = (const CBranch &branch)
	{
		vs = branch.vs;
		ve = branch.ve;
		ds = branch.ds;
        pid= branch.pid;
		cid = branch.cid;
		cnum = branch.cnum;
		level = branch.level;
	}
	~CBranch()
	{
	    if(cnum!=0 && cid!=NULL)
            delete []cid;
	}
};




class CTree
{
    	vector<CBranch *> m_nodes;
    	vector<CBranch *> m_nodesColor;
    	int m_num;
    	CVector3D m_branchClr;
    	CVector3D m_petioleClr;
    	CVector3D m_leafClr;
        vector <int> m_cnum;
        vector <CVector3D> m_leafVein;
        vector <double> m_leafVeinDia;
        vector <double> m_leafTexcoord;
        vector <CVector3D> m_leafLaminae;
        vector <CVector3D> m_leafNorm;
        vector <int> m_leafTri;
        CMatrix3D m_leafmat[LEAF_NUM];
        GLuint m_nTreeList;
        GLuint m_nLeafList;
        GLuint m_nColorTreeList;
        void DrawPetiole();
        bool LoadLeaf(char *modelFileName);
        void ArrangeWhorledLeaves(CVector3D &vs, CVector3D &ve);
        void DrawLeaf();
        void DrawBranches();
        void DrawGeneralizedCylinder(CBranch &branch);
        void CreateLeafList();
        void CreateTreeList();
        double DepthFirstSearch(CBranch &branch );
        void ReleaseNodes();
        void AdjustBranches();
        int optimization();
        int interSectTionOptimization();
        int binarySearch(int *searchData , int value,int low , int high);
       // bool LoadCopyTree(char *fileName);
         void CreateColorTreeList();
         void DrawColorBranches();



void ColorGeneralizedCylinder(CBranch &branch);


    void RemoveIrregularBranch();
    void ReLoadTree(char * fileName);
    void UpdateLevel();

    void processBranchWithAngle();

     void transFormLeaf(CMatrix3D &mat);
     void transFormVain(CMatrix3D &mat);

 void MarkBranch();

//
double CalculateRadii(CBranch &branch);


    public:
        CTree();
        void Update();
        void ChangeTreeLevel();
        void DrawTree();
       void DrawColorTree();

        bool LoadTree(char *fileName);
        ~CTree();
};

#endif // CLEVELTREE_H
