// Copyright (c) 2025 Inan Evin

#include "vector4.hpp"
#include "math.hpp"
#include "data/istream.hpp"
#include "data/ostream.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json.hpp"
#endif
namespace SFG
{
	const vector4 vector4::zero(0.0f, 0.0f, 0.0f, 0.0f);
	const vector4 vector4::one(1.0f, 1.0f, 1.0f, 1.0f);

	vector4 vector4::clamp(const vector4& vector, const vector4& min_vec, const vector4& max_vec)
	{
		return vector4(math::clamp(vector.x, min_vec.x, max_vec.x), math::clamp(vector.y, min_vec.y, max_vec.y), math::clamp(vector.z, min_vec.z, max_vec.z), math::clamp(vector.w, min_vec.w, max_vec.w));
	}

	vector4 vector4::abs(const vector4& vector)
	{
		return vector4(math::abs(vector.x), math::abs(vector.y), math::abs(vector.z), math::abs(vector.w));
	}

	vector4 vector4::min(const vector4& a, const vector4& b)
	{
		return vector4(math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z), math::min(a.w, b.w));
	}

	vector4 vector4::max(const vector4& a, const vector4& b)
	{
		return vector4(math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z), math::max(a.w, b.w));
	}

	float vector4::dot(const vector4& a, const vector4& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	float vector4::distance(const vector4& a, const vector4& b)
	{
		return (a - b).magnitude();
	}

	vector4 vector4::project(const vector4& on_normal) const
	{
		vector4 unit_normal = on_normal.normalized();
		if (unit_normal.is_zero())
		{
			return vector4::zero;
		}
		return unit_normal * dot(*this, unit_normal);
	}

	vector4 vector4::rotate(const vector4& axis, float angle_degrees) const
	{
		vector4 unit_axis = axis.normalized();
		if (unit_axis.is_zero())
		{
			return *this;
		}

		float angle_rad = math::degrees_to_radians(angle_degrees);
		float cos_theta = math::cos(angle_rad);
		float sin_theta = math::sin(angle_rad);

		vector4 v_xyz = vector4(x, y, z, 0.0f);
		vector4 k_xyz = vector4(unit_axis.x, unit_axis.y, unit_axis.z, 0.0f);

		vector4 cross_kv = vector4(k_xyz.y * v_xyz.z - k_xyz.z * v_xyz.y, k_xyz.z * v_xyz.x - k_xyz.x * v_xyz.z, k_xyz.x * v_xyz.y - k_xyz.y * v_xyz.x, 0.0f);

		float dot_kv = dot(k_xyz, v_xyz);

		vector4 rotated_xyz = (v_xyz * cos_theta) + (cross_kv * sin_theta) + (k_xyz * (dot_kv * (1.0f - cos_theta)));

		return vector4(rotated_xyz.x, rotated_xyz.y, rotated_xyz.z, w);
	}
	float vector4::magnitude() const
	{
		return math::sqrt(x * x + y * y + z * z + w * w);
	}

	float vector4::magnitude_sqr() const
	{
		return x * x + y * y + z * z + w * w;
	}

	vector4 vector4::operator/(float scalar) const
	{
		if (math::abs(scalar) < MATH_EPS)
			return vector4::zero;
		return vector4(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	vector4& vector4::operator/=(float scalar)
	{
		if (math::abs(scalar) > MATH_EPS)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			w /= scalar;
		}
		else
		{
			x = y = z = w = MATH_INF_F;
		}
		return *this;
	}

	bool vector4::equals(const vector4& other, float epsilon) const
	{
		return math::almost_equal(x, other.x, epsilon) && math::almost_equal(y, other.y, epsilon) && math::almost_equal(z, other.z, epsilon) && math::almost_equal(w, other.w, epsilon);
	}

	bool vector4::is_zero(float epsilon) const
	{
		return math::almost_equal(x, 0.0f, epsilon) && math::almost_equal(y, 0.0f, epsilon) && math::almost_equal(z, 0.0f, epsilon) && math::almost_equal(w, 0.0f, epsilon);
	}

	void vector4::serialize(ostream& stream) const
	{
		stream << x << y << z << w;
	}

	void vector4::deserialize(istream& stream)
	{
		stream >> x >> y >> z >> w;
	}

#ifdef SFG_TOOLMODE
	void to_json(nlohmann::json& j, const vector4& v)
	{
		j["x"] = v.x;
		j["y"] = v.y;
		j["z"] = v.z;
		j["w"] = v.w;
	}

	void from_json(const nlohmann::json& j, vector4& v)
	{
		v.x = j.value<float>("x", 0.0f);
		v.y = j.value<float>("y", 0.0f);
		v.z = j.value<float>("z", 0.0f);
		v.w = j.value<float>("w", 0.0f);
	}

#endif
}