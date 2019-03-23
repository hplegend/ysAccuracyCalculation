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
}PointST;   //点的结构


typedef struct TreeNodeStructure{
   CVector3D SkeletonNodePos;
   vector<int> InfluenceList;
   bool flag;
   int cnum;
   int Pid;
   vector<int> cidList;
}TreeSkeletonST;  //树的骨架点的结构


typedef struct TreeBranchStructure{
     CVector3D startPoint;
     CVector3D endPoint;
     int BranchId;
     int Pid;
     int cnum;
     vector<int> cid;
     int level;
     bool flag;   //完整与否的标记
     double ds;
}BranchST;  //树枝结构

/*以上就是空间殖民算法用到的一些结构体*/

class SpaceColonization{

private :  //成员变量
    vector<PointST> PointCloud;   //存储加载的点云
    vector<TreeSkeletonST> SkeletonNode; //存储树的骨架节点
    vector<BranchST> TreeBranch;

    int PointNumber;
    int SkeletonNodeNumber;
    int BranchNumber;

    int PointListId;
    int SkeletinListId;
    int PointLineListId;

    int RootPointId;
    CVector3D BaseTreeNode;  //存储树的根骨架节点


    vector<BranchST> OriginalTreeBranch;   //初始的树分支结构
    vector<BranchST> FinalTreeBranch;   //重新索引后的树分支结构


//成员函数
     int findStartPoint();
     void spaceColonizationAlgorithm();
     void BranchStartEndCal();
     void BranchLevelConstructure();
     double CalculateRadii(BranchST &branch);
     void writeBranch(char *fileName);


//绘制函数
    void createPointList();
    void drawPointList();
    void createSkeletonList();
    void drawSkeletonList();

   void createPointLineList();
   void drawPointLineList();

//写文件函数
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
