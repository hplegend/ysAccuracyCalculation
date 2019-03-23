#include "LineDistence.h"
//线段点A 和线段点 B ，以及另外的一点
double LineDistence::DistancePointToLine(  CVector3D star,  CVector3D end, CVector3D center )
{
double distance;
double x0 = center.x;//P点坐标（x0,y0,z0）
double y0 = center.y;
double z0 = center.z;
double x1 = star.x; //A点坐标（x1,y1,z1）
double y1 = star.y;
double z1 = star.z;
double x2 = end.x;//B点坐标（x2,y2,z2）
double y2 = end.y;
double z2 = end.z;
double t = ((x1-x0)*(x1-x2)+(y1-y0)*(y1-y2)+(z1-z0)*(z1-z2))
      /((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
if (0<=t&&t<=1)//垂足Q点落在线段AB上
{
double X = x1+t*(x2-x1);
double Y = y1+t*(y2-y1);
double Z = z1+t*(z2-z1);
CVector3D Q(X,Y,Z);
distance = (Q-center).Length();
}
if (t<0) //垂足Q点不落在线段AB上，而是落在BA的延长线上
{
distance = (star-center).Length();
}
if (t>1) //垂足Q点不落在线段AB上，而是落在AB的延长线上
{
distance = (end-center).Length();
}
return distance;
}

double LineDistence::DistanceLineToLine( CVector3D p1, CVector3D p2, CVector3D p3, CVector3D p4 )
	{
		double distance;
		double x1 = p1.x;//A点坐标（x1,y1,z1）
		double y1 = p1.y;
		double z1 = p1.z;
		double x2 = p2.x; //B点坐标（x2,y2,z2）
		double y2 = p2.y;
		double z2 = p2.z;
		double x3 = p3.x; //C点坐标（x3,y3,z3）
		double y3 = p3.y;
		double z3 = p3.z;
		double x4 = p4.x; //D点坐标（x4,y4,z4）
		double y4 = p4.y;
		double z4 = p4.z;

		double a = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1);
		double b = -((x2-x1)*(x4-x3)+(y2-y1)*(y4-y3)+(z2-z1)*(z4-z3));
		double c = -((x1-x2)*(x1-x3)+(y1-y2)*(y1-y3)+(z1-z2)*(z1-z3));

		double d = -((x2-x1)*(x4-x3)+(y2-y1)*(y4-y3)+(z2-z1)*(z4-z3));
		double e = (x4-x3)*(x4-x3)+(y4-y3)*(y4-y3)+(z4-z3)*(z4-z3);
		double f = -((x1-x3)*(x4-x3)+(y1-y3)*(y4-y3)+(z1-z3)*(z4-z3));

		if ((a*e-b*d)==0&&(b*d-a*e)==0) //平行
		{
		   // tempVector01 = p1-p2;
			double d1 = (p1-p3).Length();
			double d2 = (p1-p4).Length();
			distance = (d1<d2)?d1:d2;
			return distance;
		}

		double s = (b*f-e*c)/(a*e-b*d);
		double t = (a*f-d*c)/(b*d-a*e);

		if(0<=s&&s<=1&&0<=t&&t<=1) //说明P点落在线段AB上,Q点落在线段CD上
		{
			//2条线段的公垂线段PQ;
			//P点坐标
			double X = x1+s*(x2-x1);
			double Y = y1+s*(y2-y1);
			double Z = z1+s*(z2-z1);
			//Q点坐标
			double U = x3+t*(x4-x3);
			double V = y3+t*(y4-y3);
			double W = z3+t*(z4-z3);
			CVector3D P(X,Y,Z);
			CVector3D Q(U,V,W);
			distance = (P-Q).Length();
		}
		else
		{
			double d1 = DistancePointToLine(p3,p4,p1);
			double d2 = DistancePointToLine(p3,p4,p2);
			double d3 = DistancePointToLine(p1,p2,p3);
			double d4 = DistancePointToLine(p1,p2,p4);
			distance = (d1<d2)?d1:d2;
			distance = (distance<d3)?distance:d3;
			distance = (distance<d4)?distance:d4;
		}
		return distance;
	}
