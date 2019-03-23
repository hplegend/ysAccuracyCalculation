#ifndef LINEDISTENCE_H_INCLUDED
#define LINEDISTENCE_H_INCLUDED

#include "vector3D.h"

class LineDistence
{
public:
   static double DistancePointToLine(CVector3D star, CVector3D end,CVector3D center );
//p1 p2 AB  line01  p3 p4 CD line02
   static double DistanceLineToLine( CVector3D p1, CVector3D p2, CVector3D p3, CVector3D p4 );

};





#endif // LINEDISTENCE_H_INCLUDED
