/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SFG/Math/Transformation.hpp"
#include "SFG/Math/Matrix4x4.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Data/OStream.hpp"
#include "SFG/Data/IStream.hpp"

namespace SFG
{
	Transformation Transformation::Interpolate(const Transformation& from, const Transformation& to, float t)
	{
		return Transformation(Math::Lerp(from.m_position, to.m_position, t), Quaternion::Slerp(from.m_rotation, to.m_rotation, t), Math::Lerp(from.m_scale, to.m_scale, t));
	}

	void Transformation::SaveToStream(OStream& stream) const
	{
		stream << m_position << m_rotation << m_scale;
		stream << m_localPosition << m_localRotation << m_localScale;
		stream << m_localRotationAngles << m_rotationAngles;
	}

	void Transformation::LoadFromStream(IStream& stream)
	{
		stream >> m_position >> m_rotation >> m_scale;
		stream >> m_localPosition >> m_localRotation >> m_localScale;
		stream >> m_localRotationAngles >> m_rotationAngles;
	}

	Matrix4x4 Transformation::ToMatrix() const
	{
		return Matrix4x4::TransformMatrix(m_position, m_rotation, m_scale);
	}

	Matrix4x4 Transformation::ToLocalMatrix() const
	{
		return Matrix4x4::TransformMatrix(m_localPosition, m_localRotation, m_localScale);
	}
} // namespace SFG
