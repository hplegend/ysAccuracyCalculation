#ifndef SPACECOLONIZATIONHEADER_H_INCLUDED
#define SPACECOLONIZATIONHEADER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <fstream>
#include <ctime>
#include <vector>
#include <queue>


#include "Matrix3D.h"
#include "Vector3D.h"
#include "CTree.h"



using namespace std;


typedef struct PointStructure{
  int PointId;
  CVector3D  PointPos;
  bool flag;
}PointST;   //��Ľṹ


typedef struct TreeNodeStructure{
   CVector3D SkeletonNodePos;
   vector<int> InfluenceList;
   bool flag;
   int cnum;
   int Pid;
   vector<int> cidList;
}TreeSkeletonST;  //���ĹǼܵ�Ľṹ


typedef struct TreeBranchStructure{
     CVector3D startPoint;
     CVector3D endPoint;
     int BranchId;
     int Pid;
     int cnum;
     vector<int> cid;
     int level;
     bool flag;   //�������ı��
     double ds;
}BranchST;  //��֦�ṹ

/*���Ͼ��ǿռ�ֳ���㷨�õ���һЩ�ṹ��*/

class SpaceColonization{

private :  //��Ա����
    vector<PointST> PointCloud;   //�洢���صĵ���
    vector<TreeSkeletonST> SkeletonNode; //�洢���ĹǼܽڵ�
    vector<BranchST> TreeBranch;

    int PointNumber;
    int SkeletonNodeNumber;
    int BranchNumber;

    int PointListId;
    int SkeletinListId;
    int PointLineListId;

    int RootPointId;
    CVector3D BaseTreeNode;  //�洢���ĸ��Ǽܽڵ�


    vector<BranchST> OriginalTreeBranch;   //��ʼ������֧�ṹ
    vector<BranchST> FinalTreeBranch;   //���������������֧�ṹ


//��Ա����
     int findStartPoint();
     void spaceColonizationAlgorithm();
     void BranchStartEndCal();
     void BranchLevelConstructure();
     double CalculateRadii(BranchST &branch);
     void writeBranch(char *fileName);


//���ƺ���
    void createPointList();
    void drawPointList();
    void createSkeletonList();
    void drawSkeletonList();

   void createPointLineList();
   void drawPointLineList();

//д�ļ�����
void writeSkeleton();


public:

SpaceColonization();
~SpaceColonization();

void LoadPointCloud();
void drawPoint();
void drawSkeleton();
void drawPointLine();

void drawProcess();


};


#endif // SPACECOLONIZATIONHEADER_H_INCLUDED
