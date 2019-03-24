#ifndef OCTREE_H_INCLUDED
#define OCTREE_H_INCLUDED
//
#include <windows.h>
#include <GL\glut.h>
#include <iostream>
#include <vector>
#include <fstream>

#define MINBOUND 0.2
#define MINNUMBER 3


using namespace std;

typedef struct OCtreePoint {
    float x, y, z;
    //double n;
    //unsigned int code;
} OCPoint;

typedef struct Bound {
    OCPoint center;
    float radius;  //边长为多少？？

} Bounds;
//时空的权衡
typedef struct OCTreeNode {
    //OCPoint *points;
    OCTreeNode *childNode[8];
    int number;
// int level;
    Bounds bounds;
    bool leaf;

    // 存储区间的点云
    vector<OCTreeNode> element;

    OCTreeNode() {
        leaf = false;
        number = 0;
    }

} OCNode;

class OCTree {
private :
    GLuint OCtreeList;
    vector<OCPoint> OctreePointSet;
    int pointSetSize;
    OCNode *root;
    vector<Bound> boundSet;
    // Bounds bound;

    void createCubicList();

    void drawCubic();

    void Cubic(Bounds bounds);

    void readPoint();

    void countSpaceNumber(int number[], Bounds centerBounds);

    void transerver(OCNode *node);

    int coutingACubicNumber(Bounds centerBounds);

public :
    OCTree();

    ~OCTree();

    Bounds calCubicBounds(vector<OCPoint> point, int count);

    void buildOCtree(OCNode *OCTreeNode);

    void CallCubicList();

    void CalSplitBounds(Bounds bound, Bounds afterSplit[]);

    void OCtreeExecute();

    bool searchPointInOCtree(OCtreePoint searchPoint, OCTreeNode *branch);

};

#endif // OCTREE_H_INCLUDED
