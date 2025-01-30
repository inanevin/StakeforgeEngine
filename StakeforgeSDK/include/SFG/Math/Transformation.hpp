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

#pragma once

#include "SFG/Math/Quaternion.hpp"
#include "SFG/Math/Vector3.hpp"

namespace SFG
{
	class Matrix4x4;

	class Transformation
	{
	public:
		Transformation() : m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f)
		{
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="translationIn"></param>
		Transformation(const Vector3& translationIn) : m_position(translationIn), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f)
		{
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="rotationIn"></param>
		Transformation(const Quaternion& rotationIn) : m_position(0.0f, 0.0f, 0.0f), m_rotation(rotationIn), m_scale(1.0f, 1.0f, 1.0f)
		{
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="translationIn"></param>
		/// <param name="rotationIn"></param>
		/// <param name="scaleIn"></param>
		Transformation(const Vector3& translationIn, const Quaternion& rotationIn, const Vector3& scaleIn) : m_position(translationIn), m_rotation(rotationIn), m_scale(scaleIn)
		{
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="from"></param>
		/// <param name="to"></param>
		/// <param name="t"></param>
		/// <returns></returns>
		static Transformation Interpolate(const Transformation& from, const Transformation& to, float t);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Matrix4x4 ToMatrix() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Matrix4x4 ToLocalMatrix() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="stream"></param>
		void SaveToStream(OStream& stream) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="stream"></param>
		void LoadFromStream(IStream& stream);

		/// <summary>
		///
		/// </summary>
		/// <param name="v"></param>
		inline void SetPosition(const Vector3& v)
		{
			m_position = v;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="v"></param>
		inline void SetLocalPosition(const Vector3& v)
		{
			m_localPosition = v;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="v"></param>
		inline void SetScale(const Vector3& v)
		{
			m_scale = v;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="v"></param>
		inline void SetLocalScale(const Vector3& v)
		{
			m_localScale = v;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="q"></param>
		inline void SetRotation(const Quaternion& q)
		{
			m_rotation = q;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="q"></param>
		inline void SetLocalRotation(const Quaternion& q)
		{
			m_localRotation = q;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="v"></param>
		inline void SetRotationAngles(const Vector3& v)
		{
			m_rotationAngles = v;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="v"></param>
		inline void SetLocalRotationAngles(const Vector3& v)
		{
			m_localRotationAngles = v;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector3& GetPosition() const
		{
			return m_position;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector3& GetLocalPosition() const
		{
			return m_localPosition;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector3& GetScale() const
		{
			return m_scale;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector3& GetLocalScale() const
		{
			return m_localScale;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Quaternion& GetRotation() const
		{
			return m_rotation;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Quaternion& GetLocalRotation() const
		{
			return m_localRotation;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector3& GetRotationAngles() const
		{
			return m_rotationAngles;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector3& GetLocalRotationAngles() const
		{
			return m_localRotationAngles;
		}

	private:
		Vector3	   m_position = Vector3::Zero;
		Quaternion m_rotation;
		Vector3	   m_rotationAngles = Vector3::Zero;
		Vector3	   m_scale			= Vector3::One;
		Vector3	   m_localPosition	= Vector3::Zero;
		Quaternion m_localRotation;
		Vector3	   m_localScale			 = Vector3::One;
		Vector3	   m_localRotationAngles = Vector3::Zero;
	};

} // namespace SFG
