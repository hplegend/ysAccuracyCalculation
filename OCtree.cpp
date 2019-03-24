#include "OCtree.h"
#include "Vector3D.h"


extern float centerx, centery, centerz;

ofstream outNumber(".\\model\\number.txt");

OCTree::OCTree() {

}

OCTree::~OCTree() {

}

// 负责调用OCTree的函数。
void OCTree::OCtreeExecute() {

    // 读取空间点云
    readPoint();

    // 分配根节点
    root = (OCNode *) malloc(sizeof(OCNode));
    root->leaf = false;
    root->bounds = calCubicBounds(OctreePointSet, pointSetSize);
    root->number = pointSetSize;

    //开始从根开始构造八叉树
    buildOCtree(root);

    // 遍历八叉树
    transerver(root);

    // 创建渲染list
    createCubicList();
    outNumber.close();
}

// read point
void OCTree::readPoint() {
    OCPoint point;
    int number;
    int i;
    ifstream inFile(".\\model\\NormalDensity_07.txt");

    OctreePointSet.clear();
    inFile >> number;
    pointSetSize = number;
    for (i = 0; i < number; i++) {
        inFile >> point.x >> point.y >> point.z;
        /*  point.x*=16;
          point.y*=16;
          point.z*=16;*/
        OctreePointSet.push_back(point);
    }

}


//递归结构
void OCTree::buildOCtree(OCNode *OCTreeNode) {
    int i;
    Bounds afterSplit[8];
    int pointNumberArray[8] = {0};
    OCNode *ocnode;

//cout<<"execute"<<endl;

    // 判断是否结束返回 || OCTreeNode->number<MINNUMBER || OCTreeNode->number<5
    // leaf应该有什么条件呢？

    if (OCTreeNode->bounds.radius < MINBOUND) {
        OCTreeNode->leaf = true;
        //   outNumber<<OCTreeNode->number<<endl;
        /* for(i=0; i<8; i++)
              OCTreeNode->childNode[i]=NULL;;
          OCTreeNode->number=0;*/

        // 把数据存进来
        // 不然没法搜索到
        return;
    }
    /*
        for(i=0; i<8; i++)
        {
            OCTreeNode->childNode[i]= (OCNode *)malloc(sizeof(OCNode));
        }
    */
    // 否则继续划分

    // 计算8个子空间的边界，逆时针，先上后下
    CalSplitBounds(OCTreeNode->bounds, afterSplit); // 回去划分
    countSpaceNumber(pointNumberArray, OCTreeNode->bounds);


    for (i = 0; i < 8; i++) {
        ocnode = (OCNode *) malloc(sizeof(OCNode));
        ocnode->bounds = afterSplit[i];
        ocnode->number = pointNumberArray[i];
        ocnode->leaf = false;
        // ocnode->childNode=NULL;
        OCTreeNode->childNode[i] = ocnode;
    }

    for (i = 0; i < 8; i++) {
        buildOCtree(OCTreeNode->childNode[i]);
    }

}

// 把每一点与中间点相减，得到三个坐标的符号，通过符号来判断点的归属
void OCTree::countSpaceNumber(int number[], Bounds centerBounds) {
    float minusX, minusY, minusZ;
    int i;
    for (i = 0; i < pointSetSize; i++) {
        minusX = OctreePointSet[i].x - centerBounds.center.x;
        minusY = OctreePointSet[i].y - centerBounds.center.y;
        minusZ = OctreePointSet[i].z - centerBounds.center.z;

        if ((minusX > centerBounds.radius / 2 || minusX < -centerBounds.radius / 2) ||
            (minusY > centerBounds.radius / 2 || minusY < -centerBounds.radius / 2) ||
            (minusZ > centerBounds.radius / 2 || minusZ < -centerBounds.radius / 2))
            continue;
        // else 01
        if ((minusX > 0 && minusX < centerBounds.radius / 2) && (minusY > 0 && minusY < centerBounds.radius / 2) &&
            (minusZ < 0 && minusZ < centerBounds.radius / 2))
            number[0]++;
        // 02
        if ((minusX < 0 && minusX < centerBounds.radius / 2) && (minusY > 0 && minusY < centerBounds.radius / 2) &&
            (minusZ < 0 && minusZ < centerBounds.radius / 2))
            number[1]++;
        //03
        if ((minusX < 0 && minusX < centerBounds.radius / 2) && (minusY > 0 && minusY < centerBounds.radius / 2) &&
            (minusZ > 0 && minusZ < centerBounds.radius / 2))
            number[2]++;
        //04
        if ((minusX > 0 && minusX < centerBounds.radius / 2) && (minusY > 0 && minusY < centerBounds.radius / 2) &&
            (minusZ > 0 && minusZ < centerBounds.radius / 2))
            number[3]++;
        //05
        if ((minusX > 0 && minusX < centerBounds.radius / 2) && (minusY < 0 && minusY < centerBounds.radius / 2) &&
            (minusZ < 0 && minusZ < centerBounds.radius / 2))
            number[4]++;
        //06
        if ((minusX < 0 && minusX < centerBounds.radius / 2) && (minusY < 0 && minusY < centerBounds.radius / 2) &&
            (minusZ < 0 && minusZ < centerBounds.radius / 2))
            number[5]++;
        //07
        if ((minusX < 0 && minusX < centerBounds.radius / 2) && (minusY < 0 && minusY < centerBounds.radius / 2) &&
            (minusZ > 0 && minusZ < centerBounds.radius / 2))
            number[6]++;
        //08
        if ((minusX > 0 && minusX < centerBounds.radius / 2) && (minusY < 0 && minusY < centerBounds.radius / 2) &&
            (minusZ > 0 && minusZ < centerBounds.radius / 2))
            number[7]++;
    }

}

// 采用向量的形式计算中心点的位置。 error
void OCTree::CalSplitBounds(Bounds bound, Bounds afterSplit[]) {
    int i;
    OCPoint p1, p2, p3;
    CVector3D v1, v2, v3;
    CVector3D sum1, sum2;
    CVector3D center;
    center.x = bound.center.x;
    center.y = bound.center.y;
    center.z = bound.center.z;

    int flagArray[8][3] =
            {
                    {1, 0, 1},
                    {0, 0, 1},
                    {0, 1, 1},
                    {1, 1, 1},
                    {1, 0, 0},
                    {0, 0, 0},
                    {0, 1, 0},
                    {1, 1, 0}
            };
    // 循环8次
    for (i = 0; i < 8; i++) {
        if (flagArray[i][0] == 1) {
            p1.x = bound.center.x + bound.radius / 2;
            p1.y = bound.center.y;
            p1.z = bound.center.z;
        } else {
            p1.x = bound.center.x - bound.radius / 2;
            p1.y = bound.center.y;
            p1.z = bound.center.z;
        }
        if (flagArray[i][1] == 1) {
            p2.x = p1.x;
            p2.y = p1.y;
            p2.z = p1.z + bound.radius / 2;
        } else {
            p2.x = p1.x;
            p2.y = p1.y;
            p2.z = p1.z - bound.radius / 2;
        }
        if (flagArray[i][2] == 1) {
            p3.x = p2.x;
            p3.y = p2.y + bound.radius / 2;
            p3.z = p2.z;
        } else {
            p3.x = p2.x;
            p3.y = p2.y - bound.radius / 2;
            p3.z = p2.z;
        }

        // v1
        v1.x = p1.x - bound.center.x;
        v1.y = p1.y - bound.center.y;
        v1.z = p1.z - bound.center.z;

        // v2
        v2.x = p2.x - p1.x;
        v2.y = p2.y - p1.y;
        v2.z = p2.z - p1.z;

        // v3
        v3.x = p3.x - p2.x;
        v3.y = p3.y - p2.y;
        v3.z = p3.z - p2.z;

// minus vector
        sum1 = v1 + v2;
        sum2 = sum1 + v3;
        sum2 = sum2 / 2;

        sum1 = center + sum2;

        afterSplit[i].center.x = sum1.x;
        afterSplit[i].center.y = sum1.y;
        afterSplit[i].center.z = sum1.z;

        afterSplit[i].radius = bound.radius / 2;

    }

}


Bounds OCTree::calCubicBounds(vector<OCPoint> point, int count)  // 计算所有一个待分割的点集合的边界
{
    int i;
//  Bounds bound;
    float maxX = 0, maxY = 0, maxZ = 0;
    float temp;
    Bounds bound;

    //遍历，找到上下左右的边界
    float pointLeaft, pointRight, pointUp, pointDown, pointFront, pointBack;

    pointLeaft = pointRight = point[0].x;
    pointUp = pointDown = point[0].y;
    pointFront = pointBack = point[0].z;

    for (i = 1; i < count; i++) {
        if (point[i].x >= pointRight)
            pointRight = point[i].x;
        else if (point[i].x <= pointLeaft)
            pointLeaft = point[i].x;

    }
    for (i = 1; i < count; i++) {
        if (point[i].y >= pointUp)
            pointUp = point[i].y;
        else if (point[i].y <= pointDown)
            pointDown = point[i].y;

    }
    for (i = 1; i < count; i++) {
        if (point[i].z >= pointFront)
            pointFront = point[i].z;
        else if (point[i].z <= pointBack)
            pointBack = point[i].z;

    }

    cout << pointBack << " " << pointFront << endl;
    cout << pointUp << " " << pointDown << endl;
    cout << pointLeaft << " " << pointRight << endl;

    maxX = pointRight - pointLeaft;
    maxY = pointUp - pointDown;
    maxZ = pointFront - pointBack;

    if (maxX < maxY) {
        temp = maxY;
        maxY = maxX;
        maxX = temp;

    } else if (maxZ > maxX) {
        temp = maxX;
        maxX = maxZ;
        maxZ = temp;

    } else if (maxZ > maxY) {
        temp = maxY;
        maxY = maxZ;
        maxZ = temp;
    }

    cout << maxX << " " << maxY << " " << maxZ << endl;

    bound.radius = maxX;

    bound.center.x = pointLeaft + (pointRight - pointLeaft) / 2;
    bound.center.y = pointDown + (pointUp - pointDown) / 2;
    bound.center.z = pointBack + (pointFront - pointBack) / 2;

    centerx = bound.center.x;
    centery = bound.center.y;
    centerz = bound.center.z;

    return bound;

}

void OCTree::transerver(OCNode *node) {

    if (node->leaf == true) {
        //  cout<<"exectue"<<endl;
        // node->number= coutingACubicNumber(node->bounds);
        if (node->number == 0);
        else {

            cout << node->bounds.radius << " " << node->bounds.center.x << " " << node->bounds.center.y << " "
                 << node->bounds.center.z << endl;

            outNumber << node->number << endl;

            boundSet.push_back(node->bounds);
        }

        return;
    }
    for (int i = 0; i < 8; i++) {
        // boundSet.push_back(node->childNode[i]->bounds);
        transerver(node->childNode[i]);
    }
}


int OCTree::coutingACubicNumber(Bounds centerBounds) {
    int i = 0;
    int count = 0;
    float minusx, minusy, minusz;
    for (i = 0; i < pointSetSize; i++) {
        minusx = centerBounds.center.x - OctreePointSet[i].x;
        minusy = centerBounds.center.y - OctreePointSet[i].y;
        minusz = centerBounds.center.z - OctreePointSet[i].z;

        if ((minusx < centerBounds.radius / 2 && minusx > -centerBounds.radius / 2) &&
            (minusy < centerBounds.radius / 2 && minusy > -centerBounds.radius / 2) &&
            (minusz < centerBounds.radius / 2 && minusz > -centerBounds.radius / 2))
            count++;


    }


    return count;

}

void OCTree::createCubicList() {

    if (OCtreeList != 0) {
        glDeleteLists(OCtreeList, 1);
        OCtreeList = 0;
    }
    OCtreeList = glGenLists(1);
    glNewList(OCtreeList, GL_COMPILE);
    drawCubic();
    glEndList();
}


void OCTree::drawCubic() {
    int i;
    ofstream outfile(".\\model\\center.txt");
    cout << "Size" << boundSet.size() << endl;
    // glMatrixMode(GL_MODELVIEW);
    //  glLoadIdentity();
    for (i = 0; i < boundSet.size(); i++) {
        Cubic(boundSet[i]);
        outfile << boundSet[i].center.x << " " << boundSet[i].center.y << " " << boundSet[i].center.z << " "
                << boundSet[i].radius << endl;
    }

    cout << "finish" << endl;
    outfile.close();
    //  glFlush();

}

void OCTree::Cubic(Bounds bounds) {
    glColor3f(0, 0, 0);
    glPushMatrix();
    /* glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();*/
    /*glBegin(GL_POINTS);
    glVertex3f(bounds.center.x,bounds.center.y,bounds.center.z);
    glEnd();*/
    //glTranslatef(root->bounds.center.x,root->bounds.center.y,root->bounds.center.z);   // notice这里的累加效果
    // glutWireCube(root->bounds.radius);
    glTranslatef(bounds.center.x, bounds.center.y, bounds.center.z);
    glutWireCube(bounds.radius);
    glPopMatrix();
}

void OCTree::CallCubicList() {
    if (OCtreeList != 0)
        glCallList(OCtreeList);
}


// 递归方法
bool OCTree::searchPointInOCtree(OCtreePoint searchPoint, OCTreeNode *branch) {

    // 一定要到叶子节点，才能找到点
    if (branch->leaf) {
        // 开始从四个方向找




    }

    // 不是叶子节点，定位区间，继续递归找
}
