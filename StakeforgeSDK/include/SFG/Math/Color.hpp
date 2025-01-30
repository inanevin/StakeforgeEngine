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

#include "SFG/Data/String.hpp"

namespace SFG
{
	class Vector4;
	class OStream;
	class IStream;

	class Color
	{

	public:
		Color(float rv = 1.0f, float gv = 1.0f, float bv = 1.0f, float av = 1.0f) : x(rv), y(gv), z(bv), w(av) {};

		/// <summary>
		///
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		/// <param name="a"></param>
		/// <returns></returns>
		static Color Color255(float r, float g, float b, float a);

		/// <summary>
		///
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		/// <param name="a"></param>
		/// <returns></returns>
		static Color ColorLinear(float r, float g, float b, float a);

		/// <summary>
		///
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		/// <param name=""></param>
		/// <returns></returns>
		static Color Color255Linear(float r, float g, float b, float);

		/// <summary>
		///
		/// </summary>
		/// <param name="hex"></param>
		static Color FromHex(const String& hex);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static String ToHex(const Color& color);

		/// <summary>
		/// Lerps to white by amt.
		/// </summary>
		/// <param name="amt"></param>
		/// <returns></returns>
		Color Brighten(float amt) const;

		/// <summary>
		/// Lerps to black by amt.
		/// </summary>
		/// <returns></returns>
		Color Darken(float amt) const;

		/// <summary>
		/// Calculate angle from hue-saturation and conver
		/// </summary>
		/// <returns></returns>
		Color HS2SRGB() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Color SRGB2HSV() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Color HSV2SRGB() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Color SRGB2Linear() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Color Linear2SRGB() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector4 ToVector() const;

		/// <summary>
		/// Round each component.
		/// </summary>
		void Round();

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

		bool operator!=(const Color& rhs) const
		{
			return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}

		bool operator==(const Color& rhs) const
		{
			return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}

		Color operator*(const float& rhs) const
		{
			return Color(x * rhs, y * rhs, z * rhs, w * rhs);
		}

		Color operator+(const Color& rhs) const
		{
			return Color(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		Color operator/(float v) const
		{
			return Color(x / v, y / v, z / v, w / v);
		}

		Color operator/=(float v) const
		{
			return Color(x / v, y / v, z / v, w / v);
		}

		Color operator*=(float v) const
		{
			return Color(x * v, y * v, z * v, w * v);
		}

		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}

		static Color Red;
		static Color Green;
		static Color LightBlue;
		static Color Blue;
		static Color DarkBlue;
		static Color Cyan;
		static Color Yellow;
		static Color Black;
		static Color White;
		static Color Purple;
		static Color Maroon;
		static Color Beige;
		static Color Brown;
		static Color Gray;

		float x, y, z, w = 1.0f;
	};

	struct ColorGrad
	{
		ColorGrad() {};
		ColorGrad(const Color& col)
		{
			start = col;
			end	  = col;
		}

		ColorGrad(const Color& colStart, const Color& colEnd)
		{
			start = colStart;
			end	  = colEnd;
		}

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

		Color start = Color::White;
		Color end	= Color::White;
	};

} // namespace SFG
