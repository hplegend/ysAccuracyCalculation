#include "SpaceColonizationHeader.h"
#include "OCtree.h"

extern int PointNum;
extern double InfluenceValue;
extern double DeleteValue;
extern double NodeSpanValue;

extern char g_strFileName[];
extern int g_currLevel;
extern int g_maxLevel;
extern bool g_bShowLeaves;
extern double g_thickness;

extern unsigned int get_msec(void);

extern double precision;

extern CTree myTree;
extern OCTree octree;

extern float cube_left_top_x ,cube_left_top_y,cube_left_top_z;
extern float cube_right_buttom_x ,cube_right_buttom_y,cube_right_buttom_z;
/*
 * 把物体移动到世界坐标的中心
 * step 1：计算包围盒的中心
 * step 2：计算包围盒中心与世界坐标的偏移向量
 * step 3：每一个点与偏移向量的和
 * step 4：得到新坐标系
*/
void SpaceColonization::translateTreeCenter() {

    float center_x = cube_right_buttom_x + (cube_left_top_x - cube_right_buttom_x) / 2.0;
    float center_y = cube_right_buttom_y + (cube_left_top_y - cube_right_buttom_y) / 2.0;
    float center_z = cube_right_buttom_z + (cube_left_top_z - cube_right_buttom_z) / 2.0;

    for (int i = 0; i < PointNumber; ++i) {

        PointCloud[i].PointPos.x -= center_x;
        PointCloud[i].PointPos.y -= center_y;
        PointCloud[i].PointPos.z -= center_z;
    }


    // 移动y坐标 --- 统一将模型向上移动了
    float translate_y = fabs(PointCloud[RootPointId].PointPos.y);
    for (int i = 0; i < PointNumber; ++i) {
        PointCloud[i].PointPos.y += translate_y;
    }

}


void SpaceColonization::findTreeCenter() {

    float max_x, max_y, max_z;
    float min_x, min_y, min_z;

    if (PointCloud.size() == 0) {
        cout << "empty point" << endl;
        exit(1);
    }

    min_x = max_x = PointCloud[0].PointPos.x;
    min_y = max_y = PointCloud[0].PointPos.y;
    min_z = max_z = PointCloud[0].PointPos.z;


    for (int i = 1; i < PointNumber; ++i) {

        if (PointCloud[i].PointPos.x > max_x) {
            max_x = PointCloud[i].PointPos.x;

        } else if (PointCloud[i].PointPos.x < min_x) {

            min_x = PointCloud[i].PointPos.x;
        }

        if (PointCloud[i].PointPos.y > max_y) {
            max_y = PointCloud[i].PointPos.y;

        } else if (PointCloud[i].PointPos.y < min_y) {

            min_y = PointCloud[i].PointPos.y;
        }

        if (PointCloud[i].PointPos.z > max_z) {
            max_z = PointCloud[i].PointPos.z;

        } else if (PointCloud[i].PointPos.z < min_z) {

            min_z = PointCloud[i].PointPos.z;
        }

    }


    cube_left_top_x = max_x;
    cube_left_top_y = max_y;
    cube_left_top_z = max_z;

    cube_right_buttom_x = min_x;
    cube_right_buttom_y = min_y;
    cube_right_buttom_z = min_z;

}



//OCPoint * pointSet;

SpaceColonization::SpaceColonization() {

    PointListId = 0;
    SkeletinListId = 0;
}

SpaceColonization::~SpaceColonization() {
    PointCloud.clear();
    SkeletonNode.clear();
}

void SpaceColonization::LoadPointCloud() {
    //   ifstream inFile(".\\model\\leaf3out2.txt");
    //ifstream inFile(".\\model\\PointsGe.txt");
    ifstream inFile(".\\data\\0.0030\\0.0030_0.0030.txt");
    // ifstream inFile(".\\model\\tranlate.txt");
    //   ifstream inFile(".\\model\\parts.txt");
    //  ifstream inFile(".\\model\\processOut01.txt");
    int i;

    PointST FilePoint;


    if (inFile == NULL) {
        cout << "can't open file!" << endl;
        exit(1);
    }

    inFile >> PointNumber;   //读取点的数量
    PointNum = PointNumber;

    // pointSet = (OCPoint*)malloc(sizeof(OCPoint)*PointNumber);


    int r, g, b;

    for (i = 0; i < PointNumber; i++) //循环读取点的数据
    {
        inFile >> FilePoint.PointPos.x >> FilePoint.PointPos.y >> FilePoint.PointPos.z;
        FilePoint.flag = true;
        FilePoint.PointId = i;   //读取顺序就是ID

        inFile >> r >> g >> b;

        /*  FilePoint.PointPos.x*=16;
          FilePoint.PointPos.y*=16;
          FilePoint.PointPos.z*=16;*/
/*
 pointSet[i].x=FilePoint.PointPos.x;
 pointSet[i].y=FilePoint.PointPos.y;
 pointSet[i].z=FilePoint.PointPos.z;

*/
        PointCloud.push_back(FilePoint);

    }

    cout << "pointNumber" << PointNumber << endl;

    RootPointId = findStartPoint();

    findTreeCenter(); //2016-10-29
    translateTreeCenter(); //2016-10-29

    RootPointId = findStartPoint();

    createPointList();
    octree.OCtreeExecute();
    // spaceColonizationAlgorithm();
}


int SpaceColonization::findStartPoint() {
    int i;
    int minYId; //假设以Y值作为最小  树的生长方向是Y方向
    double minYValue;

    minYId = 0;
    minYValue = 65535.0;

    for (i = 0; i < PointNumber; i++) {
        if (PointCloud[i].PointPos.y < minYValue) {
            minYValue = PointCloud[i].PointPos.y;
            minYId = i;
        }
    }


    cout << PointCloud[minYId].PointPos.y << endl;
    return minYId;

}

void SpaceColonization::spaceColonizationAlgorithm() {
    TreeSkeletonST skeleton;
    vector<int> NotEmpty;
    vector<double> N_Pdistance;
    double countPrecision = 0.0;
    double sumPrecision = 0.0;
    int countNumber = 0;
    FILE *PrecisionFile = fopen(".\\model\\Precision.txt", "w");

    int i, j, k;
    bool equalFlag;

    int minValueId;
    double minDistance;

    int iteratorNumber = 800, iteratorCount = 0;

    CVector3D minusVector;
    double minusDistance;


    double startTime, endTime;

    CVector3D Growthdirection;
    double GrowthAngle = 40.0;

    if (N_Pdistance.size() > 0)
        N_Pdistance.clear();


    //假设，第一个骨架点就是树的根节点RootPoint;
    skeleton.flag = true;
    skeleton.InfluenceList.clear();
    skeleton.SkeletonNodePos = PointCloud[RootPointId].PointPos;
    skeleton.cnum = 0;
    skeleton.cidList.clear();
    skeleton.Pid = -1;

    PointCloud[RootPointId].flag = false;  //这里很重要

    SkeletonNode.push_back(skeleton); //the first skeleton node in
    NotEmpty.push_back(0);

    startTime = get_msec();

    while (iteratorCount <= iteratorNumber)//|| !NotEmpty.empty()iteratorCount<=iteratorNumber
    {
        for (j = 0; j < (int) NotEmpty.size(); j++)
            SkeletonNode[NotEmpty[j]].InfluenceList.clear();

        //1.找到每一个影响点非空的骨架点的S(V)集合
        for (i = 0; i < PointNumber; i++) {
            if (PointCloud[i].flag == false)
                continue;

            minDistance = 65535.0;
            for (j = 0; j < (int) NotEmpty.size(); j++) {
                if (PointCloud[i].PointPos.x - SkeletonNode[NotEmpty[j]].SkeletonNodePos.x > InfluenceValue ||
                    PointCloud[i].PointPos.y - SkeletonNode[NotEmpty[j]].SkeletonNodePos.y > InfluenceValue ||
                    PointCloud[i].PointPos.z - SkeletonNode[NotEmpty[j]].SkeletonNodePos.z > InfluenceValue
                        )
                    continue;

                minusVector = PointCloud[i].PointPos - SkeletonNode[NotEmpty[j]].SkeletonNodePos;
                minusDistance = minusVector.Length();
                /* 考虑一个方向问题*/
                //如果是第一个骨架点
                /*   if(SkeletonNode[NotEmpty[j]].Pid==-1)
                      {
                             Growthdirection.Set(0.0,1.0,0.0); // Y 方向生长
                             GrowthAngle=40.0;
                      }
                  else
                  {
                      Growthdirection = SkeletonNode[NotEmpty[j]].SkeletonNodePos - SkeletonNode[SkeletonNode[NotEmpty[j]].Pid].SkeletonNodePos;
                      GrowthAngle = Growthdirection.Angle(minusVector); // 弧度
                      GrowthAngle = GrowthAngle/3.14 * 180; // 角度
                  }
     */
                if (minusDistance <= InfluenceValue && minusDistance <= minDistance && GrowthAngle <= 60) {
                    minValueId = j;
                    minDistance = minusDistance;

                }
            }//end of
            if (minDistance < 65534.0)
                SkeletonNode[NotEmpty[minValueId]].InfluenceList.push_back(i);
        }//end of first step

        int InSize;
        CVector3D summryVector;
        CVector3D TempVector_01, TempVector_02;
        vector<int> NewAddSkeletoonNode;
        NewAddSkeletoonNode.clear();
        //2.计算和方向向量，放置新的骨架点
        for (i = 0; i < (int) NotEmpty.size(); i++) {
            InSize = SkeletonNode[NotEmpty[i]].InfluenceList.size();
            if (InSize == 0)
                continue;

            TempVector_01 = PointCloud[SkeletonNode[NotEmpty[i]].InfluenceList[0]].PointPos -
                            SkeletonNode[NotEmpty[i]].SkeletonNodePos;
            for (j = 1; j < InSize; j++) {
                TempVector_02 = PointCloud[SkeletonNode[NotEmpty[i]].InfluenceList[j]].PointPos -
                                SkeletonNode[NotEmpty[i]].SkeletonNodePos;
                TempVector_01 = TempVector_01 + TempVector_02;
            }

            summryVector = TempVector_01;  //求得了和向量
            summryVector = summryVector.Normalize(); //将自身标准化
            summryVector = summryVector * NodeSpanValue;
            summryVector = SkeletonNode[NotEmpty[i]].SkeletonNodePos + summryVector;

            //避免添加重复，因为近视对称的原因  ---important
            equalFlag = false; //表示要插入
            if (SkeletonNode[NotEmpty[i]].cnum > 0) {
                for (k = 0; k < SkeletonNode[NotEmpty[i]].cnum; k++) {
                    if (summryVector.IsEqual(SkeletonNode[SkeletonNode[NotEmpty[i]].cidList[k]].SkeletonNodePos)) {
                        equalFlag = true;
                        break;
                    }
                }

            }
            if (equalFlag == false) {
                skeleton.flag = true;
                skeleton.InfluenceList.clear();
                skeleton.SkeletonNodePos = summryVector;
                skeleton.cnum = 0;
                skeleton.cidList.clear();
                skeleton.Pid = NotEmpty[i];

                SkeletonNode[NotEmpty[i]].cnum++;
                SkeletonNode[NotEmpty[i]].cidList.push_back(SkeletonNode.size());

                NewAddSkeletoonNode.push_back(SkeletonNode.size());

                SkeletonNode.push_back(skeleton);
            }
        }//end of second step

        double DeleteDistance = 0.0;

        //3. 删除节点, 仅仅对新添加的节点进行测试

        for (i = 0; i < (int) NewAddSkeletoonNode.size(); i++)    //看一个骨架点能影响几个树节点
        {
            N_Pdistance.clear();
            precision = 0.0;
            for (j = 0; j < PointNumber; j++) {
                if (PointCloud[j].flag == false)
                    continue;
                TempVector_01 = PointCloud[j].PointPos - SkeletonNode[NewAddSkeletoonNode[i]].SkeletonNodePos;
                DeleteDistance = TempVector_01.Length();

                if (DeleteDistance <= DeleteValue) {
                    PointCloud[j].flag = false;
                    precision += DeleteDistance;
                    N_Pdistance.push_back(DeleteDistance);
                }

            }
            //
            if (N_Pdistance.size() != 0) {
                precision = precision / N_Pdistance.size();
                DeleteDistance = 0.0;
                countPrecision = 0.0;

                for (j = 0; j < N_Pdistance.size(); j++) {
                    DeleteDistance = (precision - N_Pdistance[j]) * (precision - N_Pdistance[j]);
                    countPrecision += DeleteDistance;
                }
                sumPrecision += countPrecision;
                sumPrecision += countPrecision / N_Pdistance.size();
                countNumber += N_Pdistance.size();
                fprintf(PrecisionFile, "%f\n", sumPrecision);
            }
            countNumber += N_Pdistance.size();
        }//end of third step

        vector<int> TempVector_03;
        TempVector_03.clear();
        //4. 下一次循环需要做的工作


        for (i = 0; i < (int) NotEmpty.size(); i++) {
            InSize = SkeletonNode[NotEmpty[i]].InfluenceList.size();
            if (InSize != 0)
                TempVector_03.push_back(NotEmpty[i]); //ID  in
        }

        for (i = 0; i < (int) NewAddSkeletoonNode.size(); i++)
            TempVector_03.push_back(NewAddSkeletoonNode[i]); //ID  in

        // NewAddSkeletoonNode.clear();
        NotEmpty.clear();

        for (i = 0; i < (int) TempVector_03.size(); i++)
            NotEmpty.push_back(TempVector_03[i]);

        //TempVector_03.clear();

        iteratorCount++;
        // cout<<iteratorCount<<endl;
    }//end spaceColonization

    cout << "Precision:" << sumPrecision / countNumber << endl;

    fclose(PrecisionFile);
    endTime = get_msec() - startTime;

    SkeletonNodeNumber = SkeletonNode.size();
    writeSkeleton();
    createSkeletonList();
    BranchStartEndCal();
    BranchLevelConstructure();
    CalculateRadii(TreeBranch[0]);
    writeBranch(g_strFileName);
    createPointLineList();
    myTree.LoadTree(g_strFileName);

    cout << "consuming Time" << endTime << "s" << endl;

}

//采用广度优先的顺序，依次遍历，同时建立树的层次结构
void SpaceColonization::BranchStartEndCal() {
    int i;
    BranchST NewBranch;
    int IdNext, CIdNext;
    int id;
    queue<int> BranchingId;

    IdNext = 0;
    NewBranch.BranchId = 0;
    NewBranch.cid.clear();
    NewBranch.cnum = 0;
    NewBranch.flag = true;
    NewBranch.Pid = -1;
    NewBranch.level = 1;
    NewBranch.startPoint = SkeletonNode[IdNext].SkeletonNodePos;

    id = 1;

    while (true) {
        if (SkeletonNode[IdNext].cnum == 1)
            IdNext = SkeletonNode[IdNext].cidList[0];
        else if (SkeletonNode[IdNext].cnum == 0) {
            NewBranch.endPoint = SkeletonNode[IdNext].SkeletonNodePos;
            TreeBranch.push_back(NewBranch);
            break;
        } else if (SkeletonNode[IdNext].cnum > 1) {
            NewBranch.endPoint = SkeletonNode[IdNext].SkeletonNodePos;
            TreeBranch.push_back(NewBranch);
            BranchingId.push(IdNext);
            break;
        }
    } //目的是找到第一根树枝
    while (!BranchingId.empty()) {
        IdNext = BranchingId.front();
        BranchingId.pop();

        for (i = 0; i < SkeletonNode[IdNext].cnum; i++) {
            NewBranch.BranchId = id;
            id++;

            NewBranch.startPoint = SkeletonNode[IdNext].SkeletonNodePos;
            NewBranch.cid.clear();
            NewBranch.cnum = 0;
            NewBranch.flag = true;
            NewBranch.level = 0;
            NewBranch.Pid = -2;

            CIdNext = SkeletonNode[IdNext].cidList[i];

            if (SkeletonNode[CIdNext].cnum == 0) {
                NewBranch.endPoint = SkeletonNode[CIdNext].SkeletonNodePos;
                TreeBranch.push_back(NewBranch);
                continue;
            } else if (SkeletonNode[CIdNext].cnum == 1)  //对于正常的树枝来说
            {
                while (SkeletonNode[CIdNext].cnum == 1) {
                    CIdNext = SkeletonNode[CIdNext].cidList[0];
                }
                if (SkeletonNode[CIdNext].cnum == 0) {
                    NewBranch.endPoint = SkeletonNode[CIdNext].SkeletonNodePos;
                    TreeBranch.push_back(NewBranch);
                    continue;
                } else if (SkeletonNode[CIdNext].cnum > 1) {
                    NewBranch.endPoint = SkeletonNode[CIdNext].SkeletonNodePos;
                    TreeBranch.push_back(NewBranch);
                    BranchingId.push(CIdNext);
                    continue;
                }
            } else if (SkeletonNode[CIdNext].cnum > 1) {
                NewBranch.endPoint = SkeletonNode[CIdNext].SkeletonNodePos;
                TreeBranch.push_back(NewBranch);
                BranchingId.push(CIdNext);
            }

        }

    }

    BranchNumber = TreeBranch.size();
    cout << BranchNumber << " " << TreeBranch[0].level << endl;

}

//严格按照层次关系来建立
void SpaceColonization::BranchLevelConstructure() {
    queue<int> levelQueue;
    int IdCurrent;
    int i = 0;

    levelQueue.push(0);
    TreeBranch[0].flag = false;

    while (!levelQueue.empty()) {
        IdCurrent = levelQueue.front();
        levelQueue.pop();

        for (i = 0; i < BranchNumber; i++) {
            if (TreeBranch[i].flag == false)
                continue;
            if (TreeBranch[IdCurrent].endPoint.IsEqual(TreeBranch[i].startPoint)) {
                TreeBranch[IdCurrent].cnum++;
                TreeBranch[IdCurrent].cid.push_back(i);

                TreeBranch[i].Pid = IdCurrent;
                TreeBranch[i].level = TreeBranch[IdCurrent].level + 1;

                TreeBranch[i].flag = false;

                levelQueue.push(i);
            }
        }
    }

    cout << TreeBranch[0].level << endl;

}


double SpaceColonization::CalculateRadii(BranchST &branch) {
    int i;
    double value;
    double sum = 0.0;

    if (branch.cnum == 0)
        branch.ds = g_thickness;
    else {
        for (i = 0; i < branch.cnum; i++) {
            value = CalculateRadii(TreeBranch[branch.cid[i]]);
            sum += value * value;
        }

        branch.ds = sqrt(sum);
    }

    return branch.ds;

}


void SpaceColonization::writeBranch(char *fileName) {
    int i, j;
    ofstream outFile(fileName);
    if (outFile == NULL) {
        cout << "can't open file!" << endl;
        exit(1);
    }
    outFile << ".\\model\\cherry_leaf.txt" << endl;  //leaf
    outFile << 1 << endl;  //scale
    outFile << 0.8 << " " << 0.8 << " " << 0.1 << endl;

    outFile << BranchNumber << endl;

    for (i = 0; i < BranchNumber; i++) {

        outFile << TreeBranch[i].BranchId << " " << TreeBranch[i].level << " ";
        outFile << TreeBranch[i].cnum << " ";
        for (j = 0; j < TreeBranch[i].cnum; j++)
            outFile << TreeBranch[i].cid[j] << " ";

        outFile << TreeBranch[i].startPoint.x << " " << TreeBranch[i].startPoint.y << " " << TreeBranch[i].startPoint.z
                << " ";
        outFile << TreeBranch[i].endPoint.x << " " << TreeBranch[i].endPoint.y << " " << TreeBranch[i].endPoint.z
                << " ";
        outFile << TreeBranch[i].ds << endl;
    }

    outFile.close();

}


void SpaceColonization::writeSkeleton() {
    int i = 0;
    ofstream outFile(".\\model\\skeleton.txt");

    if (outFile == NULL) {
        cout << "can't open file for writing!" << endl;
        exit(1);
    }

    for (i = 0; i < SkeletonNodeNumber; i++) {
        outFile << SkeletonNode[i].SkeletonNodePos.x << " " << SkeletonNode[i].SkeletonNodePos.y << " "
                << SkeletonNode[i].SkeletonNodePos.z << " " << SkeletonNode[i].Pid << " " << SkeletonNode[i].cnum
                << endl;

    }
    outFile.close();


}


void SpaceColonization::createPointList() {
    if (PointListId != 0) {
        glDeleteLists(PointListId, 1);
        PointListId = 0;
    }

    PointListId = glGenLists(1);
    glNewList(PointListId, GL_COMPILE);
    drawPointList();
    glEndList();

}


void SpaceColonization::drawPointList() {
    int i;
    glDisable(GL_LIGHTING);

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(4.0);

    glBegin(GL_POINTS);
    glVertex3f(PointCloud[RootPointId].PointPos.x, PointCloud[RootPointId].PointPos.y,
               PointCloud[RootPointId].PointPos.z);
    glEnd();

    glPointSize(4.0);
    // glColor3f(0.24,0.49,0.95);
    glColor3f(1.0, 0.0, 0.0);
    //glColor3f(0.7,0.5,0.5);
    glBegin(GL_POINTS);
    for (i = 0; i < PointNumber; i++)
        glVertex3f(PointCloud[i].PointPos.x, PointCloud[i].PointPos.y, PointCloud[i].PointPos.z);
    glEnd();
    glFlush();
}

void SpaceColonization::drawPoint() {
    if (PointListId != 0)
        glCallList(PointListId);
}


void SpaceColonization::createSkeletonList() {
    if (SkeletinListId != 0) {
        glDeleteLists(SkeletinListId, 1);
        SkeletinListId = 0;
    }
    SkeletinListId = glGenLists(1);
    glNewList(SkeletinListId, GL_COMPILE);
    drawSkeletonList();
    glEndList();

}

void SpaceColonization::drawSkeletonList() {
    int i = 0;
    glDisable(GL_LIGHTING);
    //glColor3f(1.0,0.0,0.0);
    glColor3f(0.24, 0.49, 0.95);
    glPointSize(3.0);  // 2.5

    glBegin(GL_POINTS);
    for (i = 0; i < SkeletonNodeNumber; i++)
        glVertex3f(SkeletonNode[i].SkeletonNodePos.x, SkeletonNode[i].SkeletonNodePos.y,
                   SkeletonNode[i].SkeletonNodePos.z);
    glEnd();

}


void SpaceColonization::drawSkeleton() {
    if (SkeletinListId != 0)
        glCallList(SkeletinListId);
}

void SpaceColonization::createPointLineList() {
    if (PointLineListId != 0) {
        glDeleteLists(PointLineListId, 1);
        PointLineListId = 0;
    }
    PointLineListId = glGenLists(1);
    glNewList(PointLineListId, GL_COMPILE);
    drawPointLineList();
    glEndList();

}

void SpaceColonization::drawPointLineList() {
    int i = 0, j = 0;
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 0.0, 0.0);
    // glColor3f(0.24,0.49,0.95);
    glLineWidth(1.0);  // 2.5

    glBegin(GL_LINES);
    for (i = 0; i < TreeBranch.size(); i++) {
        glVertex3f(TreeBranch[i].startPoint.x, TreeBranch[i].startPoint.y, TreeBranch[i].startPoint.z);
        glVertex3f(TreeBranch[i].endPoint.x, TreeBranch[i].endPoint.y, TreeBranch[i].endPoint.z);
    }
    glEnd();
    glColor3f(0.24, 0.49, 0.95);
    glPointSize(6.0);

    glBegin(GL_POINTS);
    for (i = 0; i < TreeBranch.size(); i++) {
        glVertex3f(TreeBranch[i].startPoint.x, TreeBranch[i].startPoint.y, TreeBranch[i].startPoint.z);
        glVertex3f(TreeBranch[i].endPoint.x, TreeBranch[i].endPoint.y, TreeBranch[i].endPoint.z);
    }
    glEnd();


}


void SpaceColonization::drawPointLine() {
    if (PointLineListId != 0)
        glCallList(PointLineListId);
}


void SpaceColonization::drawProcess() {
    int i = 0;
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 0.0, 0.0);

    ofstream outFile(".\\model\\parts.txt");
    int countNum = 0;

    glBegin(GL_POINTS);
    for (i = 0; i < SkeletonNodeNumber; i++)
        glVertex3f(SkeletonNode[i].SkeletonNodePos.x, SkeletonNode[i].SkeletonNodePos.y,
                   SkeletonNode[i].SkeletonNodePos.z);
    glEnd();

    glColor3f(0.45, 0.55, 0.95);
    glBegin(GL_POINTS);
    for (i = 0; i < PointNumber; i++)
        if (PointCloud[i].flag == true) {
            countNum++;
            glVertex3f(PointCloud[i].PointPos.x, PointCloud[i].PointPos.y, PointCloud[i].PointPos.z);
        }
    glEnd();

    outFile << PointNumber - countNum << endl;
    for (i = 0; i < PointNumber; i++)
        if (PointCloud[i].flag == false)
            outFile << PointCloud[i].PointPos.x << " " << PointCloud[i].PointPos.y << " " << PointCloud[i].PointPos.z
                    << endl;


}
