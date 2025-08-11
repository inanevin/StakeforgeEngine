// Copyright (c) 2025 Inan Evin

#include "vector3.hpp"
#include "math.hpp"

namespace SFG
{
	const vector3 vector3::zero(0.0f, 0.0f, 0.0f);
	const vector3 vector3::one(1.0f, 1.0f, 1.0f);
	const vector3 vector3::up(0.0f, 1.0f, 0.0f);
	const vector3 vector3::forward(0.0f, 0.0f, 1.0f);
	const vector3 vector3::right(1.0f, 0.0f, 0.0f);

	vector3 vector3::clamp(const vector3& vector, const vector3& min_vec, const vector3& max_vec)
	{
		return vector3(math::clamp(vector.x, min_vec.x, max_vec.x), math::clamp(vector.y, min_vec.y, max_vec.y), math::clamp(vector.z, min_vec.z, max_vec.z));
	}

	vector3 vector3::cross(const vector3& a, const vector3& b)
	{
		return vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	vector3 vector3::abs(const vector3& vector)
	{
		return vector3(math::abs(vector.x), math::abs(vector.y), math::abs(vector.z));
	}

	vector3 vector3::min(const vector3& a, const vector3& b)
	{
		return vector3(math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z));
	}

	vector3 vector3::max(const vector3& a, const vector3& b)
	{
		return vector3(math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z));
	}

	float vector3::dot(const vector3& a, const vector3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	float vector3::distance(const vector3& a, const vector3& b)
	{
		return (a - b).magnitude();
	}

	float vector3::distance_sqr(const vector3& a, const vector3& b)
	{
		return (a - b).magnitude_sqr();
	}

	vector3 vector3::project(const vector3& on_normal) const
	{
		return on_normal * dot(*this, on_normal);
	}

	vector3 vector3::rotate(const vector3& axis, float angle_degrees) const
	{
		vector3 unit_axis = axis.normalized();
		if (unit_axis.is_zero())
		{
			return *this;
		}

		float angle_rad = math::degrees_to_radians(angle_degrees);
		float cos_theta = math::cos(angle_rad);
		float sin_theta = math::sin(angle_rad);

		vector3 v_rot = (*this * cos_theta) + (vector3::cross(unit_axis, *this) * sin_theta) + (unit_axis * (vector3::dot(unit_axis, *this) * (1.0f - cos_theta)));
		return v_rot;
	}

	vector3 vector3::reflect(const vector3& in_normal) const
	{
		vector3 unit_normal = in_normal.normalized();
		if (unit_normal.is_zero())
		{
			return -(*this);
		}
		return *this - (unit_normal * (2.0f * vector3::dot(*this, unit_normal)));
	}

	bool vector3::equals(const vector3& other, float epsilon) const
	{
		return math::almost_equal(x, other.x, epsilon) && math::almost_equal(y, other.y, epsilon) && math::almost_equal(z, other.z, epsilon);
	}

	bool vector3::is_zero(float epsilon) const
	{
		return math::almost_equal(x, 0.0f, epsilon) && math::almost_equal(y, 0.0f, epsilon) && math::almost_equal(z, 0.0f, epsilon);
	}

	float vector3::magnitude() const
	{
		return math::sqrt(x * x + y * y + z * z);
	}

	float vector3::magnitude_sqr() const
	{
		return x * x + y * y + z * z;
	}

}