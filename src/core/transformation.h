#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_

#include "hsl_def.h"

#include <iostream>
#include "../core/geometry.h"


namespace  hsl {

	struct Matrix4x4 {
		// Matrix4x4 Public Methods
		Matrix4x4() 
		{
			m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
			m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] =
			m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.f;
		}
		Matrix4x4(Float mat[4][4]);
		Matrix4x4(Float t00, Float t01, Float t02, Float t03, Float t10, Float t11,
			Float t12, Float t13, Float t20, Float t21, Float t22, Float t23,
			Float t30, Float t31, Float t32, Float t33);

		bool operator==(const Matrix4x4 &m2) const {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (m[i][j] != m2.m[i][j]) return false;
			return true;
		}
		bool operator!=(const Matrix4x4 &m2) const {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (m[i][j] != m2.m[i][j]) return true;
			return false;
		}

		void Transpose() 
		{
			assert(false);
		}

		Matrix4x4 Transposed()const 
		{
			assert(false);
			Matrix4x4 mat;
			return mat;
		}

		void Inverse() {
			assert(false);
		}

		Matrix4x4 Inversed()const {
			assert(false);
			Matrix4x4 mat;
			return mat;
		}

		friend Matrix4x4 Transpose(const Matrix4x4 &);

		friend Matrix4x4 Inverse(const Matrix4x4 & inv);


		static Matrix4x4 Mul(const Matrix4x4 &m1, const Matrix4x4 &m2) {
			Matrix4x4 r;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					r.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] +
					m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
			return r;
		}


		friend std::ostream &operator<<(std::ostream &os, const Matrix4x4 &m) 
		{
			os << "[" <<
				m.m[0][0] << "," << m.m[0][1] << "," << m.m[0][2] << "," << m.m[0][3] << "]\n" <<
				m.m[1][0] << "," << m.m[1][1] << "," << m.m[1][2] << "," << m.m[1][3] << "]\n" <<
				m.m[2][0] << "," << m.m[2][1] << "," << m.m[2][2] << "," << m.m[2][3] << "]\n" <<
				m.m[3][0] << "," << m.m[3][1] << "," << m.m[3][2] << "," << m.m[3][3] << "]\n";
			return os;
		}

		Float m[4][4];
	};


	class Transform {
	public:
		// Transform Public Methods
		Transform() = default;

		explicit Transform(const Float mat[4][4]) 
		{
			m_m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0],
					mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1],
					mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2],
					mat[3][3]);
			m_inv = m_m.Inversed();
		}

		Transform(const Matrix4x4 & m,const Matrix4x4 & inv):m_m(m),m_inv(inv) 
		{	
		}


		explicit Transform(const Matrix4x4 & m):m_m(m),m_inv(Inverse(m)) 
		{	
		}


		Transform Transposed()const
		{
			return { m_inv, m_m };
		}

		void Transpose() 
		{
			m_m.Inverse();
			m_inv.Inverse();
		}

		bool operator==(const Transform &t) const 
		{
			return t.m_m == m_m;
		}

		bool operator!=(const Transform &t) const 
		{
			return !(*this == t);
		}

		bool IsIdentity() const 
		{
			return (m_m.m[0][0] == 1.f && m_m.m[0][1] == 0.f && m_m.m[0][2] == 0.f &&
				m_m.m[0][3] == 0.f && m_m.m[1][0] == 0.f && m_m.m[1][1] == 1.f &&
				m_m.m[1][2] == 0.f && m_m.m[1][3] == 0.f && m_m.m[2][0] == 0.f &&
				m_m.m[2][1] == 0.f && m_m.m[2][2] == 1.f && m_m.m[2][3] == 0.f &&
				m_m.m[3][0] == 0.f && m_m.m[3][1] == 0.f && m_m.m[3][2] == 0.f &&
				m_m.m[3][3] == 1.f);
		}

		const Matrix4x4 & Matrix() const { return m_m; }

		template <typename T>
		 Point3<T> operator()(const Point3<T> &p) const;

		template <typename T>
		Vector3<T> operator()(const Vector3<T> &v) const;

		template<typename T>
		Point3<T> operator*(const Point3<T> & p)const;

		template<typename T>
		Vector3<T> operator*(const Vector3<T> & v)const;



		friend std::ostream &operator<<(std::ostream &os, const Transform & t)
		{
			os << "t = " << t.m_m << " t' = " << t.m_inv;
			return os;
		}


	private:
		Matrix4x4 m_m;
		Matrix4x4 m_inv;
	};

	template<typename T>
	inline Point3<T> Transform::operator()(const Point3<T> & p) const
	{
		return (*this)*p;
	}

	template<typename T>
	inline Vector3<T> Transform::operator()(const Vector3<T> & v) const
	{
		return (*this)*v;
	}

	template<typename T>
	inline Point3<T> Transform::operator*(const Point3<T>& p) const
	{
		T x = p.x, y = p.y, z = p.z;
		T xp = m_m.m[0][0] * x + m_m.m[0][1] * y + m_m.m[0][2] * z + m_m.m[0][3];
		T yp = m_m.m[1][0] * x + m_m.m[1][1] * y + m_m.m[1][2] * z + m_m.m[1][3];
		T zp = m_m.m[2][0] * x + m_m.m[2][1] * y + m_m.m[2][2] * z + m_m.m[2][3];
		T wp = m_m.m[3][0] * x + m_m.m[3][1] * y + m_m.m[3][2] * z + m_m.m[3][3];

		if (wp == 1)
			return Point3<T>(xp, yp, zp);
		else
			return Point3<T>(xp, yp, zp) / wp;
	}

	template<typename T>
	inline Vector3<T> Transform::operator*(const Vector3<T>& v) const
	{
		T x = v.x, y = v.y, z = v.z;
		return Vector3<T>(m_m.m[0][0] * x + m_m.m[0][1] * y + m_m.m[0][2] * z,
			m_m.m[1][0] * x + m_m.m[1][1] * y + m_m.m[1][2] * z,
			m_m.m[2][0] * x + m_m.m[2][1] * y + m_m.m[2][2] * z);
	}
}
#endif
