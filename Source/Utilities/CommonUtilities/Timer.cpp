
//#include "pch.h"
#include "Timer.h"

#include <chrono>
#include <cassert>

const float minFPS = 1.f / 240.f;

namespace CommonUtilities
{
	Timer::Timer()
	{
		thisFrame = clock.now();
		lastFrame = clock.now();

		totalTimeTimer = clock.now();
		startTime = clock.now();

		duration = totalTimeTimer - startTime;

		timeScale = 1;
		deltaTime = 0;
		totalTime = 0;
	}

	void Timer::SetTimeScale(float aValue)
	{
		Timer& time = Get();
		assert(aValue > 0);

		time.timeScale = aValue;
	}

	float Timer::GetTimeScale()
	{
		Timer& time = Get();
		return time.timeScale;
	}

	void Timer::Update()
	{
		Timer& time = Get();
		time.thisFrame = time.clock.now();

		time.duration = time.thisFrame - time.lastFrame;

		time.deltaTime = time.duration.count() * time.timeScale;

		time.lastFrame = time.thisFrame;
	}

	float Timer::GetDeltaTime()
	{
		return Get().deltaTime;
	}

	double Timer::GetTotalTime()
	{
		Timer& time = Get();
		std::chrono::duration<double> totalDuration;

		time.totalTimeTimer = time.clock.now();

		totalDuration = time.totalTimeTimer - time.startTime;

		return totalDuration.count();
	}

	Timer& Timer::Get()
	{
		static Timer time;
		return time;
	}
}
