/*
	3次元の行列を扱う。
	2002.04.25	まっちゃ	初版

	３次元同次座標の行列
			| m[0]  m[1]  m[2]  m[3]  |
			| m[4]  m[5]  m[6]  m[7]  |
			| m[8]  m[9]  m[10] m[11] |
			| m[12] m[13] m[14] m[15] |

	基本スタイル v → v' の変換
  		| x'|   | m[0]  m[1]  m[2]  m[3]  |   | x |
		| y'| = | m[4]  m[5]  m[6]  m[7]  | * | y |
		| z'|   | m[8]  m[9]  m[10] m[11] |   | z |
		| w'|   | m[12] m[13] m[14] m[15] |   | w |
*/

#pragma once
#include "Vector3D.h"

class CMatrix3D
{
public:
	double m[16];

public:
	CMatrix3D(void) {
		ZeroMemory(m, 16*sizeof(double));
	};
	CMatrix3D(const CMatrix3D & rhs){
		memcpy(m, rhs.m, 16*sizeof(double));
	};
	CMatrix3D(const double * rhs){
		memcpy(m, rhs, 16*sizeof(double));
	};
	CMatrix3D(	double e0, double e1, double e2, double e3,
		double e4, double e5, double e6, double e7,
		double e8, double e9, double e10, double e11,
		double e12, double e13, double e14, double e15)
	{
		m[0]=e0;	m[1]=e1;	m[2]=e2;	m[3]=e3;
		m[4]=e4;	m[5]=e5;	m[6]=e6;	m[7]=e7;
		m[8]=e8;	m[9]=e9;	m[10]=e10;	m[11]=e11;
		m[12]=e12;	m[13]=e13;	m[14]=e14;	m[15]=e15;
	};
	~CMatrix3D(void) {};

	//cast to pointer to a (double *) for glGetdoublev etc
	operator double* () const {return (double *) this;}

	CMatrix3D& LoadIdentity() {
		ZeroMemory(m, 16*sizeof(double));
		m[0] = m[5] = m[10] = m[15] = 1.0;
		return *this;
	};

	CMatrix3D& Set(double *nm) {
		CopyMemory(m, nm, 16*sizeof(double));
		return *this;
	};

	void Get(double *nm) {
		CopyMemory(nm, m, 16*sizeof(double));
	};

	CMatrix3D glMatrix()   // 返回一个按列存储的矩阵
	{
		CMatrix3D _m;
		_m[0] = m[0];		_m[1] = m[4]; 		_m[2] = m[8];			_m[3] = m[12];
		_m[4] = m[1];		_m[5] = m[5]; 		_m[6] = m[9];			_m[7] = m[13];
		_m[8] = m[2];		_m[9] = m[6]; 		_m[10] = m[10];		_m[11] = m[14];
		_m[12] = m[3];	_m[13] = m[7]; 	_m[14] = m[11];		_m[15] = m[15];

		return _m;
	}

	CMatrix3D operator*(CMatrix3D& m3d) {// (*this) * m3d
		CMatrix3D _m;
		for(int i=0; i<4; ++i) {
			for(int j=0; j<4; ++j) {
				for(int k=0; k<4; ++k) {
					_m.m[4*i+j] += m[4*i+k] * m3d.m[4*k+j];
				}
			}
		}
		return _m;
	};

	CMatrix3D glMultiply(CMatrix3D& m3d) {
		CMatrix3D _m;
		for(int i=0; i<4; ++i) {
			for(int j=0; j<4; ++j) {
				for(int k=0; k<4; ++k) {
					_m.m[4*i+j] += m[4*k+j] * m3d.m[4*i+k];
				}
			}
		}
		return _m;
	};

	CMatrix3D& operator*=(CMatrix3D& m3d) {		// (*this) *= m3d
		CMatrix3D _m;
		for(int i=0; i<4; ++i) {
			for(int j=0; j<4; ++j) {
				for(int k=0; k<4; ++k) {
					_m.m[4*i+j] += m[4*i+k] * m3d.m[4*k+j];
				}
			}
		}
		CopyMemory(m, _m.m, 16*sizeof(double));
		return *this;
	};

	CVector3D operator*(CVector3D& v) {		// (*this) * v^T
		CVector3D _v;
		double scale;
		scale = v.x * m[12] + v.y * m[13] + v.z * m[14] + m[15];
		_v.x = v.x * m[0] + v.y * m[1] + v.z * m[2] + m[3];
		_v.y = v.x * m[4] + v.y * m[5] + v.z * m[6] + m[7];
		_v.z = v.x * m[8] + v.y * m[9] + v.z * m[10] + m[11];
		return _v/scale;
	};

	CVector3D Affin(double x, double y, double z) {	// アフィン変換の適用
		CVector3D _v;
		_v.x = x*m[0] + y*m[1] + z*m[2] + m[3];
		_v.y = x*m[4] + y*m[5] + z*m[6] + m[7];
		_v.z = x*m[8] + y*m[9] + z*m[10] + m[11];
		return _v;
	};

	CVector3D Affin(CVector3D& v3d) {				// アフィン変換の適用
		CVector3D _v;
		_v.x = v3d.x*m[0] + v3d.y*m[1] + v3d.z*m[2] + m[3];
		_v.y = v3d.x*m[4] + v3d.y*m[5] + v3d.z*m[6] + m[7];
		_v.z = v3d.x*m[8] + v3d.y*m[9] + v3d.z*m[10] + m[11];
		return _v;
	};
	CMatrix3D& Scale(double x, double y, double z) {	// 拡大縮小の適用
		CMatrix3D _m;
		_m.LoadIdentity();
		_m.m[0] = x;
		_m.m[5] = y;
		_m.m[10] = z;

		*this *= _m;
		return *this;
	};
	CMatrix3D& Translate(double x, double y, double z) {	// 平行移動の適用
		CMatrix3D _m;
		_m.LoadIdentity();
		_m.m[3] = x;
		_m.m[7] = y;
		_m.m[11] = z;
		*this *= _m;
		return *this;
	};
	CMatrix3D& Translate(CVector3D& v) {	// 平行移動の適用
		return Translate(v.x, v.y, v.z);
	};
	CMatrix3D& Rotate_x(double r) {	// 回転
		CMatrix3D _m;
		double c = cos(r);
		double s = sin(r);
		_m.LoadIdentity();
		_m.m[5] = c;
		_m.m[6] = s;
		_m.m[9] = -s;
		_m.m[10] = c;
		*this *= _m;
		return *this;
	};
	CMatrix3D& Rotate_y(double r) {
		CMatrix3D _m;
		double c = cos(r);
		double s = sin(r);
		_m.LoadIdentity();
		_m.m[0] = c;
		_m.m[2] = -s;
		_m.m[8] = s;
		_m.m[10] = c;
		*this *= _m;
		return *this;
	};
	CMatrix3D& Rotate_z(double r) {
		CMatrix3D _m;
		double c = cos(r);
		double s = sin(r);
		_m.LoadIdentity();
		_m.m[0] = c;
		_m.m[1] = s;
		_m.m[4] = -s;
		_m.m[5] = c;
		*this *= _m;
		return *this;
	};
	CMatrix3D& Rotate(double r, double x, double y, double z) {
		CMatrix3D _m;
		double c = cos(r);
		double s = sin(r);
		_m.m[0] = c + (1-c)*x*x;
		_m.m[1] = (1-c)*x*y + z*s;
		_m.m[2] = (1-c)*x*z - y*s;
		_m.m[4] = (1-c)*x*y - z*s;
		_m.m[5] = c + (1-c)*y*y;
		_m.m[6] = (1-c)*y*z + x*s;
		_m.m[8] = (1-c)*x*z + y*s;
		_m.m[9] = (1-c)*y*z - x*s;
		_m.m[10] = c + (1-c)*z*z;
		_m.m[15] = 1.0;
		*this *= _m;
		return *this;
	};
	CMatrix3D& Rotate(double r, CVector3D& v) {
		return Rotate(r, v.x, v.y, v.z);
	};
};
