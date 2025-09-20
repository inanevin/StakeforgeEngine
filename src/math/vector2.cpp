// Copyright (c) 2025 Inan Evin

#include "vector2.hpp"
#include "math.hpp"
#include "data/istream.hpp"
#include "data/ostream.hpp"
#include <limits>

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json.hpp"
#endif

namespace SFG
{
	vector2 vector2::zero = vector2(0.f, 0.f);
	vector2 vector2::one  = vector2(1.f, 1.f);

	vector2 vector2::clamp(const vector2& vector, const vector2& min_vec, const vector2& max_vec)
	{
		return vector2(math::max(min_vec.x, math::min(vector.x, max_vec.x)), math::max(min_vec.y, math::min(vector.y, max_vec.y)));
	}

	vector2 vector2::clamp_magnitude(const vector2& vector, float max_length)
	{
		float mag = vector.magnitude();
		if (mag > max_length)
		{
			return vector.normalized() * max_length;
		}
		return vector;
	}

	vector2 vector2::abs(const vector2& vector)
	{
		return vector2(math::abs(vector.x), math::abs(vector.y));
	}

	vector2 vector2::min(const vector2& a, const vector2& b)
	{
		return vector2(math::min(a.x, b.x), math::min(a.y, b.y));
	}

	vector2 vector2::max(const vector2& a, const vector2& b)
	{
		return vector2(math::max(a.x, b.x), math::max(a.y, b.y));
	}

	float vector2::dot(const vector2& a, const vector2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	float vector2::distance(const vector2& a, const vector2& b)
	{
		return (a - b).magnitude();
	}

	float vector2::angle(const vector2& a, const vector2& b)
	{
		float dot_product = dot(a, b);
		float magnitudes  = a.magnitude() * b.magnitude();

		if (magnitudes == 0.0f)
			return 0.0f;

		float cos_angle = dot_product / magnitudes;
		cos_angle		= math::max(-1.0f, math::min(1.0f, cos_angle));

		return math::cos(cos_angle) * (180.0f / MATH_PI);
	}

	vector2 vector2::normalized() const
	{
		float mag = magnitude();
		if (mag > MATH_EPS)
		{
			return vector2(x / mag, y / mag);
		}
		return vector2::zero;
	}

	bool vector2::equals(const vector2& other, float epsilon) const
	{
		return math::abs(x - other.x) < epsilon && math::abs(y - other.y) < epsilon;
	}

	bool vector2::is_zero(float epsilon) const
	{
		return math::abs(x) < epsilon && math::abs(y) < epsilon;
	}

	float vector2::magnitude() const
	{
		return math::sqrt(x * x + y * y);
	}

	float vector2::magnitude_sqr() const
	{
		return x * x + y * y;
	}

	void vector2::serialize(ostream& stream) const
	{
		stream << x << y;
	}

	void vector2::deserialize(istream& stream)
	{
		stream >> x >> y;
	}

#ifdef SFG_TOOLMODE
	void to_json(nlohmann::json& j, const vector2& v)
	{
		j["x"] = v.x;
		j["y"] = v.y;
	}

	void from_json(const nlohmann::json& j, vector2& v)
	{
		v.x = j.value<float>("x", 0.0f);
		v.y = j.value<float>("y", 0.0f);
	}

#endif
}