#include "CTree.h"

extern int g_currLevel;
extern int g_maxLevel;
extern bool g_bShowLeaves;
extern double g_thickness;
extern float NodeSpanValue;    //0.1-0.3

extern char g_optimizationFile[];
extern FILE *fobj;
extern int lastIndex;


/*��Ҫ���Ƶĵط����������ļ� �� ������뾶 �� ������֦��*/

#define DELETESPAN 0.2

/*ͬʱ�����������Ľṹ*/

CTree::CTree()
{
    srand(time(NULL));
    m_nTreeList = 0;
    m_nLeafList = 0;
    m_nColorTreeList=0;
}

bool CTree::LoadTree(char *fileName)
{
    int i, j, id, maxLevel;
    double scale;
    char leafFileName[512];
    CBranch *NewBranch;
    CBranch *NewBranchColor;
    queue<int> outTree;

    int oBackValue=0;
    int iBackValue=0;
    int pos=0;
    int count=0;

    int *SearchArray;
    int newPos;


    ifstream inFile(fileName);
    ofstream outFile(g_optimizationFile);

    if(inFile==NULL) return false ;

    inFile>>leafFileName;
    cout<<leafFileName<<endl;

    LoadLeaf(leafFileName);

    inFile>>scale;
    inFile>>m_branchClr.x>>m_branchClr.y>>m_branchClr.z;
    inFile>>m_num;//��֧��Ŀ

    cout<<"Number of branches:"<<m_num<<endl;
    // m_nodes = new CBranch[m_num];
    //  m_nodes.resize(m_num);

    //����push_front����ʽ����Ԫ�أ�
    //Ԥ����ռ䣬�������Ĳ���

    for(i=0; i<m_num; i++)
    {
        NewBranch = new CBranch();
        m_nodes.push_back(NewBranch);

        NewBranchColor = new CBranch();
        m_nodesColor.push_back(NewBranchColor);
    }

    for(i=0; i<m_num; i++)
    {
        inFile>>id>>m_nodes[i]->level; //

        m_nodesColor[i]->level = m_nodes[i]->level;
        if(i==0)
        {
            m_nodes[i]->pid=-1;
            m_nodesColor[i]->pid=-1;
            maxLevel=m_nodes[i]->level;

        }
        else
        {
            if(m_nodes[i]->level > maxLevel)  //��������ڸ��µ�ʱ������Ķ�ȡ����
                maxLevel = m_nodes[i]->level;
        }

        inFile>>m_nodes[i]->cnum;
        m_nodesColor[i]->cnum = m_nodes[i]->cnum;
        m_cnum.push_back(m_nodes[i]->cnum);
        m_nodes[i]->cid = new int [m_nodes[i]->cnum];
        m_nodesColor[i]->cid = new int [m_nodesColor[i]->cnum] ;
        for(j=0; j<m_nodes[i]->cnum; j++)
        {
            inFile >>m_nodes[i]->cid[j];
            m_nodesColor[i]->cid[j] =  m_nodes[i]->cid[j];
            m_nodes[m_nodes[i]->cid[j]]->pid=i;
            m_nodesColor[m_nodesColor[i]->cid[j]]->pid = i;
        }
        inFile>>m_nodes[i]->vs.x>>m_nodes[i]->vs.y>>m_nodes[i]->vs.z;
        inFile>>m_nodes[i]->ve.x>>m_nodes[i]->ve.y>>m_nodes[i]->ve.z;
        inFile>>m_nodes[i]->ds;

        m_nodesColor[i]->vs = m_nodes[i]->vs;
        m_nodesColor[i]->ve = m_nodes[i]->ve;
        m_nodesColor[i]->ds =  m_nodes[i]->ds;   //

        m_nodes[i]->vs = m_nodes[i]->vs*scale;
        m_nodes[i]->ve = m_nodes[i]->ve*scale;
        m_nodes[i]->ds = m_nodes[i]->ds*scale;

        m_nodesColor[i]->vs = m_nodes[i]->vs*scale;
        m_nodesColor[i]->ve = m_nodes[i]->ve*scale;
        m_nodesColor[i]->ds = m_nodes[i]->ds*scale;

        if(m_nodes[i]->cnum==0)
            g_thickness = m_nodes[i]->ds;

//����δ��ɾ����flag
        m_nodes[i]->flag=true;

        m_nodesColor[i]->flag= true;

    }   //finish read
////////////////////////////////////�������׺��ӹ�ϵ��
    inFile.close();

    g_maxLevel = maxLevel;    //maintain a maxLevel of tree;
    cout<<"number of Levels: "<<g_maxLevel<<endl;

    g_currLevel = g_maxLevel;

    AdjustBranches();  //calculate radii

//����һ������ -----���ǰ�m_nodes �е����ݿ�����m_nodesColor��

    for(i=0; i<m_num; i++)
    {
        m_nodesColor[i]->de = m_nodes[i]->de;

    }
/////
    inFile.close();

//���������Ѿ�ȫ�������ˣ�������Ǵ����б��ˣ��������Ľṹ

    RemoveIrregularBranch();
     MarkBranch();
   // processBranchWithAngle();
  // oBackValue= optimization();
   // iBackValue =interSectTionOptimization();    //�Ż��Ĵ���Ҫ�ڴ��������б�ǰ�汻ִ��
 //

    //  CreateTreeList();

    // CreateColorTreeList();


    /*�Ѿ���������ļ�����д�뵽�ļ���  --- ���õķ����ǣ�������������������ȷ��λ�ã����д�뵽�ļ���*/

    for(i=0; i<m_num; i++)
        if(m_nodes[i]->flag == false)
        {
            count++;
        }
    cout<<"begin outFile"<<endl;

    outFile<<leafFileName<<endl;
    outFile<<scale<<endl;
    outFile<<m_branchClr.x<<" "<<m_branchClr.y<<" "<<m_branchClr.z<<endl;
    outFile<<m_num-count<<endl;

    SearchArray=new int [m_num-count];   //������ڴ������ܴ�������

    count=0;
    for(i=0; i<m_num; i++)
        if(m_nodes[i]->flag == true)
        {
            SearchArray[count] = i;
            count++;
        }

//һ�������һ��������һ��д���ļ�
    for(i=0; i<m_num; i++)
    {
        if(m_nodes[i]->flag==true)
        {
            for(j=0; j<m_nodes[i]->cnum; j++)
            {

                newPos = binarySearch(SearchArray,m_nodes[i]->cid[j],0,count-1);
                if(newPos==-2)
                {
                    cout<<"error1"<<endl;
                    exit(1);

                }
                m_nodes[i]->cid[j]=newPos;
            }
            //д�ļ�   -- pos���µ�λ��
            outFile<<pos<<" "<<m_nodes[i]->level<<" ";
            if(m_nodes[i]->cnum!= m_cnum[i])
            {
                cout<<"error2"<<endl;
                exit(0);
            }
            outFile<<m_nodes[i]->cnum<<" ";
            for(j=0; j<m_nodes[i]->cnum; j++)
            {
                outFile<<m_nodes[i]->cid[j]<<" ";
            }

            outFile<<m_nodes[i]->vs.x<<" "<<m_nodes[i]->vs.y<<" "<<m_nodes[i]->vs.z<<" ";
            outFile<<m_nodes[i]->ve.x<<" "<<m_nodes[i]->ve.y<<" "<<m_nodes[i]->ve.z<<" ";

            outFile<<m_nodes[i]->ds<<endl;
            pos++;
        }

    }//end of for

 // g_thickness=0.02;
    ReLoadTree(g_optimizationFile);   //���¼���һ����
      // CalculateRadii(*m_nodes[0]);
    //   AdjustBranches();

    CreateLeafList();   //����Ҷ�ӵ�ʱ�򣬸��������ݽṹһ���ϵ��û��   Ҷ�ӵ��б�Ӧ����ǰ��  ��ʵ��loadLeaf��Ϳ���
   CreateColorTreeList();
    CreateTreeList();

    return true ;
}

//�ݹ�Ķ�������

int CTree::binarySearch(int *searchData , int value,int low , int high)
{
    int middle;
    if(low>high)
        return -2;
    middle = (low+high)/2;
    if(value==searchData[middle])
        return middle;
    if(value<searchData[middle])   // former
    {
        binarySearch(searchData,value,low, middle-1);
    }
    else    //later
    {
        binarySearch(searchData,value,middle+1, high);
    }
}

//�����Ǹ�������
void CTree::ReLoadTree(char * fileName)
{
    int i, j, id, maxLevel;
    double scale;
    char leafFileName[512];
    CBranch *NewBranch;

    ifstream inFile(fileName);
    if(inFile==NULL) return  ;

    inFile>>leafFileName;
    cout<<leafFileName<<endl;

    inFile>>scale;
    inFile>>m_branchClr.x>>m_branchClr.y>>m_branchClr.z;
    inFile>>m_num;//��֧��Ŀ

    m_cnum.clear();

    for(i=0; i<m_num; i++)
    {
        NewBranch = new CBranch();
        m_nodes.push_back(NewBranch);
    }
    for(i=0; i<m_num; i++)
    {
        inFile>>id>>m_nodes[i]->level; //

        if(i==0)
        {
            m_nodes[i]->pid=-1;
            maxLevel=m_nodes[i]->level;
        }
        else
        {
            if(m_nodes[i]->level > maxLevel)  //��������ڸ��µ�ʱ������Ķ�ȡ����
                maxLevel = m_nodes[i]->level;
        }
        inFile>>m_nodes[i]->cnum;

        m_cnum.push_back(m_nodes[i]->cnum);
        m_nodes[i]->cid = new int [m_nodes[i]->cnum];

        for(j=0; j<m_nodes[i]->cnum; j++)
        {
            inFile >>m_nodes[i]->cid[j];
            m_nodes[m_nodes[i]->cid[j]]->pid=i;
        }
        inFile>>m_nodes[i]->vs.x>>m_nodes[i]->vs.y>>m_nodes[i]->vs.z;
        inFile>>m_nodes[i]->ve.x>>m_nodes[i]->ve.y>>m_nodes[i]->ve.z;
        inFile>>m_nodes[i]->ds;

        m_nodes[i]->vs = m_nodes[i]->vs*scale;
        m_nodes[i]->ve = m_nodes[i]->ve*scale;
        m_nodes[i]->ds = m_nodes[i]->ds*scale;

        if(m_nodes[i]->cnum==0)
            g_thickness = m_nodes[i]->ds;
//����δ��ɾ����flag  , ����ӵ���֦��δ��ɾ���ġ�
        m_nodes[i]->flag=true;

    }   //finish read
////////////////////////////////////�������׺��ӹ�ϵ��
    g_maxLevel = maxLevel;    //maintain a maxLevel of tree;
    g_currLevel = g_maxLevel;

    inFile.close();

  AdjustBranches();
}

bool CTree::LoadLeaf(char *modelFileName)
{
    ifstream inFile;
    int i, idx;
    int nVeinVNum, nVerticesNum, nTrianglesNum;
    double dia;
    CVector3D ver, v1, v2, v3;
    double texX, texY;
    int a, b, c;
    double scale;

    inFile.open(modelFileName);

    if(!inFile) return false;

    inFile >> m_petioleClr.x >> m_petioleClr.y >> m_petioleClr.z;
    inFile >> m_leafClr.x >> m_leafClr.y >> m_leafClr.z;
    inFile >> scale;
    inFile >> nVeinVNum >> nVerticesNum >> nTrianglesNum;
    m_leafVein.resize(nVeinVNum);
    m_leafVeinDia.resize(nVeinVNum);
    m_leafLaminae.resize(nVerticesNum);
    m_leafTexcoord.resize(2*nVerticesNum);
    m_leafTri.resize(3*nTrianglesNum);
    m_leafNorm.resize(nTrianglesNum);
    for(i=0; i<nVeinVNum; i++)
    {
        inFile >> idx >> ver.x >> ver.y >> ver.z >> dia;
        m_leafVein[i] = ver*scale;
        m_leafVeinDia[i] = dia*scale;
    }
    for(i=0; i<nVerticesNum; i++)
    {
        inFile >> idx >> ver.x >> ver.y >> ver.z >>texX >> texY;
        m_leafLaminae[i] = ver*scale;
        m_leafTexcoord[2*i] = texX;    //����洢����
        m_leafTexcoord[2*i+1] = texY;
    }
    for(i=0; i<nTrianglesNum; i++)
    {
        inFile >> idx >> a >> b >> c;
        m_leafTri[3*i] = a;
        m_leafTri[3*i+1] = b;
        m_leafTri[3*i+2] = c;
        v1 = m_leafLaminae[b] - m_leafLaminae[a];
        v2 = m_leafLaminae[c] - m_leafLaminae[a];
        v3 = v1.Outer(v2);   //Outer�����ڻ������Ǽ��㷽������
        m_leafNorm[i] = v3.Normal();
        if(m_leafNorm[i].y<0)
            m_leafNorm[i] = m_leafNorm[i]*(-1);
    }
    inFile.close();
    return true;
}

//����ÿһ����֦��ĩ�˰뾶 de
void CTree::AdjustBranches()
{
    int i , j, k;
    double maxDia = 0.0 ;
    for(i=0; i<m_num; i++)
    {
        maxDia=0.0;
        if(m_nodes[i]->flag == false) continue;
        for(j=0; j<m_nodes[i]->cnum; j++)
        {
            k= m_nodes[i]->cid[j];
            if(m_nodes[k]->ds>maxDia)
                maxDia = m_nodes[k]->ds;
        }

        if(m_nodes[i]->cnum==0)
            m_nodes[i]->de=m_nodes[i]->ds;
        else
            m_nodes[i]->de=maxDia;   //��ʵ��adjustBranch������Ŀ�ľ���Ϊ�����ÿһ����֧����ֹ�뾶����Ϊ��ʼ�뾶ds �Ǵ��ļ��ж�ȡ��
    }
}


void CTree::CreateLeafList()
{
    cout<<"---createleaflist\n";
    if(m_nLeafList!=0)
    {
        glDeleteLists(m_nLeafList,1);
        m_nLeafList=0;
    }
    m_nLeafList = glGenLists(1);
    glNewList(m_nLeafList,GL_COMPILE);   //�����б���Ҫ������λ���Ҷ��
    DrawLeaf();
    glEndList();
}

void CTree::DrawLeaf()
{
    cout<<"---drawleaf\n";
    CVector3D v1 , v2 , v3;
    int i , tnum , a, b,c;

    int * leafIndexSet;

    tnum  = m_leafTri.size()/3;


    glEnable(GL_LIGHTING);
    glColor3f(m_petioleClr.x, m_petioleClr.y, m_petioleClr.z);
   //glColor3f( 0.5, 0.5,0.5);

    DrawPetiole();
    glColor3f(m_leafClr.x, m_leafClr.y, m_leafClr.z);
    glBegin(GL_TRIANGLES);
    for (i=0; i<tnum; i++)
    {
        a = m_leafTri[3*i];
        b = m_leafTri[3*i+1];
        c = m_leafTri[3*i+2];
        glNormal3f(m_leafNorm[i].x, m_leafNorm[i].y, m_leafNorm[i].z);   //��ķ���
        glTexCoord2d(m_leafTexcoord[2*a],m_leafTexcoord[2*a+1]);
        glVertex3d(m_leafLaminae[a].x, m_leafLaminae[a].y, m_leafLaminae[a].z);

        glTexCoord2d(m_leafTexcoord[2*b], m_leafTexcoord[2*b+1]);
        glVertex3d(m_leafLaminae[b].x, m_leafLaminae[b].y, m_leafLaminae[b].z);

        glTexCoord2d(m_leafTexcoord[2*c], m_leafTexcoord[2*c+1]);
        glVertex3d(m_leafLaminae[c].x, m_leafLaminae[c].y, m_leafLaminae[c].z);

    }
    glEnd();

}

void CTree::DrawPetiole()
{
    //cout<<"----draw petiole\n";
    const int SLICE_NUM=3;
    int i,j,num;
    CVector3D vDirStart , vDirEnd , InitM , N ,B ,vTmp;
    CVector3D posStart[SLICE_NUM];
    CVector3D posEnd[SLICE_NUM];
    CVector3D normStart[SLICE_NUM];
    CVector3D normEnd[SLICE_NUM];


    num = m_leafVein.size();

    for(i=0; i<(num-1); i++)
    {
        if(i==0)
            vDirStart=m_leafVein[i+1]-m_leafVein[i];
        else
            vDirStart = vDirEnd;
        if(i==(num-2))
            vDirEnd=m_leafVein[i+1] - m_leafVein[i];
        else
            vDirEnd = m_leafVein[i+2] - m_leafVein[i+1];
        if(i==0)
        {
            if(vDirStart.x*vDirStart.y*vDirStart.z!=0)
            {
                double d=sqrt(vDirStart.y*vDirStart.y+vDirStart.z*vDirStart.z);
                InitM.Set(0,vDirStart.z/d,-vDirStart.y/d);
            }
            else
            {
                if(vDirStart.x==0) InitM.Set(1,0,0);
                else if(vDirStart.y==0)	InitM.Set(0,1,0);
                else	InitM.Set(0,0,1);
            }
            N = InitM.Outer(vDirStart);
            N.Normal();
            B=vDirStart.Outer(N);
            B.Normal();
            for(j=0; j<SLICE_NUM; j++)
            {
                normStart[j] = N*cos(2*M_PI*j/(double)SLICE_NUM);
                vTmp=B*sin(2*M_PI*j/(double)SLICE_NUM);
                normStart[j] = normStart[j] + vTmp;
                posStart[j] = normStart[j]*0.5*m_leafVeinDia[i] + m_leafVein[i];
                posStart[j] = posStart[j];
            }

        }
        else
        {
            for(j=0; j<SLICE_NUM; j++)
            {
                normStart[j]= normEnd[j];
                posStart[j] = posEnd[j];
            }
        }
        N = B.Outer(vDirEnd);
        N.Normalize();
        B = vDirEnd.Outer(N);
        B.Normalize();
        for(j=0; j<SLICE_NUM; j++)
        {
            normEnd[j]= N*cos(2* M_PI*j/(double)SLICE_NUM);
            vTmp = B*sin(2* M_PI*j/(double)SLICE_NUM);
            normEnd[j] = normEnd[j] + vTmp;
            posEnd[j] = normEnd[j]*0.5*m_leafVeinDia[i+1] + m_leafVein[i+1];
            posEnd[j] = posEnd[j];
        }

        glBegin(GL_QUAD_STRIP);
        for(j=0; j<SLICE_NUM; j++)
        {
            glNormal3d(normEnd[j].x,normEnd[j].y,normEnd[j].z);
            glVertex3d(posEnd[j].x,posEnd[j].y,posEnd[j].z);

            glNormal3d(normStart[j].x,normStart[j].y,normStart[j].z);
            glVertex3d(posStart[j].x,posStart[j].y,posStart[j].z);

        }
        glNormal3d(normEnd[0].x,normEnd[0].y,normEnd[0].z);
        glVertex3d(posEnd[0].x,posEnd[0].y,posEnd[0].z);
        glNormal3d(normStart[0].x,normStart[0].y,normStart[0].z);
        glVertex3d(posStart[0].x,posStart[0].y,posStart[0].z);
        glEnd();

}
}

void CTree::CreateColorTreeList()
{
    int pos , i;
    queue<int> q;

    if(m_nColorTreeList!=0)
    {
        glDeleteLists(m_nColorTreeList,1);
        m_nColorTreeList=0;
    }
    m_nColorTreeList = glGenLists(1);
    glNewList(m_nColorTreeList,GL_COMPILE);
    DrawColorBranches();
    if(g_bShowLeaves)
    {
        q.push(0);
        while(!q.empty())
        {
            pos=q.front();  //ȡ����Ԫ��
            q.pop();  //ɾ��Ԫ��

            ArrangeWhorledLeaves(m_nodesColor[pos]->vs, m_nodesColor[pos]->ve);
            for(i=0; i<LEAF_NUM; i++)
            {
                glPushMatrix();
                glMultMatrixd(m_leafmat[i].glMatrix());
                glCallList(m_nLeafList);  //һ����֦���ǻ���һ����Ҷ�ļ����ļ�һ����5ƬҶ��,���Ƶ�ʱ������λ�����ƬҶ��
                glPopMatrix();
            }

            for(i=0; i<m_nodesColor[pos]->cnum; i++)   //�����Ϊfalse�Ķ��Ѿ�������ȫͼ���ˣ�����ͨ�����������ܱ��������Ϊfalse����֦
                // if(m_nodes[m_nodes[pos]->cid[i]]->flag==true)   //֦û�б�ɾ�����Ŵ�������Ͳ����ˡ�
                q.push(m_nodesColor[pos]->cid[i]);
        }
    }
    glEndList();
}

void CTree::DrawColorBranches()
{
    int i , pos;
    queue<int> q ;
    queue<int> deleteBranch;
    int number=0;

    glEnable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glColor4f(m_branchClr.x, m_branchClr.y, m_branchClr.z,1.0f);

 m_nodesColor[0]->Np=false;

    q.push(0);
    while (!q.empty())
    {
        pos = q.front();
        q.pop();
        if(m_nodesColor[pos]->flag==true)
        {
              glColor4f(m_branchClr.x, m_branchClr.y, m_branchClr.z,1.0f);
              ColorGeneralizedCylinder(*m_nodesColor[pos]);    // Q 1 color branch Np all is false
              number++;
        }
        else
        {
               glColor4f(1.0f,0.0f,0.0f,1.0f);
               ColorGeneralizedCylinder(*m_nodesColor[pos]);
        }


        for(i=0; i<m_nodesColor[pos]->cnum; i++)    //ֻҪ���ƺ��������Ϳ������ɾ�����ӽڵ�Ĺ���������Ϊ��һ����ͨͼ��ͨ��һ����������㷨���ܱ������
            q.push(m_nodesColor[pos]->cid[i]);         //ͨ����������ά�ֺ��ӹ�ϵ�ģ�������������ͨ�������й�����ȱ����㷨

    }
    //�ֿ����λ���  ������Ҫɾ���ıߣ��ú�ɫ����
/*
    glColor4f(1.0f,0.0f,0.0f,1.0f);
    while (!deleteBranch.empty())
    {
        pos = deleteBranch.front();
        deleteBranch.pop();
        DrawGeneralizedCylinder(*m_nodesColor[pos]);
        number++;
    }
*/
 cout<<"color branch Draw over :"<<number<<endl;

}



void CTree::ColorGeneralizedCylinder(CBranch &branch)
{
  const int SLICE_NUM = 12 ;
    int j;
    double d;
    CVector3D vDirStart , vDirEnd , InitM , N , B ,vTmp;   //�����Ҷ���Ĵ��뼸����ͬ
    CVector3D posStart[SLICE_NUM];
    CVector3D posEnd[SLICE_NUM];
    CVector3D normStart[SLICE_NUM];
    CVector3D normEnd[SLICE_NUM];

    double maxRadii=0.0;
    int maxPos;

if(branch.Np==false)    // note that vstart and vend is tangent
{
    vDirStart = branch.vs - branch.ve;    //��֧�ķ�������
    vDirEnd = vDirStart;


     if (vDirStart.x*vDirStart.y*vDirStart.z!=0)   // ����x��y��z�غ�
    {
        d=sqrt(vDirStart.y*vDirStart.y+vDirStart.z*vDirStart.z);
        InitM.Set(0,vDirStart.z/d,-vDirStart.y/d);
    }
    else
    {
        if(vDirStart.x==0)
            InitM.Set(1,0,0);
        else if(vDirStart.y==0)
            InitM.Set(0,1,0);
        else
            InitM.Set(0,0,1);
    }


    N = InitM.Outer(vDirStart);   //�����
    N.Normalize();
    B = vDirStart.Outer(N);    //Ϊʲô����ֱ�أ�
    B.Normalize();



}
else
{
    vDirStart = m_nodesColor[branch.pid]->Tn;
    vDirEnd = branch.vs - branch.ve;
   // InitM = m_nodes[branch.pid]->Bn;
     N = m_nodesColor[branch.pid]->Nn;
     B = m_nodesColor[branch.pid]->Bn;
}



    //  cout<<B.x<<" "<<B.y<<" "<<B.z<<endl;


    for(j=0; j<SLICE_NUM; j++)
    {
        normStart[j]= N*cos(2*M_PI*j/(double)SLICE_NUM);
        vTmp = B*sin(2*M_PI*j/(double)SLICE_NUM);
        normStart[j] = normStart[j] + vTmp;
        posStart[j] = normStart[j]*0.5*branch.ds + branch.vs;    // ֻ�����İ뾶������й�ϵ
    }
    N = B.Outer(vDirEnd);
    N.Normalize();
    B = vDirEnd.Outer(N);
    B.Normalize();
    for(j=0; j<SLICE_NUM; j++)
    {
        normEnd[j] = N*cos(2*M_PI*j/(double)SLICE_NUM);
        vTmp = B*sin(2*M_PI*j/(double)SLICE_NUM);
        normEnd[j] = normEnd[j] + vTmp;
        posEnd[j] = normEnd[j]*0.5*branch.de + branch.ve;
    }
    glBegin(GL_QUAD_STRIP);
    for(j=0; j<SLICE_NUM; j++) //���ѭ��д�ĺܺ�
    {
        glNormal3f(normEnd[j].x,normEnd[j].y,normEnd[j].z);  // ����ȥʱ���ƺ���Ҫ���ÿһ����ķ���
        glTexCoord2d(j/(double)SLICE_NUM, 1.0);
        glVertex3f(posEnd[j].x, posEnd[j].y, posEnd[j].z);
        glNormal3f(normStart[j].x,normStart[j].y,normStart[j].z);
        glTexCoord2d(j/(double)SLICE_NUM, 0.0);
        glVertex3f(posStart[j].x, posStart[j].y, posStart[j].z);
    }
    glNormal3f(normEnd[0].x,normEnd[0].y,normEnd[0].z);
    glTexCoord2d(1.0, 1.0);
    glVertex3f(posEnd[0].x, posEnd[0].y, posEnd[0].z);
    glNormal3f(normStart[0].x,normStart[0].y,normStart[0].z);
    glTexCoord2d(1.0, 0.0);
    glVertex3f(posStart[0].x, posStart[0].y, posStart[0].z);
    glEnd();

//ȷ����һ����dominant
for(j=0;j<branch.cnum;j++)
{
  if(m_nodesColor[branch.cid[j]]->ds > maxRadii)
  {
    maxRadii = m_nodesColor[branch.cid[j]]->ds;
    maxPos = branch.cid[j];
  }

}

for(j=0;j<branch.cnum ;j++)    //
{
    if(branch.cid[j]==maxPos)
      m_nodesColor[branch.cid[j]]->Np = true;
    else
      m_nodesColor[branch.cid[j]]->Np = false;
}

    //����De�Ĳο����, ���ڴ��ݵ������á�

    branch.Bn = B;
    branch.Tn = vDirEnd;
    branch.Nn = N;

}

void CTree::CreateTreeList()
{
    cout<<"----createtreelist\n";
    int pos , i;
    queue<int> q;
    CMatrix3D mat;
    int j=0;
    double w;
    int tnum =  m_leafTri.size()/3;
    int a,b,c;
    CVector3D temp01,temp02;

    if(m_nTreeList!=0)
    {
        glDeleteLists(m_nTreeList,1);
        m_nTreeList=0;
    }
    m_nTreeList = glGenLists(1);
    glNewList(m_nTreeList,GL_COMPILE);
    DrawBranches();
    if(g_bShowLeaves)
    {
        q.push(0);
        while(!q.empty())
        {
            pos=q.front();  //ȡ����Ԫ��
            q.pop();  //ɾ��Ԫ��

            if(m_nodes[pos]->cnum==0)
            {
                ArrangeWhorledLeaves(m_nodes[pos]->vs, m_nodes[pos]->ve);   //����matrix  , һ���Լ���8ƬҶ�ӵľ���
                for(i=0; i<LEAF_NUM; i++)   //һ����֦��Ƭ��Ҷ
                {
                    glPushMatrix();
                    mat = m_leafmat[i].glMatrix();
                    glMultMatrixd(m_leafmat[i].glMatrix());    // Matrix3D�У����ݰ����д��棬���Ǿ���Ҫ�����ݰ����д洢   CMatrix3D ��
                    glCallList(m_nLeafList);
                    glPopMatrix();
                }
            }

            for(i=0; i<m_nodes[pos]->cnum; i++)   //�����Ϊfalse�Ķ��Ѿ�������ȫͼ���ˣ�����ͨ�����������ܱ��������Ϊfalse����֦
                // if(m_nodes[m_nodes[pos]->cid[i]]->flag==true)   //֦û�б�ɾ�����Ŵ�������Ͳ����ˡ�
                q.push(m_nodes[pos]->cid[i]);
        }
    }
    glEndList();
}

void CTree::DrawBranches()
{
    cout<<"----drawbranch\n";
    int i , pos;
    queue<int> q ;
    queue<int> deleteBranch;
    int count = 0;
    int endId;
     glEnable(GL_LIGHTING);


  // glEnable(GL_BLEND);
  // glDisable(GL_DEPTH_TEST); // �ر���Ȳ���
  //  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //GL_ONE_MINUS_SRC_ALPHA
   glColor4f(m_branchClr.x, m_branchClr.y, m_branchClr.z,1.0f);
  //glColor4f(0.5, 0.5,0.5,1.0f);
   //  glColor4f(1.0f,1.0f, 1.0f,0.6f);
    q.push(0);   //����֦��Զ������Ŀ�ͷ��һ��Ҳ�ǰ���˳�����ġ�
    while (!q.empty())
    {
        pos = q.front();
        q.pop();

        DrawGeneralizedCylinder(*m_nodes[pos]);

        count++;
        for(i=0; i<m_nodes[pos]->cnum; i++)    //ֻҪ���ƺ��������Ϳ������ɾ�����ӽڵ�Ĺ���������Ϊ��һ����ͨͼ��ͨ��һ����������㷨���ܱ������
            q.push(m_nodes[pos]->cid[i]);         //ͨ����������ά�ֺ��ӹ�ϵ�ģ�������������ͨ�������й�����ȱ����㷨
    }

    cout<<m_num<<"::"<<count<<"::"<<m_nodes.size();
    //  outFile.close();
    cout<<"finish drawBranch"<<endl;
//glDisable(GL_BLEND);
//glEnable(GL_DEPTH_TEST);
}

void CTree::DrawGeneralizedCylinder(CBranch &branch)
{
    const int SLICE_NUM = 12 ;
    int j;
    double d;
    CVector3D vDirStart , vDirEnd , InitM , N , B ,vTmp;   //�����Ҷ���Ĵ��뼸����ͬ
    CVector3D posStart[SLICE_NUM];
    CVector3D posEnd[SLICE_NUM];
    CVector3D normStart[SLICE_NUM];
    CVector3D normEnd[SLICE_NUM];

    double maxRadii=0.0;
    int maxPos;

    //// index part
    int startIndex[12];
    int endIndex[12];
    int i,k;
    int firstIndex;
   // int *backEndIndex;
    //  CVector3D textureStart[SLICE_NUM] , textureEnd[SLICE_NUM];


    if(branch.Np==false)    // note that vstart and vend is tangent
    {
        vDirStart = branch.vs - branch.ve;    //��֧�ķ�������
        vDirEnd = vDirStart;


        if (vDirStart.x*vDirStart.y*vDirStart.z!=0)   // ����x��y��z�غ�
        {
            d=sqrt(vDirStart.y*vDirStart.y+vDirStart.z*vDirStart.z);
            InitM.Set(0,vDirStart.z/d,-vDirStart.y/d);
        }
        else
        {
            if(vDirStart.x==0)
                InitM.Set(1,0,0);
            else if(vDirStart.y==0)
                InitM.Set(0,1,0);
            else
                InitM.Set(0,0,1);
        }

        N = InitM.Outer(vDirStart);   //�����
        N.Normalize();
        B = vDirStart.Outer(N);    //Ϊʲô����ֱ�أ�
        B.Normalize();

    }
    else
    {
        vDirStart = m_nodes[branch.pid]->Tn;
        vDirEnd = branch.vs - branch.ve;
        // InitM = m_nodes[branch.pid]->Bn;
        N = m_nodes[branch.pid]->Nn;
        B = m_nodes[branch.pid]->Bn;
    }

    //  cout<<B.x<<" "<<B.y<<" "<<B.z<<endl;

    for(j=0; j<SLICE_NUM; j++)
    {
        normStart[j]= N*cos(2*M_PI*j/(double)SLICE_NUM);
        vTmp = B*sin(2*M_PI*j/(double)SLICE_NUM);
        normStart[j] = normStart[j] + vTmp;
        posStart[j] = normStart[j]*0.5*branch.ds + branch.vs;    // ֻ�����İ뾶������й�ϵ
    }
    N = B.Outer(vDirEnd);
    N.Normalize();
    B = vDirEnd.Outer(N);
    B.Normalize();
    for(j=0; j<SLICE_NUM; j++)
    {
        normEnd[j] = N*cos(2*M_PI*j/(double)SLICE_NUM);
        vTmp = B*sin(2*M_PI*j/(double)SLICE_NUM);
        normEnd[j] = normEnd[j] + vTmp;
        posEnd[j] = normEnd[j]*0.5*branch.de + branch.ve;
    }
    glBegin(GL_QUAD_STRIP);
    for(j=0; j<SLICE_NUM; j++) //���ѭ��д�ĺܺ�
    {
        glNormal3f(normEnd[j].x,normEnd[j].y,normEnd[j].z);  // ����ȥʱ���ƺ���Ҫ���ÿһ����ķ���
        glTexCoord2d(j/(double)SLICE_NUM, 1.0);   //�ٴμ��㿩
        glVertex3f(posEnd[j].x, posEnd[j].y, posEnd[j].z);

        /*endIndex[j]= lastIndex+1;
        lastIndex++;*/

        glNormal3f(normStart[j].x,normStart[j].y,normStart[j].z);
        glTexCoord2d(j/(double)SLICE_NUM, 0.0);
        glVertex3f(posStart[j].x, posStart[j].y, posStart[j].z);

       /* startIndex[j] = lastIndex+1;
        lastIndex++;*/
    }
    glNormal3f(normEnd[0].x,normEnd[0].y,normEnd[0].z);
    glTexCoord2d(1.0, 1.0);
    glVertex3f(posEnd[0].x, posEnd[0].y, posEnd[0].z);
    glNormal3f(normStart[0].x,normStart[0].y,normStart[0].z);
    glTexCoord2d(1.0, 0.0);
    glVertex3f(posStart[0].x, posStart[0].y, posStart[0].z);
    glEnd();

//ȷ����һ����dominant
    for(j=0; j<branch.cnum; j++)
    {
        if(m_nodes[branch.cid[j]]->ds > maxRadii)
        {
            maxRadii = m_nodes[branch.cid[j]]->ds;
            maxPos = branch.cid[j];
        }

    }

    for(j=0; j<branch.cnum ; j++)
    {
        if(branch.cid[j]==maxPos)
            m_nodes[branch.cid[j]]->Np = true;
        else
            m_nodes[branch.cid[j]]->Np = false;

    }

    //����De�Ĳο����, ���ڴ��ݵ������á�

    branch.Bn = B;
    branch.Tn = vDirEnd;
    branch.Nn = N;

// index process and write process

  //  backEndIndex = new int [12];    //����ٿ���
    /*
     if(lastIndex==0)         // note :�����ø��ַ��ߣ����������һ��һ���ġ�
     {
         firstIndex = 1;
         lastIndex = 2;
         startIndex[0] = firstIndex;
         endIndex[0] = lastIndex;

         firstIndex = lastIndex +1;
         lastIndex = firstIndex +1;

         for(i=1;i<SLICE_NUM;i++)
         {
             endIndex[i] = firstIndex;
            startIndex[i]= lastIndex;

         firstIndex = lastIndex +1;
         lastIndex = firstIndex +1;   //���Ӧ���Ǽ���
         }

     }else{

            startIndex[0] = lastIndexSet[0];
            endIndex[0] = lastIndex;
            lastIndex++;
        for(i=1;i<SLICE_NUM;i++)
        {
           endIndex[i] = lastIndex;
           startIndex[i] = lastIndexSet[i];
           lastIndex++;
        }
     } //
    */
   /* for(i=0; i<SLICE_NUM; i++)
        backEndIndex[i] = startIndex[i];*/

// xie wen jian
/*
    for(i=0; i<SLICE_NUM; i++)
    {
        fprintf(fobj,"v ");
        fprintf(fobj,"%f %f %f\n",posStart[i].x,posStart[i].y,posStart[i].z);
        fprintf(fobj,"v ");
        fprintf(fobj,"%f %f %f\n",posEnd[i].x,posEnd[i].y,posEnd[i].z);
    }

// д����

    for(i=0; i<SLICE_NUM; i++)
    {
        fprintf(fobj,"vn ");
        fprintf(fobj,"%f %f %f\n",normStart[i].x ,normStart[i].y,normStart[i].z);
        fprintf(fobj,"vn ");
        fprintf(fobj,"%f %f %f\n",normEnd[i].x,normEnd[i].y,normEnd[i].z);
    }


//д��������   have question

    for(i=0; i<SLICE_NUM; i++)
    {
        fprintf(fobj,"vt ");
        fprintf(fobj,"%f %f\n",i/(double)SLICE_NUM, 1.0);
        fprintf(fobj,"vt ");
        fprintf(fobj,"%f %f\n",i/(double)SLICE_NUM, 0.0);
    }
    /*
     fprintf(fobj,"vt ");
        fprintf(fobj,"%f %f\n",1.0,1.0);
        fprintf(fobj,"vt ");
        fprintf(fobj,"%f %f\n",1.0,0.0);
    */
// xie face
    /*
      fprintf(fobj,"f ");
       fprintf(fobj,"%d/%d/%d\n",startIndex[0],startIndex[0],startIndex[0])
       fprintf(fobj,"%d/%d/%d\n",endIndex[0],endIndex[0],endIndex[0])
        fprintf(fobj,"%d/%d/%d\n",endIndex[1],endIndex[1],endIndex[1]);
       fprintf(fobj,"%d/%d/%d\n",startIndex[1],startIndex[1],startIndex[1]);*/
       ////////////////////
       /*
    for(i=0; i<SLICE_NUM-1; i++)
    {
        fprintf(fobj,"f ");

        fprintf(fobj,"%d/%d/%d ",endIndex[i],endIndex[i],endIndex[i]);
        fprintf(fobj,"%d/%d/%d ",startIndex[i],startIndex[i],startIndex[i]);
        fprintf(fobj,"%d/%d/%d ",startIndex[i+1],startIndex[i+1],startIndex[i+1]);
        fprintf(fobj,"%d/%d/%d\n",endIndex[i+1],endIndex[i+1],endIndex[i+1]);

    }
// ���һ����

    fprintf(fobj,"f ");

    fprintf(fobj,"%d/%d/%d ",endIndex[11],endIndex[11],endIndex[11]);
    fprintf(fobj,"%d/%d/%d ",startIndex[11],startIndex[11],startIndex[11]);
    fprintf(fobj,"%d/%d/%d ",startIndex[0],startIndex[0],startIndex[0]);
    fprintf(fobj,"%d/%d/%d\n",endIndex[0],endIndex[0],endIndex[0]);
*/
    /*
      fprintf(fobj,"%d//%d ",endIndex[11],endIndex[11]);
        fprintf(fobj,"%d//%d ",startIndex[11],startIndex[11]);
        fprintf(fobj,"%d//%d ",startIndex[0],startIndex[0]);
        fprintf(fobj,"%d//%d\n",endIndex[0],endIndex[0]);
    */



  //  return backEndIndex;

}


void CTree::ArrangeWhorledLeaves(CVector3D &vs, CVector3D &ve) //��Ҫ��֧����ʼ��������ֹ����
{
    int i;
    CMatrix3D m , m0;
    CVector3D v, vPos, vtmp, p,n0,n1,n2;
    CVector3D vLeafPetiole(1,0,0), vLeafNorm, vy;
    double alpha, beta, gamma;
    double phai;

    int j;

    v = ve - vs;//direction of current segment
    v.Normalize();
    p.Set(v.x,0,v.z); //project vector "v" on xoz plane and get new vector p
    if(p.Length()<0.001) p.Set(1, 0, 0);
    alpha = p.Angle(v); // get the inclined angle of "v" and xoz plane
    n0 = p.Outer(v); // get the normal of surface determined by p*v

    if(n0.Length()<0.001)
    {
        vtmp.Set(0, 1, 0);
        n0 = p.Outer(vtmp);
        if(n0.Length()<0.001)
            printf("n0 error %f %f %f %f %f %f\n", p.x, p.y, p.z, vtmp.x, vtmp.y, vtmp.z);
    }

    n0.Normalize();
    m0.LoadIdentity();

    for(i=0; i<LEAF_NUM; i++)
        m_leafmat[i].LoadIdentity();

    for(i=0; i<LEAF_NUM; i++)
    {
        vLeafNorm.Set(0,1,0);
        beta = vLeafPetiole.Angle(n0);
        if(beta<0.001)
            n1.Set(0, 1, 0);
        else
            n1 = vLeafPetiole.Outer(n0);
        n1.Normalize();
        m.LoadIdentity();
        m.Rotate(-beta, n1);
        vLeafNorm = m*vLeafNorm;

        gamma = v.Angle(vLeafNorm);
        n2 = v.Outer(vLeafNorm);
        if(n2.Length()<0.001)
        {
            vtmp.Set(0,0,1);
            n2 = v.Outer(vtmp);
            if(n2.Length()<0.001)
                printf("n2 error %f %f %f %f %f %f\n", v.x, v.y, v.z, vtmp.x, vtmp.y, vtmp.z);
        }
        n2.Normalize();
        m0.LoadIdentity();
        m0.Rotate(gamma,n2);
        m= m0*m;

        m0.LoadIdentity();
        p = v.Outer(n0);
        if(p.Length()<0.001)
        {
            printf("p error %f %f %f %f %f %f\n", v.x, v.y, v.z, n0.x, n0.y, n0.z);
        }
        p.Normalize();
        m0.Rotate(M_PI/2 - M_PI/8*(2+rand()/(float)RAND_MAX) - M_PI/6*(rand()/(float)RAND_MAX-0.5), p);
        m= m0*m;

        //translate the leaf to the corresponding joint
        m0.LoadIdentity();
        vPos = (ve-vs)*(i/(double)(LEAF_NUM-1));
        vPos = vs + vPos;
        m0.Translate(vPos);
        m = m0*m;

        m_leafmat[i] = m;

      //  transFormLeaf(m);

        //Rotate leaf about branch direction with an angle close to the golder angle 137.5 degree
        m0.LoadIdentity();
        phai = 137.5*M_PI/180 + 14*M_PI/180*(rand()/(float)RAND_MAX-0.5);

        m0.Rotate(phai, v);
        n0 = m0*n0;
        n0.Normalize();

        // xie m
        /*
                for(j=0;j<16;j++)
                {
                    fprintf(fwriteMatrix,"%lf ",m[j]);
                }
                fprintf(fwriteMatrix,"\n");
        */

    }
}

void CTree::DrawTree()
{
    if(m_nTreeList!=0)
        glCallList(m_nTreeList);
}

void CTree::DrawColorTree()
{
    if(m_nColorTreeList!=0)

         glCallList(m_nColorTreeList);


}

void CTree::ChangeTreeLevel()
{
    int i ;

    for(i=0; i<m_num; i++)
        if(m_nodes[i]->flag==true)
            m_nodes[i]->cnum = m_cnum[i];
    for(i=0; i<m_num; i++)
        if(m_nodes[i]->level>g_currLevel && m_nodes[i]->flag==true)
            m_nodes[i]->cnum = 0;
    DepthFirstSearch(*m_nodes[0]);

    AdjustBranches();

}

double CTree::DepthFirstSearch(CBranch &branch)    //����ʼ�˰뾶��adjust ����ĩ�˰뾶
{
    int i;
    double  r, rsum;

    if(branch.cnum==0)
        branch.ds = g_thickness;
    else
    {
        rsum = 0.0;

        for(i=0; i<branch.cnum; i++)
        {
            r = DepthFirstSearch(*m_nodes[branch.cid[i]]);
            rsum += r*r;
        }
        branch.ds = sqrt(rsum);
    }

    return branch.ds;
}

void CTree::ReleaseNodes()
{
    int i;
    for(i=0; i<m_num; i++)
    {
        if(m_nodes[i]->cnum!=0 && m_nodes[i]->cid!=NULL)
        {
            delete [] m_nodes[i]->cid;
            m_nodes[i]->cid = NULL;
            m_nodes[i]->cnum = 0;
        }
    }
    if(m_num!=0 && m_nodes.size()>0)
    {
        // delete []m_nodes;
        m_nodes.clear();
        // m_nodes = NULL;
        m_num = 0;
        m_cnum.clear();
    }
}

void CTree::Update()
{
    // lastIndex=1;
    ChangeTreeLevel();
    CreateTreeList();
}

CTree::~CTree()
{
    ReleaseNodes();
}


/*
��Ҫǿ��һ�㣺����ĳһ����֧��һЩ���ݣ��Ƿ���m_nodes��id������ġ�
��Ҫ�����ľ���m_nodes��������

��֦����ϸ�ṹһ������m_nodes��
*/


//optimization�������õģ��������ԣ�ȷʵ�бߣ�û�б����ƣ�


/*
  1�����ĳһ��ĺ��ӽڵ����ཻ�沿���Ƿ񳬹�50%

   ��������������
*/


int CTree::optimization()
{
    int i ,j ,k;
    int m;

    int idNext;
    double acosValue=0.0;

    int count = 0;

    CVector3D idCurrentStart,idCurrentEnd;
    CVector3D idNextStart,idNextEnd;

    CVector3D idCurrentDir,idNextDir;

    int idCurrentLen, idNextLen;
    double crossArea=0.0;

    for(i=0; i<m_num; i++) //���ǲ�νṹ�ģ���ˡ�������
    {
        if(m_nodes[i]->flag==false)
            continue;
        if(m_nodes[i]->cnum==0)
            continue;
        for(j=0; j<m_nodes[i]->cnum; j++) //һ����˵������С֦��û�к��ӽڵ�
        {
            if(m_nodes[j]->flag==false)    //��������Ƕ����
                continue;
            if(m_nodes[m_nodes[i]->cid[j]]->cnum==0)  //��ȡһ���к��ӽڵ�ĵ�
                continue;
            idCurrentDir=m_nodes[m_nodes[i]->cid[j]]->ve - m_nodes[m_nodes[i]->cid[j]]->vs;
            idCurrentLen = idCurrentDir.Length();

            for(k=0; k<m_nodes[i]->cnum; k++)
            {
                if(m_nodes[k]->flag==false)
                    continue;
                if(m_nodes[m_nodes[i]->cid[k]]->cnum!=0)
                    continue;
                idNextDir = m_nodes[m_nodes[i]->cid[k]]->ve - m_nodes[m_nodes[i]->cid[k]]->vs;
                idNextLen = idNextDir.Length();

                idNext = m_nodes[i]->cid[k];

                acosValue = idNextDir.Angle(idCurrentDir);
                crossArea = (m_nodes[m_nodes[i]->cid[j]]->ds / 2.0) / sin(acosValue) ;   //



                if(fabs(crossArea-idNextLen)< idNextLen*0.4)
                {
                    m_nodes[m_nodes[i]->cid[k]]->flag=false;

                    m_nodesColor[m_nodesColor[i]->cid[k]]->flag=false;  //

                    for(m=0; m<m_nodes[i]->cnum; m++)
                        if(m_nodes[i]->cid[m]==idNext)
                            break;
                    if ( m+1 == m_nodes[m_nodes[idNext]->pid]->cnum )
                    {
                        m_nodes[i]->cnum -=1;
                        m_cnum[i] -= 1;

                    }
                    else
                    {
                        for(; m<m_nodes[i]->cnum -1 ; m++)
                            m_nodes[i]->cid[m]=m_nodes[i]->cid[m+1];

                        m_nodes[i]->cnum-=1;
                        m_cnum[i] -= 1;
                    }
                    count++;

                }
            }
        }
    }

    cout<<"optimization:"<<count<<endl;
    DepthFirstSearch(*m_nodes[0]);
    AdjustBranches();


    return count;

}



/*
 1��ͬһ�����ڵ�ģ�����Ҫ���ԣ�
 2��Z�������Ըߵģ�����һ����εģ�����Ҫ���ԣ�
 3��ͬһ���level�ģ�����Ҫ���ԣ�
 4��optimization�Ż����ģ�����Ҫ���ԣ�

 notice : һ����С�ߴ����ߣ�����Ҫ���жϣ��ٴ���

 ���ǣ���֦��С֦ ,���ô���ж���������


 version V-1.0  3-25

*/

int CTree::interSectTionOptimization()
{
    //�鿴�������Ƿ��ཻ
    int i , j , m ,k ;

    double distence=0.0;

    int count=0;
    int *CopySpace;

    int high , middle , low;
    int countNO=0;

    cout<<"intersect enter"<<endl;
    //  ofstream outFile(".\\model\\myoutcome.txt");

    for(i=0; i<m_num-1; i++)
    {

        if(m_nodes[i]->flag==false) continue;    //Ϊ��ɾ���ĺ��ӽڵ㲻�ڲ���

        for(j=i+1; j<m_num; j++)
        {
            if(m_nodes[j]->flag==false) continue;   ////Ϊ��ɾ���ĺ��ӽڵ㲻�ڲ���
            //���i��j�����Ƿ��ཻ���ߴ���
            if (m_nodes[j]->pid == i || m_nodes[j]->pid == m_nodes[i]->pid)    //ͬ����һ�����ڵ��������֧��һ�����ཻ
                continue;
            // else  if(m_nodes[j].level==m_nodes[i].level)    //ͬһ�㻹���п����ཻ��
            //  continue;
            else if(m_nodes[j]->vs.y >= m_nodes[i]->ve.y || m_nodes[j]->ve.y <=  m_nodes[i]->vs.y )   //�ӵȺſ��Ա�����ѭ��
                //else if(!((m_nodes[j].vs.z>m_nodes[i].vs.z &&m_nodes[j].vs.z<m_nodes[i].ve.z) && (m_nodes[i].vs.z>m_nodes[j].vs.z &&m_nodes[i].vs.z<m_nodes[j].ve.z)))
                continue;

            distence = LineDistence::DistanceLineToLine(m_nodes[i]->vs,m_nodes[i]->ve,m_nodes[j]->vs,m_nodes[j]->ve);
            //  outFile<<distence<<endl;

            //ɾ������   ---������Ҫ֦�� �� ���֦��ռ��Ҫ

            if( m_nodes[i]->ds >= m_nodes[j]->ds && distence<= m_nodes[i]->de /2.0)
            {
                //i�� ��ɾ��������j
                low=m_nodes[i]->vs.y;     //begin  ����z���������м���
                high= m_nodes[i]->ve.y;
                middle = (low+high)/2.0;

//  û�к��ӽڵ㣬ɾ��
                if(m_nodes[j]->cnum==0)
                {
                    countNO++;
                    m_nodes[j]->flag=false;

                    //�ڸ��ڵ���ɾ��
                    for(k=0; k<m_nodes[m_nodes[j]->pid]->cnum ; k++)
                    {
                        if(m_nodes[m_nodes[j]->pid]->cid[k] == j)
                            break;

                    }
                    if(k== (m_nodes[m_nodes[j]->pid]->cnum - 1))
                    {
                        (m_nodes[m_nodes[j]->pid]->cnum) -- ;
                        m_cnum[m_nodes[j]->pid] --;
                    }
                    else
                    {
                        for(; k<(m_nodes[m_nodes[j]->pid]->cnum - 1); k++)
                            m_nodes[m_nodes[j]->pid]->cid[k]=m_nodes[m_nodes[j]->pid]->cid[k+1];

                        (m_nodes[m_nodes[j]->pid]->cnum) -- ;
                        m_cnum[m_nodes[j]->pid] --;
                    }

                }

//
                else if(m_nodes[j]->vs.y > middle && m_nodes[j]->vs.y<high && m_nodes[j]->ve.y > m_nodes[i]->ve.y)    //һ������һ����Χ�ڵ�
                {
                    count++;
                    //���ϰ벿�ֲ���  �޸�����  ���뵽i�ĺ����б���
                    CopySpace = new int [m_nodes[i]->cnum+1];
                    for(m=0; m<m_nodes[i]->cnum; m++)
                        CopySpace[m] = m_nodes[i]->cid[m];

                    CopySpace[m_nodes[i]->cnum] =  j ;
                    delete []m_nodes[i]->cid ;
                    m_nodes[i]->cid = new int[m_nodes[i]->cnum+1];
                    for(m=0; m<=m_nodes[i]->cnum; m++)
                        m_nodes[i]->cid[m]=CopySpace[m];

                    //�޸� i�ĺ�����Ϣ
                    m_nodes[i]->cnum++;
                    m_cnum[i]++;

                    //ɾ����ԭ���б��λ��
                    for(m=0; m<m_nodes[m_nodes[j]->pid]->cnum; m++)
                        if(m_nodes[m_nodes[j]->pid]->cid[m]==j)
                            break;
                    if(m==m_nodes[m_nodes[j]->pid]->cnum -1 )
                    {
                        m_nodes[m_nodes[j]->pid]->cnum-=1;
                        m_cnum[m_nodes[j]->pid] -=1;
                    }
                    else
                    {
                        for(; m<m_nodes[m_nodes[j]->pid]->cnum-1; m++)
                            m_nodes[m_nodes[j]->pid]->cid[m]=m_nodes[m_nodes[j]->pid]->cid[m+1];

                        m_nodes[m_nodes[j]->pid]->cnum-=1;
                        m_cnum[m_nodes[j]->pid] -=1;
                    }

                    //�޸�j �ڵ��һЩ��Ϣ
                    m_nodes[j]->pid = i ;
                    m_nodes[j]->vs= m_nodes[i]->ve;
                    //     m_nodes[j]->ds= m_nodes[i]->de;

                }
                else if(m_nodes[j]->vs.y > low  && m_nodes[j]->vs.y<middle)
                {
                    count ++;
                    //���м����ֵ
                    CopySpace = new int [m_nodes[i]->cnum+1];
                    for(m=0; m<m_nodes[i]->cnum; m++)
                        CopySpace[m] = m_nodes[i]->cid[m];

                    CopySpace[m_nodes[i]->cnum] =  j ;
                    delete []m_nodes[i]->cid ;
                    m_nodes[i]->cid = new int[m_nodes[i]->cnum+1];
                    for(m=0; m<=m_nodes[i]->cnum; m++)
                        m_nodes[i]->cid[m]=CopySpace[m];

                    //�޸� i�ĺ�����Ϣ
                    m_nodes[i]->cnum++;
                    m_cnum[i]++;

                    //ɾ����ԭ���б��λ��
                    for(m=0; m<m_nodes[m_nodes[j]->pid]->cnum; m++)
                        if(m_nodes[m_nodes[j]->pid]->cid[m]==j)
                            break;
                    if(m==m_nodes[m_nodes[j]->pid]->cnum -1 )
                    {
                        m_nodes[m_nodes[j]->pid]->cnum-=1;
                        m_cnum[m_nodes[j]->pid] -=1;
                    }
                    else
                    {
                        for(; m<m_nodes[m_nodes[j]->pid]->cnum-1; m++)
                            m_nodes[m_nodes[j]->pid]->cid[m]=m_nodes[m_nodes[j]->pid]->cid[m+1];

                        m_nodes[m_nodes[j]->pid]->cnum-=1;
                        m_cnum[m_nodes[j]->pid] -=1;
                    }

                    //�޸�j �ڵ��һЩ��Ϣ
                    m_nodes[j]->pid = i ;
                    m_nodes[j]->vs= (m_nodes[i]->ve + m_nodes[i]->vs)/2.0;
                    //   m_nodes[j]->ds= (m_nodes[i]->de + m_nodes[i]->ds)/2.0;

                }
                else        //����������������±߲��롣
                {
                    count++;
                    //���±߲���ֵ
                    CopySpace = new int [m_nodes[m_nodes[i]->pid]->cnum+1];
                    for(m=0; m<m_nodes[m_nodes[i]->pid]->cnum; m++)
                        CopySpace[m] = m_nodes[m_nodes[i]->pid]->cid[m];

                    CopySpace[m] = j;
                    delete []m_nodes[m_nodes[i]->pid]->cid;
                    m_nodes[m_nodes[i]->pid]->cid = new int [m_nodes[m_nodes[i]->pid]->cnum+1];
                    for(m=0; m<=m_nodes[m_nodes[i]->pid]->cnum; m++)
                        m_nodes[m_nodes[i]->pid]->cid[m] = CopySpace[m];

                    m_nodes[m_nodes[i]->pid]->cnum ++ ;
                    m_cnum[m_nodes[i]->pid] ++;

                    //ɾ������
                    for(m=0; m<m_nodes[m_nodes[j]->pid]->cnum; m++)
                        if(m_nodes[m_nodes[j]->pid]->cid[m]==j)
                            break;
                    if(m==m_nodes[m_nodes[j]->pid]->cnum -1 )
                    {
                        m_nodes[m_nodes[j]->pid]->cnum-=1;
                        m_cnum[m_nodes[j]->pid] -=1;
                    }
                    else
                    {
                        for(; m<m_nodes[m_nodes[j]->pid]->cnum-1; m++)
                            m_nodes[m_nodes[j]->pid]->cid[m]=m_nodes[m_nodes[j]->pid]->cid[m+1];

                        m_nodes[m_nodes[j]->pid]->cnum-=1;
                        m_cnum[m_nodes[j]->pid] -=1;
                    }

                    //�޸�j �ڵ��һЩ��Ϣ
                    m_nodes[j]->pid = m_nodes[i]->pid ;
                    m_nodes[j]->vs=  m_nodes[i]->vs;
                }

            }// end of if  i  is the  main
            else  if(m_nodes[i]->ds < m_nodes[j]->ds && distence< m_nodes[j]->de /2.0) //j  is  main
            {
                // count++;
                //j�� ��ɾ��������i
                low=m_nodes[j]->vs.y;     //begin  ����z���������м���
                high= m_nodes[j]->ve.y;
                middle = (low+high)/2.0;
//  û�к��ӽڵ㣬ɾ��
                if(m_nodes[i]->cnum==0)
                {
                    countNO++;
                    m_nodes[i]->flag=false;

                    //�ڸ��ڵ���ɾ��
                    for(k=0; k<m_nodes[m_nodes[i]->pid]->cnum ; k++)
                    {
                        if(m_nodes[m_nodes[i]->pid]->cid[k] == i)
                            break;

                    }
                    if(k== (m_nodes[m_nodes[i]->pid]->cnum - 1))
                    {
                        (m_nodes[m_nodes[i]->pid]->cnum) -- ;
                        m_cnum[m_nodes[i]->pid] --;
                    }
                    else
                    {
                        for(; k<(m_nodes[m_nodes[i]->pid]->cnum - 1); k++)
                            m_nodes[m_nodes[i]->pid]->cid[k]=m_nodes[m_nodes[i]->pid]->cid[k+1];

                        (m_nodes[m_nodes[i]->pid]->cnum) -- ;
                        m_cnum[m_nodes[i]->pid] --;
                    }
                    break;
                }

//
                else if(m_nodes[i]->vs.y > middle && m_nodes[i]->vs.y<high )    //һ������һ����Χ�ڵ�
                {
                    count++;
                    //���ϰ벿�ֲ���  �޸�����  ���뵽i�ĺ����б���
                    CopySpace = new int [m_nodes[j]->cnum+1];
                    for(m=0; m<m_nodes[j]->cnum; m++)
                        CopySpace[m] = m_nodes[j]->cid[m];

                    CopySpace[m_nodes[j]->cnum] =  i ;
                    delete []m_nodes[j]->cid ;
                    m_nodes[j]->cid = new int[m_nodes[j]->cnum+1];
                    for(m=0; m<=m_nodes[j]->cnum; m++)
                        m_nodes[j]->cid[m]=CopySpace[m];

                    //�޸� i�ĺ�����Ϣ
                    m_nodes[j]->cnum++;   //��������ȼ���Ҫ������
                    m_cnum[j]++;

                    //ɾ����ԭ���б��λ��
                    for(m=0; m<m_nodes[m_nodes[i]->pid]->cnum; m++)
                        if(m_nodes[m_nodes[i]->pid]->cid[m]==i)
                            break;
                    if(m==m_nodes[m_nodes[i]->pid]->cnum -1 )
                    {
                        m_nodes[m_nodes[i]->pid]->cnum-=1;
                        m_cnum[m_nodes[i]->pid] -=1;
                    }
                    else
                    {
                        for(; m<m_nodes[m_nodes[i]->pid]->cnum-1; m++)
                            m_nodes[m_nodes[i]->pid]->cid[m]=m_nodes[m_nodes[i]->pid]->cid[m+1];

                        m_nodes[m_nodes[i]->pid]->cnum-=1;
                        m_cnum[m_nodes[i]->pid] -=1;
                    }

                    //�޸�j �ڵ��һЩ��Ϣ
                    m_nodes[i]->pid = j ;
                    m_nodes[i]->vs= m_nodes[j]->ve;
                    //  m_nodes[i]->ds= m_nodes[j]->de;

                }
                else if(m_nodes[i]->vs.y > low  && m_nodes[i]->vs.y<middle)
                {
                    count++;
                    //���м����ֵ
                    CopySpace = new int [m_nodes[j]->cnum+1];
                    for(m=0; m<m_nodes[j]->cnum; m++)
                        CopySpace[m] = m_nodes[j]->cid[m];

                    CopySpace[m_nodes[j]->cnum] =  i ;
                    delete []m_nodes[j]->cid ;
                    m_nodes[j]->cid = new int[m_nodes[j]->cnum+1];
                    for(m=0; m<=m_nodes[j]->cnum; m++)
                        m_nodes[j]->cid[m]=CopySpace[m];

                    //�޸� i�ĺ�����Ϣ
                    m_nodes[j]->cnum++;
                    m_cnum[j]++;

                    //ɾ����ԭ���б��λ��
                    for(m=0; m<m_nodes[m_nodes[i]->pid]->cnum; m++)
                        if(m_nodes[m_nodes[i]->pid]->cid[m]==i)
                            break;
                    if(m==m_nodes[m_nodes[i]->pid]->cnum -1 )
                    {
                        m_nodes[m_nodes[i]->pid]->cnum-=1;
                        m_cnum[m_nodes[i]->pid] -=1;
                    }
                    else
                    {
                        for(; m<m_nodes[m_nodes[i]->pid]->cnum-1; m++)
                            m_nodes[m_nodes[i]->pid]->cid[m]=m_nodes[m_nodes[i]->pid]->cid[m+1];

                        m_nodes[m_nodes[i]->pid]->cnum-=1;
                        m_cnum[m_nodes[i]->pid] -=1;
                    }

                    //�޸�j �ڵ��һЩ��Ϣ
                    m_nodes[i]->pid = j ;
                    m_nodes[i]->vs= (m_nodes[j]->ve + m_nodes[j]->vs)/2.0;
                    //   m_nodes[i]->ds= (m_nodes[j]->de + m_nodes[j]->ds)/2.0;

                }
                else
                {
                    count++;
                    //���±߲���ֵ
                    CopySpace = new int [m_nodes[m_nodes[j]->pid]->cnum+1];
                    for(m=0; m<m_nodes[m_nodes[j]->pid]->cnum; m++)
                        CopySpace[m] = m_nodes[m_nodes[j]->pid]->cid[m];

                    CopySpace[m] = i;
                    delete []m_nodes[m_nodes[j]->pid]->cid;
                    m_nodes[m_nodes[j]->pid]->cid = new int [m_nodes[m_nodes[j]->pid]->cnum+1];
                    for(m=0; m<=m_nodes[m_nodes[j]->pid]->cnum; m++)
                        m_nodes[m_nodes[j]->pid]->cid[m] = CopySpace[m];

                    m_nodes[m_nodes[j]->pid]->cnum ++ ;
                    m_cnum[m_nodes[j]->pid] ++;

                    //ɾ������
                    for(m=0; m<m_nodes[m_nodes[i]->pid]->cnum; m++)
                        if(m_nodes[m_nodes[i]->pid]->cid[m]==i)
                            break;
                    if(m==m_nodes[m_nodes[i]->pid]->cnum -1 )
                    {
                        m_nodes[m_nodes[i]->pid]->cnum-=1;
                        m_cnum[m_nodes[i]->pid] -=1;
                    }
                    else
                    {
                        for(; m<m_nodes[m_nodes[i]->pid]->cnum-1; m++)
                            m_nodes[m_nodes[i]->pid]->cid[m]=m_nodes[m_nodes[i]->pid]->cid[m+1];

                        m_nodes[m_nodes[i]->pid]->cnum-=1;
                        m_cnum[m_nodes[i]->pid] -=1;
                    }

                    //�޸�j �ڵ��һЩ��Ϣ
                    m_nodes[i]->pid = m_nodes[j]->pid ;
                    m_nodes[i]->vs=  m_nodes[j]->vs;

                }
            }//end of main else
        }//second for
    } // first for

    cout<<"Intersection:"<<count<<endl;

    //  outFile.close();

    DepthFirstSearch(*m_nodes[0]);

    cout<<"finish search"<<endl;
    AdjustBranches();

    cout<<"finish"<<endl;
    cout<<"countNO"<<countNO<<endl;
    return count;
}


/*
�����ܽ᣺

��ʵ������û�д���ֻ����ɾ���ڵ��ʱ��ʹ��flag��ǰ�ԭ�������ݽṹ���׵ĸ�Ū����
�����µ�������ʱ���������У���û������ġ���

*/


//ɾ�����������֦

/*
�Ƴ�˵����
1��������֦����С�� 4��deleteRadii ��û�к��ӽڵ�� ��֦��ɾ��  ���к��ӽڵ�ģ��ϲ�   ����������


NOTICE��ע��ά�����Ľṹ����Ҫ���� .���⣬������Ҫ��optimization��intersection֮ǰ execute

stratege 01;
�����ĺ��ӽڵ����

*/


void CTree::RemoveIrregularBranch()
{
    cout<<"remove enter"<<endl;

    int i ,j,k;
    double BranchLength;
    CVector3D vector_01;

    int childNum;
    int *tempChildList;

    for(i=0; i<m_num; i++)
    {
        if(m_nodes[i]->flag==false)
            continue;
        //Ԥ�ȴ����ӽڵ㣬�ٴ�������
        childNum = m_nodes[i]->cnum ;  //��������Ϊ�ں����ɾ�������У����ܻ�ı�cnum������ѭ���Ĵ�������
        //�ȴ����� ,ɾ�����ӽڵ�
        if(childNum>0)
        {
            for(j=0; j<childNum; j++)
            {
                if(m_nodes[m_nodes[i]->cid[j]]->flag==true && m_nodes[m_nodes[i]->cid[j]]->cnum==0)
                {
                    vector_01 = m_nodes[m_nodes[i]->cid[j]]->ve - m_nodes[m_nodes[i]->cid[j]]->vs;
                    BranchLength = vector_01.Length();

                    if(BranchLength<=DELETESPAN)  //��4����֮�ڣ���Ҫɾ������
                    {
//ɾ���ڵ�
                        m_nodes[m_nodes[i]->cid[j]]->flag=false;
                      //  m_nodesColor[m_nodesColor[i]->cid[j]]->flag=false;
                        //ɾ�����ڸ��ڵ㺢���е�λ��

                        if(j==m_nodes[i]->cnum - 1)
                        {
                            m_nodes[i]->cnum --;
                            m_cnum[i] --;
                        }
                        else
                        {
                            k=j;
                            for(; k<m_nodes[i]->cnum - 1 ; k++)
                                m_nodes[i]->cid[k] =   m_nodes[i]->cid[k+1];

                            m_nodes[i]->cnum --;
                            m_cnum[i] --;

                        }
                    }//
                }
            }
        } //  cnum>0
        //�������ɣ�����û�к��ӽڵ����֦�����Ӧ�ñ�������ˣ����������ٴδ���

        //01 ,�����ɵĳ���

        vector_01 = m_nodes[i]->ve - m_nodes[i]->vs;
        BranchLength = vector_01.Length();
        if(m_nodes[i]->cnum==0)   //
        {
            if(BranchLength<=DELETESPAN && m_nodes[i]->pid!= -1)
            {
                //�ϲ������ڵ���  �к��ӣ����ϲ���û�к��Ӻϲ�
                if( m_nodes[m_nodes[i]->pid]->cnum==1)
                {
                    m_nodes[i]->flag=false;
                 //   m_nodesColor[i]->flag=false;

                    m_nodes[m_nodes[i]->pid]->ve = m_nodes[i]->ve;

                    m_nodes[m_nodes[i]->pid]->cnum=0;
                    m_cnum[m_nodes[i]->pid] = 0;

                }
            }
        }
        else if(m_nodes[i]->cnum==1 && BranchLength <= DELETESPAN && m_nodes[i]->pid!= -1 )
        {
            //��������Ҫ�ϲ�������
            m_nodes[i]->flag=false;
           // m_nodesColor[i]->flag=false;

            m_nodes[i]->cnum=0;
            m_cnum[i]=0;

            m_nodes[m_nodes[i]->cid[0]]->vs = m_nodes[i]->vs;
            m_nodes[m_nodes[i]->cid[0]]->pid = m_nodes[i]->pid;

            //�޸ĸ��ڵ�ĺ�������
            for(k=0; k<m_nodes[m_nodes[i]->pid]->cnum ; k++)
            {
                if(m_nodes[m_nodes[i]->pid]->cid[k]==i)
                {
                    m_nodes[m_nodes[i]->pid]->cid[k]  = m_nodes[i]->cid[0];
                    break;
                }
            }
            //���ڵ�
        }
        // �����һ���ڵ�����
        else if(m_nodes[i]->cnum==1 && m_nodes[i]->pid== -1 && BranchLength<= DELETESPAN)
        {
            //��������Ҫ�ϲ�������
            m_nodes[i]->flag=false;
          //  m_nodesColor[i]->flag=false;
            m_nodes[i]->cnum=0;
            m_cnum[i]=0;

            m_nodes[m_nodes[i]->cid[0]]->vs = m_nodes[i]->vs;
            m_nodes[m_nodes[i]->cid[0]]->pid = m_nodes[i]->pid;

        }
        else if(m_nodes[i]->cnum >1 && BranchLength<=DELETESPAN && m_nodes[i]->pid!= -1 )
        {
            if(m_nodes[m_nodes[i]->pid]->cnum==1)
            {
                //�ϲ�����֫ ���ڵ㣬ע���ڴ�Ŀ��� ��i�ĺ��ӣ����丸�ڵ��

                tempChildList = new int [m_nodes[i]->cnum];
                for(k=0; k<m_nodes[i]->cnum; k++)
                    tempChildList[k] = m_nodes[i]->cid[k];

                //ɾ��
                m_nodes[i]->flag=false;
              //  m_nodesColor[i]->flag=false;

                //�޸�����
                m_nodes[m_nodes[i]->pid]->ve = m_nodes[i]->ve;

                delete m_nodes[m_nodes[i]->pid]->cid;

                m_nodes[m_nodes[i]->pid]->cid = new int [m_nodes[i]->cnum];

                for(k=0; k<m_nodes[i]->cnum; k++)
                {
                    m_nodes[m_nodes[i]->pid]->cid[k] = tempChildList[k];
                    m_nodes[tempChildList[k]]->pid = m_nodes[i]->pid;
                }

                m_nodes[m_nodes[i]->pid]->cnum = m_nodes[i]->cnum;
                m_cnum[m_nodes[i]->pid] = m_nodes[i]->cnum;

                m_nodes[i]->cnum = 0;
                m_cnum[i] = 0;
            }

        }

    }// end of last for

    UpdateLevel();
    g_thickness+=0.01;
    DepthFirstSearch(*m_nodes[0]);
    cout<<"finish remove"<<endl;

}

/*

���¸������Ĳ�νṹ

*/

void CTree::UpdateLevel()
{
    int i ;
    int pos;

    int count=1;
    queue<int> Q;

    Q.push(0);
    while(!Q.empty())
    {
        pos = Q.front();
        Q.pop();
        for(i=0; i<m_nodes[pos]->cnum; i++)
        {
            m_nodes[m_nodes[pos]->cid[i]]->level = m_nodes[pos]->level +1;
            Q.push(m_nodes[pos]->cid[i]);
        }
        count++;
    }

    cout<<count<<endl;

}

/*ֻ����˱ߣ���ɾ��*/

void CTree::processBranchWithAngle()
{
    int i,j,k;
    double maxValue;
    int maxPos;
    CVector3D parentV, childV;
    int branchBum;

    bool addFlag ;
    double cosValue;
    double Theta;
    CBranch *newbranch;

    branchBum = m_num;

    cout<<m_num<<" ";

    for(i=0; i<branchBum; i++)
    {
        if(m_nodes[i]->flag==false)
         continue;

        parentV = m_nodes[i]->ve - m_nodes[i]->vs;
        // parentV = parentV * 0.5;
        maxPos = -1;
        maxValue = -1.0;
        addFlag = false;

        for(j=0; j<m_nodes[i]->cnum ; j++)
        {
            if(m_nodes[m_nodes[i]->cid[j]]->ds > maxValue)
            {
                maxValue = m_nodes[m_nodes[i]->cid[j]]->ds;
                maxPos = m_nodes[i]->cid[j] ;
                addFlag = true;
            }
        }   // Ѱ������

        if(addFlag==true)
        {
            childV = m_nodes[maxPos]->ve - m_nodes[maxPos]->vs;
            // childV  = childV *0.5;

            cosValue = fabs(parentV.Inner(childV)) / (parentV.Length() * childV.Length());
            Theta = acos(cosValue);

            if(Theta>0.94)  // insert   �����Գ����ӡ��ܴ�
            {
                parentV = parentV *0.5;
                childV = childV *0.5;

                //add new branch

                newbranch = (CBranch *)malloc(sizeof(CBranch));
                newbranch->vs = m_nodes[i]->vs + parentV;
                newbranch->ve = m_nodes[maxPos]->vs + childV;
                newbranch->de = m_nodes[maxPos]->ds;
                newbranch->ds = m_nodes[i]->de;

                newbranch->pid = i;
                newbranch->cnum = 1;
                newbranch->cid = (int *)malloc(sizeof(int)*1);
                newbranch->cid[0]= maxPos;
                newbranch->level = m_nodes[maxPos]->level;
                newbranch->flag= true;
                m_nodes.push_back(newbranch); //
                m_cnum.push_back(1);  //
// xiu gai yuan shu zhi
                for(k=0; k<m_nodes[i]->cnum ; k++)
                {
                    if(m_nodes[i]->cid[k] == maxPos)
                    {
                        m_nodes[i]->cid[k] = m_num;
                    }
                    else
                    {
                        m_nodes[m_nodes[i]->cid[k]]->vs = newbranch->vs;
                    }
                }
                m_nodes[i]->ve = newbranch->vs;
                //
                m_nodes[maxPos]->vs = newbranch->ve;
                m_nodes[maxPos]->pid = m_num;

                m_num ++;
            }
        }
    }//end of last for
    cout<<m_num<<endl;
}

void CTree::MarkBranch()
{
    int i;

    for(i=0;i<m_num;i++)
    {
        m_nodesColor[i]->flag = m_nodes[i]->flag;
    }
}

double CTree::CalculateRadii(CBranch &branch)
{
   // cout<<".."<<endl;
     int i;
    double value;
    double sum=0.0;

    if(branch.cnum==0)
        branch.ds=g_thickness;
    else
    {
        for(i=0; i<branch.cnum; i++)
        {
            value=CalculateRadii(*m_nodes[branch.cid[i]]);
            sum+=value*value;
        }

        branch.ds=sqrt(sum);
    }

    return  branch.ds;
}
