// Copyright (c) 2025 Inan Evin

#pragma once

namespace SFG
{
	class easing
	{
	public:
		static float smooth_damp(float current, float target, float* currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
		static float lerp(float val1, float val2, float amt);
		static float cubic_lerp(float val1, float val2, float amt);
		static float cubic_interp(float val0, float val1, float val2, float val3, float amt);
		static float cubic_interp_tangents(float val1, float tan1, float val2, float tan2, float amt);
		static float bilerp(float val00, float val10, float val01, float val11, float amtX, float amtY);
		static float step(float edge, float x);
		static float ease_in(float start, float end, float alpha);
		static float ease_out(float start, float end, float alpha);
		static float ease_in_out(float start, float end, float alpha);
		static float cubic(float start, float end, float alpha);
		static float exponential(float start, float end, float alpha);
		static float bounce(float start, float end, float alpha);
		static float sinusodial(float start, float end, float alpha);
	};
} // namespace SFG
