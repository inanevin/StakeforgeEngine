// Copyright (c) 2025 Inan Evin

#pragma once

namespace SFG
{
	class ostream;
	class istream;

	class compressor
	{
	public:
		static ostream compress(ostream& stream);
		static istream decompress(istream& stream);
	};
};
