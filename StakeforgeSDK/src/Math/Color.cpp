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

#include "SFG/Math/Color.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Math/Vector4.hpp"
#include "SFG/Data/OStream.hpp"
#include "SFG/Data/IStream.hpp"

namespace SFG
{

	Color Color::Red	   = Color(1, 0, 0, 1);
	Color Color::Green	   = Color(0, 1, 0);
	Color Color::LightBlue = Color(0.4f, 0.4f, 0.8f);
	Color Color::Blue	   = Color(0, 0, 1);
	Color Color::DarkBlue  = Color(0.1f, 0.1f, 0.6f);
	Color Color::Cyan	   = Color(0, 1, 1);
	Color Color::Yellow	   = Color(1, 1, 0);
	Color Color::Black	   = Color(0, 0, 0);
	Color Color::White	   = Color(1, 1, 1);
	Color Color::Purple	   = Color(1, 0, 1);
	Color Color::Maroon	   = Color(0.5f, 0, 0);
	Color Color::Beige	   = Color(0.96f, 0.96f, 0.862f);
	Color Color::Brown	   = Color(0.647f, 0.164f, 0.164f);
	Color Color::Gray	   = Color(0.5f, 0.5f, 0.5f);

	Color Color::Color255(float r, float g, float b, float a)
	{
		return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	void Color::Round()
	{
		x = Math::RoundToFloat(x);
		y = Math::RoundToFloat(y);
		z = Math::RoundToFloat(z);
		w = Math::RoundToFloat(w);
	}

	void Color::SaveToStream(OStream& stream) const
	{
		stream << x << y << z << w;
	}

	void Color::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z >> w;
	}

	Vector4 Color::ToVector() const
	{
		return Vector4(x, y, z, w);
	}

	void ColorGrad::SaveToStream(OStream& stream) const
	{
		stream << start << end;
	}

	void ColorGrad::LoadFromStream(IStream& stream)
	{
		stream >> start >> end;
	}

} // namespace SFG
