#include "transformation.h"
#include "numeric.h"
#include <cstring>


namespace ysl
{

	Matrix3x3::Matrix3x3()
	{
		m[0][0] = m[1][1] = m[2][3] = 1.0f;
		m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1] = 0.f;
	}

	Matrix3x3::Matrix3x3(Float mat[3][3])
	{
		std::memcpy(m, mat, sizeof(Float) * 9);
	}

	Matrix3x3::Matrix3x3(Float t00, Float t01, Float t02, Float t10, Float t11, Float t12, Float t20, Float t21,
		Float t22)
	{
		m[0][0] = t00;
		m[0][1] = t01;
		m[0][2] = t02;
		m[1][0] = t10;
		m[1][1] = t11;
		m[1][2] = t12;
		m[2][0] = t20;
		m[2][1] = t21;
		m[2][2] = t22;
	}

	Matrix3x3::Matrix3x3(const Matrix4x4& mat)
	{
		m[0][0] = mat.m[0][0];
		m[0][1] = mat.m[0][1];
		m[0][2] = mat.m[0][2];
		m[1][0] = mat.m[1][0];
		m[1][1] = mat.m[1][1];
		m[1][2] = mat.m[1][2];
		m[2][0] = mat.m[2][0];
		m[2][1] = mat.m[2][1];
		m[2][2] = mat.m[2][2];
	}

	Matrix3x3::Matrix3x3(Float mat[4][4])
	{
		m[0][0] = mat[0][0];
		m[0][1] = mat[0][1];
		m[0][2] = mat[0][2];
		m[1][0] = mat[1][0];
		m[1][1] = mat[1][1];
		m[1][2] = mat[1][2];
		m[2][0] = mat[2][0];
		m[2][1] = mat[2][1];
		m[2][2] = mat[2][2];
	}

	bool Matrix3x3::operator==(const Matrix3x3& m2) const
	{
		for (auto i = 0; i < 3; ++i)
			for (auto j = 0; j < 3; ++j)
				if (m[i][j] != m2.m[i][j]) return false;
		return true;
	}

	bool Matrix3x3::operator!=(const Matrix3x3& m2) const
	{
		for (auto i = 0; i < 3; ++i)
			for (auto j = 0; j < 3; ++j)
				if (m[i][j] != m2.m[i][j]) return true;
		return false;
	}

	Matrix3x3& Matrix3x3::operator/=(const Float& s)
	{
		const auto inv = 1.0 / s;
		(*this) *= inv;
		return *this;
	}

	Matrix3x3& Matrix3x3::operator*=(const Float& s)
	{
		for (auto i = 0; i < 3; ++i)
			for (auto j = 0; j < 3; ++j)
				m[i][j] *= s;
		return *this;
	}

	void Matrix3x3::Transpose()
	{
		for (auto i = 0; i < 3; i++)
		{
			for (auto j = 0; j < 3; j++)
			{
				const auto t = m[i][j];
				m[i][j] = m[j][i];
				m[j][i] = t;
			}
		}
	}

	Matrix3x3 Matrix3x3::Transposed() const
	{
		Matrix3x3 mat
		{
			m[0][0], m[1][0], m[2][0],
			m[0][1], m[1][1], m[2][1],
			m[0][2], m[1][2], m[2][2]
		};
		return mat;
	}

	Float Matrix3x3::Det() const
	{
		return m[0][0] * m[1][1] * m[2][2] +
			m[0][1] * m[1][2] * m[2][0]
			+ m[0][2] * m[1][0] * m[2][1]
			- m[0][0] * m[1][2] * m[2][1]
			- m[0][1] * m[1][0] * m[2][2]
			- m[0][2] * m[1][1] * m[2][0];
	}

	Matrix3x3 Matrix3x3::Inversed() const
	{
		const auto det = Det();
		if (std::fabs(det) <= 0.00001)
		{
			return Matrix3x3{};
		}
		Matrix3x3 mat
		{
			m[1][1] * m[2][2] - m[2][1] * m[1][2], -(m[0][1] * m[2][2] - m[2][1] * m[0][2]),
			m[0][1] * m[1][2] - m[0][2] * m[1][1],
			m[1][2] * m[2][0] - m[2][2] * m[1][0], -(m[0][2] * m[2][0] - m[2][2] * m[0][0]),
			m[0][2] * m[1][0] - m[0][0] * m[1][2],
			m[1][0] * m[2][1] - m[2][0] * m[1][1], -(m[0][0] * m[2][1] - m[2][0] * m[0][1]),
			m[0][0] * m[1][1] - m[0][1] * m[1][0]
		};
		mat /= det;
		return mat;
	}

	Matrix4x4::Matrix4x4()
	{
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
		m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] =
			m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.f;
	}

	Matrix4x4::Matrix4x4(Float mat[4][4])
	{
		std::memcpy(m, mat, sizeof(Float) * 16);
	}

	Matrix4x4::Matrix4x4(Float t00, Float t01, Float t02, Float t03, Float t10, Float t11, Float t12, Float t13,
		Float t20, Float t21, Float t22, Float t23, Float t30, Float t31, Float t32, Float t33)
	{
		m[0][0] = t00;
		m[0][1] = t01;
		m[0][2] = t02;
		m[0][3] = t03;
		m[1][0] = t10;
		m[1][1] = t11;
		m[1][2] = t12;
		m[1][3] = t13;
		m[2][0] = t20;
		m[2][1] = t21;
		m[2][2] = t22;
		m[2][3] = t23;
		m[3][0] = t30;
		m[3][1] = t31;
		m[3][2] = t32;
		m[3][3] = t33;
	}

	bool Matrix4x4::operator==(const Matrix4x4& m2) const
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m[i][j] != m2.m[i][j]) return false;
		return true;
	}

	bool Matrix4x4::operator!=(const Matrix4x4& m2) const
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m[i][j] != m2.m[i][j]) return true;
		return false;
	}

	void Matrix4x4::Transpose()
	{
		for (auto i = 0; i < 3; i++)
		{
			for (auto j = 0; j < 3; j++)
			{
				const auto t = m[i][j];
				m[i][j] = m[j][i];
				m[j][i] = t;
			}
		}
	}

	Matrix4x4 Matrix4x4::Transposed() const
	{
		Matrix4x4 mat(m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);
		return mat;
	}

	Matrix3x3 Matrix4x4::NormalMatrix() const
	{
		Matrix3x3 mat3x3{ Inversed().Transposed() };
		return mat3x3;
	}


	Matrix4x4 Matrix4x4::Inversed() const
	{
		int indxc[4], indxr[4];
		int ipiv[4] = { 0, 0, 0, 0 };
		Float minv[4][4];
		std::memcpy(minv, this->m, 4 * 4 * sizeof(Float));
		for (int i = 0; i < 4; i++)
		{
			int irow = 0, icol = 0;
			Float big = 0.f;
			// Choose pivot
			for (int j = 0; j < 4; j++)
			{
				if (ipiv[j] != 1)
				{
					for (int k = 0; k < 4; k++)
					{
						if (ipiv[k] == 0)
						{
							if (std::abs(minv[j][k]) >= big)
							{
								big = Float(std::abs(minv[j][k]));
								irow = j;
								icol = k;
							}
						}
						else if (ipiv[k] > 1)
							assert(false);
					}
				}
			}
			++ipiv[icol];
			// Swap rows _irow_ and _icol_ for pivot
			if (irow != icol)
			{
				for (int k = 0; k < 4; ++k) std::swap(minv[irow][k], minv[icol][k]);
			}
			indxr[i] = irow;
			indxc[i] = icol;
			if (minv[icol][icol] == 0.f)
				assert(false);

			// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
			Float pivinv = 1. / minv[icol][icol];
			minv[icol][icol] = 1.;
			for (int j = 0; j < 4; j++) minv[icol][j] *= pivinv;

			// Subtract this row from others to zero out their columns
			for (int j = 0; j < 4; j++)
			{
				if (j != icol)
				{
					Float save = minv[j][icol];
					minv[j][icol] = 0;
					for (int k = 0; k < 4; k++) minv[j][k] -= minv[icol][k] * save;
				}
			}
		}
		// Swap columns to reflect permutation
		for (int j = 3; j >= 0; j--)
		{
			if (indxr[j] != indxc[j])
			{
				for (int k = 0; k < 4; k++)
					std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
			}
		}
		return Matrix4x4{ minv };
	}

	Matrix4x4 Matrix4x4::Mul(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		Matrix4x4 r;
		for (auto i = 0; i < 4; ++i)
			for (auto j = 0; j < 4; ++j)
				r.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] +
				m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		return r;
	}

	Transform::Transform(const Float mat[4][4])
	{
		m_m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0],
			mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1],
			mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2],
			mat[3][3]);
		m_inv = m_m.Inversed();
	}

	Transform::Transform(const Matrix4x4& m, const Matrix4x4& inv) : m_m(m), m_inv(inv)
	{
	}

	Transform::Transform(const Matrix4x4& m) : m_m(m), m_inv(m.Inversed())
	{
	}

	std::ostream& operator<<(std::ostream& os, const Matrix4x4& m)
	{
		os << "[" <<
			m.m[0][0] << ", " << m.m[0][1] << ", " << m.m[0][2] << ", " << m.m[0][3] << "]\n" <<
			m.m[1][0] << ", " << m.m[1][1] << ", " << m.m[1][2] << ", " << m.m[1][3] << "]\n" <<
			m.m[2][0] << ", " << m.m[2][1] << ", " << m.m[2][2] << ", " << m.m[2][3] << "]\n" <<
			m.m[3][0] << ", " << m.m[3][1] << ", " << m.m[3][2] << ", " << m.m[3][3] << "]\n";
		return os;
	}


	Transform
		Transform::Transposed() const
	{
		return { m_inv.Transposed(), m_m.Transposed() };
	}


	void
		Transform::Transpose()
	{
		m_inv.Transpose();
		m_m.Transposed();
	}


	bool
		Transform::operator==(const Transform& t) const
	{
		return t.m_m == m_m;
	}


	bool
		Transform::operator!=(const Transform& t) const
	{
		return !(*this == t);
	}

	Ray
	Transform::operator*(const Ray& ray) const
	{
		return { (*this)*ray.Direction() ,(*this)*ray.Original() };
	}

	Bound3f
		Transform::operator*(const Bound3f & aabb) const
	{
		assert(false);
		return Bound3f{};
	}

	Transform
		Transform::operator*(const Transform & trans)const
	{
		return { this->m_m*trans.m_m,trans.m_inv*this->m_inv };
	}


	bool
		Transform::IsIdentity() const
	{
		return (m_m.m[0][0] == 1.f && m_m.m[0][1] == 0.f && m_m.m[0][2] == 0.f &&
			m_m.m[0][3] == 0.f && m_m.m[1][0] == 0.f && m_m.m[1][1] == 1.f &&
			m_m.m[1][2] == 0.f && m_m.m[1][3] == 0.f && m_m.m[2][0] == 0.f &&
			m_m.m[2][1] == 0.f && m_m.m[2][2] == 1.f && m_m.m[2][3] == 0.f &&
			m_m.m[3][0] == 0.f && m_m.m[3][1] == 0.f && m_m.m[3][2] == 0.f &&
			m_m.m[3][3] == 1.f);
	}

	const Matrix4x4&
		Transform::Matrix() const
	{
		return m_m;
	}

	const Matrix4x4&
		Transform::InverseMatrix() const
	{
		return m_inv;
	}

	void
		Transform::SetLookAt(const Point3f& eye, const Point3f& center, const Vector3f& up)
	{
		const auto direction = (center - eye).Normalized();
		const auto right = Vector3f::Cross(direction, up).Normalized();
		const auto newUp = Vector3f::Cross(right, direction).Normalized();

		m_inv = Matrix4x4
		{
			right.x,newUp.x,-direction.x,eye.x,
			right.y,newUp.y,-direction.y,eye.y,
			right.z,newUp.z,-direction.z,eye.z,
			0.f,0.f,0.f,1.0f
		};

		// In right-hand coordinates system, the direction of direction components is different from left-hand coordinates system.
		m_m = m_inv.Inversed();
	}

	void
		Transform::SetGLOrtho(Float left, Float right, Float bottom, Float top, Float nearPlane, Float farPlane)
	{
		const auto width = right - left;
		const auto height = top - bottom;
		const auto clip = farPlane - nearPlane;
		if (width == 0.0 || height == 0.0 || clip == 0.0)
		{
			return;
		}

		m_m = Matrix4x4{
			2.0f / width,0.0f,0.0f,-(right + left) / width,
			0.0f,2.0f / height,0.0f,-(top + bottom) / height,
			0.0f,0.0f,-2.0f / clip,-(farPlane + nearPlane) / clip,
			0.0f,0.0f,0.0f,1.0f
		};
		m_inv = m_m.Inversed();
	}

	void
		Transform::SetGLPerspective(Float vertcialAngle, Float aspectRation, Float nearPlane, Float farPlane)
	{
		const auto top = std::tan(DegreesToRadians(vertcialAngle / 2))*nearPlane;
		const auto right = top * aspectRation;
		SetFrustum(-right, right, top, -top, nearPlane, farPlane);
	}

	void
		Transform::SetFrustum(Float left, Float right, Float bottom, Float top, Float nearPlane, Float farPlane)
	{
		const auto width = right - left;
		const auto height = top - bottom;
		const auto clip = farPlane - nearPlane;

		if (width == 0.0 || height == 0.0 || clip == 0.0)
		{
			return;
		}

		m_m = Matrix4x4
		{
			2.f * nearPlane / width,0.0f,(right + left) / width,0.0f,
			0.0f,2.f * nearPlane / height,(top + bottom) / height,0.0f,
			0.0f,0.0f,-(farPlane + nearPlane) / clip,-2.f * farPlane*nearPlane / clip,
			0.0f,0.0f,-1.0f,0.0f
		};
		m_inv = m_m.Inversed();
	}


	void
		Transform::SetTranslate(const Vector3f& t)
	{
		SetTranslate(t[0], t[1], t[2]);
	}

	void
		Transform::SetTranslate(Float x, Float y, Float z)
	{
		m_m = Matrix4x4{ 1.f,0.f,0.f,x,
				   0.f,1.f,0.f,y,
				   0.f,0.f,1.f,z,
				   0.f,0.f,0.f,1.f };
		m_inv = Matrix4x4{ 1.f, 0.f, 0.f, -x,
		   0.f, 1.f, 0.f, -y,
		   0.f, 0.f, 1.f, -z,
		   0.f, 0.f, 0.f, 1.f };
	}

	void
		Transform::SetTranslate(Float* t)
	{
		SetTranslate(t[0], t[1], t[2]);
	}

	void
		Transform::SetScale(const Vector3f& s)
	{
		SetScale(s[0], s[1], s[2]);
	}

	void
		Transform::SetScale(Float x, Float y, Float z)
	{
		m_m = Matrix4x4
		{
		   x,0.f,0.f,0.f,
		   0.f,y,0.f,0.f,
		   0.f,0.f,z,0.f,
		   0.f,0.f,0.f,1.f
		};
		m_inv = Matrix4x4
		{
			1 / x, 0.f, 0.f, 0.f,
		   0.f, 1 / y, 0.f, 0.f,
		   0.f, 0.f, 1 / z, 0.f,
		   0.f, 0.f, 0.f, 1.f
		};
	}

	void
		Transform::SetScale(Float* s)
	{
		SetScale(s[0], s[1], s[2]);
	}

	void
		Transform::SetRotate(const Vector3f & axis, Float degrees)
	{
		SetRotate(axis[0], axis[1], axis[2], degrees);
	}

	void
		Transform::SetRotate(Float x, Float y, Float z, Float degrees)
	{
		//YSL_ASSERT_X(false, "Transform::SetRotate", "Not implemented yet.");
		Vector3f axis = { x,y,z };
		Vector3f a = axis.Normalized();
		const auto sinTheta = std::sin(DegreesToRadians(degrees));
		const auto cosTheta = std::cos(DegreesToRadians(degrees));
		Matrix4x4 m;
		// Compute rotation of first basis vector
		m.m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
		m.m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
		m.m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
		m.m[0][3] = 0;

		// Compute rotations of second and third basis vectors
		m.m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
		m.m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
		m.m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
		m.m[1][3] = 0;

		m.m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
		m.m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
		m.m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
		m.m[2][3] = 0;

		m_m = m;
		m_inv = m.Inversed();
	}

	void
		Transform::SetRotate(Float* a, Float degrees)
	{
		SetRotate(a[0], a[1], a[2], degrees);
	}

	void
		Transform::SetRotateX(Float degrees)
	{
		const auto radians = DegreesToRadians(degrees);
		const auto sinTheta = std::sin(radians);
		const auto cosTheta = std::cos(radians);

		m_m = Matrix4x4
		{
		   1.f,0.f,0.f,0.f,
		   0.f,cosTheta,-sinTheta,0.f,
		   0.f,sinTheta,cosTheta,0.f,
		   0.f,0.f,0.f,1.f
		};
		m_inv = m_m.Inversed();

	}

	void
		Transform::SetRotateY(Float degrees)
	{
		const auto radians = DegreesToRadians(degrees);
		const auto sinTheta = std::sin(radians);
		const auto cosTheta = std::cos(radians);

		m_m = Matrix4x4
		{
		   cosTheta,0.f,-sinTheta,0.f,
		   0.f,1.f,0.f,0.f,
		   sinTheta,0.f,cosTheta,0.f,
		   0.f,0.f,0.f,1.f
		};
		m_inv = m_m.Inversed();
	}

	void
		Transform::SetRotateZ(Float degrees)
	{
		const auto radians = DegreesToRadians(degrees);
		const auto sinTheta = std::sin(radians);
		const auto cosTheta = std::cos(radians);

		m_m = Matrix4x4
		{
		   cosTheta,-sinTheta,0.f,0.f,
		   sinTheta,cosTheta,0.f,0.f,
		   0.f,0.f,1.f,0.f,
		   0.f,0.f,0.f,1.f
		};

		m_inv = m_m.Inversed();
	}

	Matrix4x4
		Transform::ColumnMajorMatrix() const
	{
		return m_m.Transposed();
	}
}
