// Copyright (c) 2025 Inan Evin
#include "descriptions.hpp"

#ifdef SFG_TOOLMODE
#include "io/assert.hpp"
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace SFG
{
#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const sampler_desc& s)
	{
		j["anisotropy"] = s.anisotropy;
		j["min_lod"]	= s.min_lod;
		j["max_lod"]	= s.max_lod;
		j["lod_bias"]	= s.lod_bias;

		// --- min filter ---
		if (s.flags.is_set(sampler_flags::saf_min_anisotropic))
			j["min"] = "anisotropic";
		else if (s.flags.is_set(sampler_flags::saf_min_linear))
			j["min"] = "linear";
		else if (s.flags.is_set(sampler_flags::saf_min_nearest))
			j["min"] = "nearest";

		// --- mag filter ---
		if (s.flags.is_set(sampler_flags::saf_mag_anisotropic))
			j["mag"] = "anisotropic";
		else if (s.flags.is_set(sampler_flags::saf_mag_linear))
			j["mag"] = "linear";
		else if (s.flags.is_set(sampler_flags::saf_mag_nearest))
			j["mag"] = "nearest";

		// --- mip filter ---
		if (s.flags.is_set(sampler_flags::saf_mip_nearest))
			j["mip"] = "nearest";
		else if (s.flags.is_set(sampler_flags::saf_mip_linear))
			j["mip"] = "linear";

		// --- address mode ---
		if (s.flags.is_set(sampler_flags::saf_address_mode_border))
			j["address_mode"] = "border";
		else if (s.flags.is_set(sampler_flags::saf_address_mode_clamp))
			j["address_mode"] = "clamp";
		else if (s.flags.is_set(sampler_flags::saf_address_mode_mirrored_clamp))
			j["address_mode"] = "mirrored_clamp";
		else if (s.flags.is_set(sampler_flags::saf_address_mode_mirrored_repeat))
			j["address_mode"] = "mirrored_repeat";
		else if (s.flags.is_set(sampler_flags::saf_address_mode_repeat))
			j["address_mode"] = "repeat";

		// --- border color ---
		if (s.flags.is_set(sampler_flags::saf_border_transparent))
			j["border"] = "transparent";
		else if (s.flags.is_set(sampler_flags::saf_border_white))
			j["border"] = "white";
	}

	void from_json(const nlohmann::json& j, sampler_desc& s)
	{
		s.anisotropy = j.value<uint32>("anisotropy", 0);
		s.min_lod	 = j.value<float>("min_lod", .0f);
		s.max_lod	 = j.value<float>("max_lod", .0f);
		s.lod_bias	 = j.value<float>("lod_bias", .0f);
		s.flags		 = j.value<uint16>("flags", 0);

		const string min		  = j.value<string>("min", "anisotropic");
		const string mag		  = j.value<string>("mag", "anisotropic");
		const string mip		  = j.value<string>("mip", "linear");
		const string address_mode = j.value<string>("address_mode", "clamp");
		const string border		  = j.value<string>("border", "transparent");

		if (min.compare("anisotropic") == 0)
			s.flags.set(sampler_flags::saf_min_anisotropic);
		else if (min.compare("linear") == 0)
			s.flags.set(sampler_flags::saf_min_linear);
		else if (min.compare("nearest") == 0)
			s.flags.set(sampler_flags::saf_min_nearest);

		if (mag.compare("anisotropic") == 0)
			s.flags.set(sampler_flags::saf_mag_anisotropic);
		else if (mag.compare("linear") == 0)
			s.flags.set(sampler_flags::saf_mag_linear);
		else if (mag.compare("nearest") == 0)
			s.flags.set(sampler_flags::saf_mag_nearest);

		if (mip.compare("nearest") == 0)
			s.flags.set(sampler_flags::saf_mip_nearest);
		else if (mip.compare("linear") == 0)
			s.flags.set(sampler_flags::saf_mip_linear);

		if (address_mode.compare("border") == 0)
			s.flags.set(sampler_flags::saf_address_mode_border);
		else if (address_mode.compare("clamp") == 0)
			s.flags.set(sampler_flags::saf_address_mode_clamp);
		else if (address_mode.compare("mirrored_clamp") == 0)
			s.flags.set(sampler_flags::saf_address_mode_mirrored_clamp);
		else if (address_mode.compare("mirrored_repeat") == 0)
			s.flags.set(sampler_flags::saf_address_mode_mirrored_repeat);
		else if (address_mode.compare("repeat") == 0)
			s.flags.set(sampler_flags::saf_address_mode_repeat);

		if (border.compare("transparent") == 0)
			s.flags.set(sampler_flags::saf_border_transparent);
		else if (border.compare("white") == 0)
			s.flags.set(sampler_flags::saf_border_white);
	}

#endif

}