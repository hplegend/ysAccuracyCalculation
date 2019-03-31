#include "OCtree.h"
#include "Vector3D.h"


extern float centerx, centery, centerz;

ofstream outNumber(".\\model\\number.txt");

ofstream cubicStream(".\\model\\cubic.txt");


extern float cube_left_top_x, cube_left_top_y, cube_left_top_z;
extern float cube_right_buttom_x, cube_right_buttom_y, cube_right_buttom_z;

/*
 * �������ƶ����������������
 * step 1�������Χ�е�����
 * step 2�������Χ�����������������ƫ������
 * step 3��ÿһ������ƫ�������ĺ�
 * step 4���õ�������ϵ
*/
void OCTree::translateTreeCenter() {

    float center_x = cube_right_buttom_x + (cube_left_top_x - cube_right_buttom_x) / 2.0;
    float center_y = cube_right_buttom_y + (cube_left_top_y - cube_right_buttom_y) / 2.0;
    float center_z = cube_right_buttom_z + (cube_left_top_z - cube_right_buttom_z) / 2.0;

    for (int i = 0; i < pointSetSize; ++i) {

        OctreePointSet[i].x -= center_x;
        OctreePointSet[i].y -= center_y;
        OctreePointSet[i].z -= center_z;
    }


    // �ƶ�y���� --- ͳһ��ģ�������ƶ���
    float translate_y = fabs(OctreePointSet[RootPointId].y);
    for (int i = 0; i < pointSetSize; ++i) {
        OctreePointSet[i].y += translate_y;
    }

}


void OCTree::findTreeCenter() {

    float max_x, max_y, max_z;
    float min_x, min_y, min_z;

    if (OctreePointSet.size() == 0) {
        cout << "empty point" << endl;
        exit(1);
    }

    min_x = max_x = OctreePointSet[0].x;
    min_y = max_y = OctreePointSet[0].y;
    min_z = max_z = OctreePointSet[0].z;


    for (int i = 1; i < pointSetSize; ++i) {

        if (OctreePointSet[i].x > max_x) {
            max_x = OctreePointSet[i].x;

        } else if (OctreePointSet[i].x < min_x) {

            min_x = OctreePointSet[i].x;
        }

        if (OctreePointSet[i].y > max_y) {
            max_y = OctreePointSet[i].y;

        } else if (OctreePointSet[i].y < min_y) {

            min_y = OctreePointSet[i].y;
        }

        if (OctreePointSet[i].z > max_z) {
            max_z = OctreePointSet[i].z;

        } else if (OctreePointSet[i].z < min_z) {

            min_z = OctreePointSet[i].z;
        }

    }


    cube_left_top_x = max_x;
    cube_left_top_y = max_y;
    cube_left_top_z = max_z;

    cube_right_buttom_x = min_x;
    cube_right_buttom_y = min_y;
    cube_right_buttom_z = min_z;

}

int OCTree::findStartPoint() {
    int i;
    int minYId; //������Yֵ��Ϊ��С  ��������������Y����
    double minYValue;

    minYId = 0;
    minYValue = 65535.0;

    for (i = 0; i < pointSetSize; i++) {
        if (OctreePointSet[i].y < minYValue) {
            minYValue = OctreePointSet[i].y;
            minYId = i;
        }
    }


    cout << OctreePointSet[minYId].y << endl;
    return minYId;

}


OCTree::OCTree() {

}

OCTree::~OCTree() {

}

// �������OCTree�ĺ�����
void OCTree::OCtreeExecute() {

    // ��ȡ�ռ����
    readPoint();


    RootPointId = findStartPoint();

      findTreeCenter();
      translateTreeCenter();

    // ������ڵ�
    root = (OCNode *) malloc(sizeof(OCNode));
    root->leaf = false;
    root->bounds = calCubicBounds(OctreePointSet, pointSetSize);
    root->number = pointSetSize;

    //��ʼ�Ӹ���ʼ����˲���
    buildOCtree(root);

    // �����˲���
    transerver(root);

    // ģ������
    //  doSearchPoint();

    // ������Ⱦlist
    createCubicList();
    outNumber.close();
    //  cubicStream.close();
}


float OCTree::doSearchPoint(const char *fileName) {
    // read from file
    ifstream inFile(fileName);

    OCPoint curSearchPoint;

    int cnt;
    inFile >> cnt;

    int matchNum = 0;

    int r, g, b;
    for (int i = 0; i < cnt; ++i) {

        cout << "do find" << endl;
        inFile >> curSearchPoint.x >> curSearchPoint.y >> curSearchPoint.z;
        inFile >> r >> g >> b;

        bool searchRet = searchPointInOCtree(curSearchPoint, root);

        if (searchRet) {
            ++matchNum;
        }
    }

    return (matchNum * 1.0) / cnt;
}

void OCTree::doSearchPoint() {

    for (int i = 0; i < OctreePointSet.size(); ++i) {
        // test search
        OCtreePoint *searchPoint = new OCtreePoint;
        searchPoint->x = OctreePointSet[i].x;
        searchPoint->y = OctreePointSet[i].y;
        searchPoint->z = OctreePointSet[i].z;
        searchPointInOCtree(*searchPoint, root);
    }

}

// read point
void OCTree::readPoint() {
    OCPoint point;
    int number;
    int i;
    // ifstream inFile(".\\model\\NormalDensity_07.txt");
    ifstream inFile(".\\data\\groundtruth.pcd");
    // ifstream inFile(".\\data\\0.0030\\0.0030_0.0030.txt");

    OctreePointSet.clear();
    inFile >> number;
    pointSetSize = number;
    long long r, g, b;
    for (i = 0; i < number; i++) {
        inFile >> point.x >> point.y >> point.z;

        // discard
        inFile >> r;
        /*  point.x*=16;
          point.y*=16;
          point.z*=16;*/
        OctreePointSet.push_back(point);
    }

}


//�ݹ�ṹ
void OCTree::buildOCtree(OCNode *OCTreeNode) {
    int i;
    Bounds afterSplit[8];
    int pointNumberArray[8] = {0};
    OCNode *ocnode;

//cout<<"execute"<<endl;

    // �ж��Ƿ�������� || OCTreeNode->number<MINNUMBER || OCTreeNode->number<5
    // leafӦ����ʲô�����أ�

    if (OCTreeNode->bounds.radius < MINBOUND) {
        OCTreeNode->leaf = true;
        //   outNumber<<OCTreeNode->number<<endl;
        /* for(i=0; i<8; i++)
              OCTreeNode->childNode[i]=NULL;;
          OCTreeNode->number=0;*/

        // �����ݴ����
        // ��Ȼû��������
        fileLeafCubic(OCTreeNode->bounds, OCTreeNode);
        return;
    }
    /*
        for(i=0; i<8; i++)
        {
            OCTreeNode->childNode[i]= (OCNode *)malloc(sizeof(OCNode));
        }
    */
    // �����������

    // ����8���ӿռ�ı߽磬��ʱ�룬���Ϻ���
    CalSplitBounds(OCTreeNode->bounds, afterSplit); // ��ȥ����
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


void OCTree::fileLeafCubic(Bound bound, OCNode *leafNode) {

    leafNode->element.clear();

    //
    float leftX, rightX;
    float upY, downY;
    float frontZ, backZ;

    //
    leftX = bound.center.x - bound.radius;
    rightX = bound.center.x + bound.radius;


    downY = bound.center.y - bound.radius;
    upY = bound.center.y + bound.radius;

    backZ = bound.center.z - bound.radius;
    frontZ = bound.center.z + bound.radius;

    for (int i = 0; i < pointSetSize; ++i) {
        //
        if (OctreePointSet[i].isIndex) {
            continue;
        }

        if (OctreePointSet[i].x >= leftX && OctreePointSet[i].x <= rightX
            &&
            OctreePointSet[i].y >= downY && OctreePointSet[i].y <= upY
            &&
            OctreePointSet[i].z >= backZ && OctreePointSet[i].z <= frontZ
                ) {

            leafNode->element.push_back(OctreePointSet[i]);

            OctreePointSet[i].isIndex = true;
        }

    }

      if (leafNode->element.size() > 2000) {
          cout << leafNode->bounds.center.x << "," <<leafNode->bounds.center.y<<"," <<leafNode->bounds.center.z<<
          "error" << endl;
          system("pause");
      }

    cubicStream << "node number��" << leafNode->element.size() << endl;
}

// ��ÿһ�����м��������õ���������ķ��ţ�ͨ���������жϵ�Ĺ���
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

// ������������ʽ�������ĵ��λ�á� error
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
    // ѭ��8��
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


Bounds OCTree::calCubicBounds(vector<OCPoint> point, int count)  // ��������һ�����ָ�ĵ㼯�ϵı߽�
{
    int i;
//  Bounds bound;
    float maxX = 0, maxY = 0, maxZ = 0;
    float temp;
    Bounds bound;

    //�������ҵ��������ҵı߽�
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
    //glTranslatef(root->bounds.center.x,root->bounds.center.y,root->bounds.center.z);   // notice������ۼ�Ч��
    // glutWireCube(root->bounds.radius);
    glTranslatef(bounds.center.x, bounds.center.y, bounds.center.z);
    glutWireCube(bounds.radius);
    glPopMatrix();
}

void OCTree::CallCubicList() {
    if (OCtreeList != 0)
        glCallList(OCtreeList);
}


// �ݹ鷽��
bool OCTree::searchPointInOCtree(OCtreePoint searchPoint, OCTreeNode *branch) {

    // һ��Ҫ��Ҷ�ӽڵ㣬�����ҵ���
    if (branch->leaf) {

        for (int i = 0; i < branch->element.size(); ++i) {
            if (abs(branch->element[i].x - searchPoint.x) <= 0.00001 &&
                abs(branch->element[i].y - searchPoint.y) <= 0.00001 &&
                abs(branch->element[i].z - searchPoint.z) <= 0.00001
                    ) {
                cout << "find: " << branch->element[i].x << "," << branch->element[i].y << ","
                     << branch->element[i].z << endl;
                return true;
            }
        }

        cout << "not find: " << searchPoint.x << "," << searchPoint.y << ","
             << searchPoint.z << endl;
        return false;
    }

    float leftX, rightX;
    float upY, downY;
    float frontZ, backZ;


    // ����Ҷ�ӽڵ㣬��λ���䣬�����ݹ���
    // ���ڽӵ����ģ�ȷ������
    for (int j = 0; j < 8; ++j) {


        Bound bound = branch->childNode[j]->bounds;

        //
        leftX = bound.center.x - bound.radius;
        rightX = bound.center.x + bound.radius;


        downY = bound.center.y - bound.radius;
        upY = bound.center.y + bound.radius;

        backZ = bound.center.z - bound.radius;
        frontZ = bound.center.z + bound.radius;


        if (searchPoint.x >= leftX && searchPoint.x <= rightX
            &&
            searchPoint.y >= downY && searchPoint.y <= upY
            &&
            searchPoint.z >= backZ && searchPoint.z <= frontZ
                ) {

            // �ݹ�
            bool searchRet = searchPointInOCtree(searchPoint, branch->childNode[j]);
            if (searchRet) {
                return searchRet;
            }
        }
    }

    return false;
}
