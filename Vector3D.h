/*
	3次元のベクトルを扱う。
	2002.04.25	まっちゃ	初版
*/

#pragma once

#include <windows.h>
#include <GL/glut.h>
#include <cmath>

class CVector3D
{
	enum VECTOR3D_ERROR {
		VECTOR3D_NO_ERROR = 0,				// エラーなし。通常これ。
		VECTOR3D_ZERO_DIVIDE = 1,			// 除数がゼロ。
		VECTOR3D_PROCESSED_EXCEPTION = 2,	// 代入元等が例外処理されたベクトルである。
		VECTOR3D_TOO_SHORT = 4				// ベクトルが短すぎて長さor角度が求まらない
	};

public:
	double x, y, z;
	int m_error;
public:
	CVector3D(void) {
		x = y = z = 0.0;
		m_error = VECTOR3D_NO_ERROR;
	};
	CVector3D(double x, double y, double z) {
		this->x = x; this->y = y; this->z = z;
		m_error = VECTOR3D_NO_ERROR;
	};
	CVector3D(const CVector3D& v) {		// コピーコンストラクタ
		x = v.x; y = v.y; z = v.z;
		m_error = v.m_error;
	};
	~CVector3D(void) {};

	void Set(double x, double y, double z) {	// 初期化と同等。
		this->x = x; this->y = y; this->z = z;
		m_error = VECTOR3D_NO_ERROR;
	};

	bool Less(const CVector3D& v)
	{
		if (this->x<v.x && this->y<v.y && this->z<v.z)
			return true;
		else
			return false;
	};

	bool NotLess(const CVector3D& v)
	{
		if (this->x>=v.x && this->y>=v.y && this->z>=v.z)
			return true;
		else
			return false;
	};

	bool IsEqual(const CVector3D& v)
	{
		if (this->x==v.x && this->y==v.y && this->z==v.z)
			return true;
		else
			return false;
	};

	void Set(const CVector3D& v) {			// コピーコンストラクタと同等。
		x = v.x; y = v.y; z = v.z;
		m_error = v.m_error;
	};
	void Get(float *v) {					// エラーがあったらゼロ配列を返す。
		if(m_error == VECTOR3D_NO_ERROR) {
			v[0] = x; v[1] = y; v[2] = z;
		} else {
			v[0] = 0.0; v[1] = 0.0; v[2] = 0.0;
		}
	};
	void Get(double *v) {						// エラーがあったらゼロ配列を返す。
		if(m_error == VECTOR3D_NO_ERROR) {
			v[0] = (double)x; v[1] = (double)y; v[2] = (double)z;
		} else {
			v[0] = 0.0; v[1] = 0.0; v[2] = 0.0;
		}
	};

	//cast to pointer to a (float *) for glGetfloatv etc
	//operator float* () {m[0]=x; m[1]=y; m[2]=z; return m;}
	operator double* () const {return (double*) this;}

	CVector3D& operator = (const CVector3D& v) {	// コピーコンストラクタと同等。
		x=v.x; y=v.y; z=v.z;
		m_error = v.m_error;
		return *this;
	};
	CVector3D  operator + (CVector3D& v) {
		CVector3D _v(x+v.x, y+v.y, z+v.z);
		_v.m_error = m_error | v.m_error;
		return _v;
	};
	CVector3D& operator +=(CVector3D& v) {
		x += v.x; y += v.y; z += v.z;
		m_error |= v.m_error;
		return *this;
	};
	CVector3D  operator - (CVector3D& v) {
		CVector3D _v(x-v.x, y-v.y, z-v.z);
		_v.m_error = m_error | v.m_error;
		return _v;
	};
	CVector3D& operator -=(CVector3D& v) {
		x -= v.x; y -= v.y; z -= v.z;
		m_error |= v.m_error;
		return *this;
	};
	CVector3D  operator * (double a) {
		CVector3D _v(x*a, y*a, z*a);
		_v.m_error = m_error;
		return _v;
	};
	CVector3D& operator *=(double a) {
		x *= a; y *= a; z *= a;
		return *this;
	};
	CVector3D  operator / (double a) {
		CVector3D _v;
		if(a==0.0) {
			_v.Set(0.0, 0.0, 0.0);
			_v.m_error = VECTOR3D_ZERO_DIVIDE;
			return _v;
		}
		_v.Set(x/a, y/a, z/a);
		_v.m_error = m_error;
		return _v;
	};
	CVector3D& operator /=(double a) {
		if(a==0.0) {
			Set(0.0, 0.0, 0.0);
			m_error = VECTOR3D_ZERO_DIVIDE;
			return *this;
		}
		x/=a; y/=a; z/=a;
		return *this;
	};

	CVector3D& Translate(CVector3D& v, double t) { // 方向（単位ベクトル）と距離で移動
		x += v.x * t;
		y += v.y * t;
		z += v.z * t;
		m_error |= v.m_error;
		return *this;
	};

	CVector3D& Minus()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	double Length() {
		if(m_error) return 0.0;
		else return sqrt(x*x+y*y+z*z);
	};

	double Inner(CVector3D& v) {
		if(m_error) return 0.0;
		else return x*v.x + y*v.y + z*v.z;
	};

	CVector3D Outer(CVector3D& v) {   //求的就是叉积，其结果是一个向量
		CVector3D _v(	y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		_v.m_error = m_error | v.m_error;
		if(_v.m_error) {
			_v.Set(0.0, 0.0, 0.0);
			return _v;
		} else return _v;
	};

	CVector3D GetTriangleNormal(CVector3D& v1, CVector3D& v2) {
		CVector3D _v, dir1, dir2;
		dir1 = v1-*this; dir2 = v2-*this;
		_v = dir1.Outer(dir2);
		_v.Normalize();
		if(_v.m_error) {
			_v.Set(0.0, 0.0, 0.0);
			return _v;
		} else return _v;
	};

	CVector3D Normal() {
		CVector3D _v;
		double w = Length();
		if(m_error) {
			_v.Set(0.0, 0.0, 0.0);
			m_error = VECTOR3D_PROCESSED_EXCEPTION;
			return _v;
		}
		if(w==0.0) {
			_v.Set(0.0, 0.0, 0.0);
			m_error = VECTOR3D_ZERO_DIVIDE;
			return _v;
		}
		_v.Set(x/w, y/w, z/w);
		_v.m_error = m_error;
		return _v;
	};

	CVector3D& Normalize() {
		double w = Length();
		if(m_error) {
			Set(0.0, 0.0, 0.0);
			m_error = VECTOR3D_PROCESSED_EXCEPTION;
			return *this;
		}
		if(w == 0.0) {
			Set(0.0, 0.0, 0.0);
			m_error = VECTOR3D_ZERO_DIVIDE;
			return *this;
		}
		x/=w; y/=w; z/=w;
		return *this;
	};

	double Angle(CVector3D& v) {	// 自分と指定ベクトルとのなす角を返す。何かあった時は0.0を返す。
		if( m_error || v.m_error ) {
			return 0.0;
		}
		if( fabs(v.Length()) < 1.0e-10 || fabs(this->Length()) < 1.0e-10 ) {
			return 0.0;
		}
		CVector3D _v = v.Normal();
		CVector3D _this = Normal();
		double val = _v.Inner(_this);
		if(val < -1.0) val = -1.0;
		if(val >  1.0) val =  1.0;
		return acos(val);
	};

	bool IsNoError() {	// エラーが無ければtrue。あればfalseを返す。
		if(m_error == VECTOR3D_NO_ERROR) return true;
		else return false;
	};

	bool IsError() {		// エラーがあればtrue。無ければfalseを返す。
		if(m_error != VECTOR3D_NO_ERROR) return true;
		else return false;
	};
};

