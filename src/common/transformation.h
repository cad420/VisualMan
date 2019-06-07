#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_
#include <iostream>

#include "geometry.h"
#include "numeric.h"

#include "common.h"


namespace  ysl
{
	struct Matrix4x4;

	struct Matrix3x3;

	//template<typename Ty, int Col,int Row>
	//struct GenericMatrix
	//{
	//	Float m[Row][Col];

	//};

	struct COMMON_EXPORT_IMPORT Matrix3x3
	{
		typedef	Float(*Matrix3x3DataType)[3];
		Float m[3][3];
		Matrix3x3();
		Matrix3x3(Float mat[3][3]);
		Matrix3x3(Float t00, Float t01, Float t02, Float t10, Float t11, Float t12, Float t20, Float t21, Float t22);
		Matrix3x3(const Matrix4x4 & mat);
		Matrix3x3(Float mat[4][4]);
		bool operator==(const Matrix3x3& m2) const;
		bool operator!=(const Matrix3x3& m2) const;
		Matrix3x3& operator/=(const Float& s);
		Matrix3x3& operator*=(const Float& s);

		void SetToIdentity();

		void Transpose();
		Matrix3x3 Transposed() const;
		Float Det() const;
		Float * FlatData() { return *m; }
		const Float * FlatData()const { return *m; }
		Matrix3x3DataType MatrixData() { return m; }
		void Inverse() { *this = Inversed(); }
		Matrix3x3 Inversed() const;
	};

	struct COMMON_EXPORT_IMPORT Matrix4x4
	{
		typedef	Float(*Matrix4x4DataType)[4];
		Matrix4x4();
		Matrix4x4(const Matrix3x3 & mat33);
		Matrix4x4(Float mat[4][4]);
		Matrix4x4(Float t00, Float t01, Float t02, Float t03, Float t10, Float t11,
			Float t12, Float t13, Float t20, Float t21, Float t22, Float t23,
			Float t30, Float t31, Float t32, Float t33);


		bool operator==(const Matrix4x4& m2) const;
		bool operator!=(const Matrix4x4& m2) const;

		void SetToIdentity();

		void Transpose();
		Matrix4x4 Transposed() const;

		Matrix3x3 NormalMatrix() const;
		void Inverse() { *this = Inversed(); }
		Matrix4x4 Inversed() const;

		static Matrix4x4 Mul(const Matrix4x4& m1, const Matrix4x4& m2);
		friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& m);

		Float * FlatData() { return *m; }
		const Float * FlatData()const { return *m; }
		Matrix4x4DataType MatrixData() { return m; }
		Float m[4][4];
	};

	inline
		Matrix4x4
		operator*(const Matrix4x4 & m1, const Matrix4x4 & m2)
	{
		return Matrix4x4::Mul(m1, m2);
	}



	class COMMON_EXPORT_IMPORT  Transform
	{
	public:
		// Transform Public Methods
		Transform() = default;

		explicit Transform(const Float mat[4][4]);
		Transform(const Matrix4x4& m, const Matrix4x4& inv);
		explicit Transform(const Matrix4x4& m);
		Transform Transposed() const;
		void Transpose();
		bool operator==(const Transform& t) const;
		bool operator!=(const Transform& t) const;
		bool IsIdentity() const;
		Transform Inversed()const { return Transform{ m_inv,m_m }; }
		const Matrix4x4& Matrix() const;
		const Matrix4x4& InverseMatrix() const;

		void SetLookAt(const Point3f & eye, const Point3f & center, const Vector3f & up);
		void SetGLOrtho(Float left, Float right, Float bottom, Float top, Float nearPlane, Float farPlane);
		void SetGLPerspective(Float vertcialAngle, Float aspectRation, Float nearPlane, Float farPlane);
		void SetFrustum(Float left, Float right, Float bottom, Float top, Float nearPlane, Float farPlane);
		void SetTranslate(const Vector3f& t);
		void SetTranslate(Float x, Float y, Float z);
		void SetTranslate(Float * t);
		void SetScale(const Vector3f & s);
		void SetScale(Float x, Float y, Float z);
		void SetScale(Float * s);
		void SetRotate(const Vector3f & axis, Float degrees);
		void SetRotate(Float x, Float y, Float z, Float degrees);
		void SetRotate(Float *a, Float degrees);
		void SetRotateX(Float degrees);
		void SetRotateY(Float degrees);
		void SetRotateZ(Float degrees);
		void SetIdentity();

		//const Float * ConstMatrixData()const;
		//Float * MatrixData();
		//const Float * ConstInverseMatrixData()const;
		//Float* InverseMatrixData();

		Matrix4x4 ColumnMajorMatrix() const;
		Transform operator*(const Transform & trans)const;
		template<typename T> Point3<T> operator*(const Point3<T> & p)const;
		template<typename T> Vector3<T> operator*(const Vector3<T> & v)const;

		Ray operator*(const Ray & ray)const;
		Bound3f operator*(const Bound3f & aabb)const;

		friend std::ostream &operator<<(std::ostream &os, const Transform & t)
		{
			os << "t = " << t.m_m << " t' = " << t.m_inv;
			return os;
		}

	private:
		Matrix4x4 m_m;
		Matrix4x4 m_inv;
	};



	template<typename T> inline
		Point3<T>
		Transform::operator*(const Point3<T> & p) const
	{
		const auto x = p[0], y = p[1], z = p[2];
		const auto rx = m_m.m[0][0] * x + m_m.m[0][1] * y + m_m.m[0][2] * z + m_m.m[0][3];
		const auto ry = m_m.m[1][0] * x + m_m.m[1][1] * y + m_m.m[1][2] * z + m_m.m[1][3];
		const auto rz = m_m.m[2][0] * x + m_m.m[2][1] * y + m_m.m[2][2] * z + m_m.m[2][3];
		const auto rw = m_m.m[3][0] * x + m_m.m[3][1] * y + m_m.m[3][2] * z + m_m.m[3][3];

		if (rw == 1)return Point3<T>{rx, ry, rz};
		return Point3<T>{ rx, ry, rz } / rw;
	}

	template<typename T> inline
		Vector3<T>
		Transform::operator*(const Vector3<T>& v) const
	{
		const auto x = v[0], y = v[1], z = v[2];
		const auto rx = m_m.m[0][0] * x + m_m.m[0][1] * y + m_m.m[0][2] * z;
		const auto ry = m_m.m[1][0] * x + m_m.m[1][1] * y + m_m.m[1][2] * z;
		const auto rz = m_m.m[2][0] * x + m_m.m[2][1] * y + m_m.m[2][2] * z;
		return Vector3<T>{rx, ry, rz};
	}

	inline Transform Scale(Float x,Float y,Float z)
	{
		Transform t;
		t.SetScale(x, y, z);
		return t;
	}
	inline Transform Scale(Float *v)
	{
		Transform t;
		t.SetScale(v);
		return t;
	}
	inline Transform Scale(const Vector3f & s)
	{
		Transform t;
		t.SetScale(s);
		return t;
	}

	inline Transform Translate(Float x,Float y,Float z)
	{
		Transform t;
		t.SetTranslate(x, y, z);
		return t;
	}
	inline Transform Translate(Float *v)
	{
		Transform t;
		t.SetTranslate(v);
		return t;
	}
	inline Transform Translate(const Vector3f & v)
	{
		Transform t;
		t.SetTranslate(v);
		return t;
	}


	inline Transform Rotate(Float x, Float y, Float z, Float degrees)
	{
		Transform t;
		t.SetRotate(x,y,z,degrees);
		return t;
	}
	inline Transform Rotate(Float * v, Float degrees)
	{
		Transform t;
		t.SetRotate(v, degrees);
		return t;
	}
	inline Transform Rotate(const Vector3f & axis, Float degrees)
	{
		Transform t;
		t.SetRotate(axis, degrees);
		return t;
	}

	inline Transform RotateX(Float degrees)
	{
		Transform t;
		t.SetRotateX(degrees);
		return t;
	}
	inline Transform RotateY(Float degrees)
	{
		Transform t;
		t.SetRotateY( degrees);
		return t;
	}
	inline Transform RotateZ(Float degrees)
	{
		Transform t;
		t.SetRotateZ(degrees);
		return t;
	}

	inline 
	Transform 
	LookAt(const Point3f & eye, const Point3f & center, const Vector3f & up)
	{
		Transform t;
		t.SetLookAt(eye, center, up);
		return t;
	}
	inline 
	Transform 
	Ortho(Float left, Float right, Float bottom, Float top, Float nearPlane, Float farPlane)
	{
		Transform t;
		t.SetGLOrtho(left, right, bottom, top, nearPlane, farPlane);
		return t;
	}
	inline 
	Transform 
	Perspective(Float vertcialAngle, Float aspectRation, Float nearPlane, Float farPlane)
	{
		Transform t;
		t.SetGLPerspective(vertcialAngle, aspectRation, nearPlane, farPlane);
		return t;
	}

	inline 
	Transform 
	Orthographic(Float zNear,Float zFar)
	{
		return Scale(1,1,1.0/(zFar-zNear)) * Translate(0,0,-zNear);
	}

	inline 
	Transform 
	Perspective(Float zNear,Float zFar,Float fov)
	{
		Matrix4x4 persp(1.0,0,0,0,
						0,1,0,0,
						0,0,zFar/(zFar-zNear),-zFar*zNear/(zFar-zNear),
						0,0,1,0);
		Float cot = 1 / (std::tan(DegreesToRadians(fov) / 2));
		return Scale(cot, cot, 1.0)*Transform(persp);
	}




}



#endif
