
#include "pch.h"
#include "Timer.h"

#include <chrono>
#include <cassert>

namespace CommonUtilities
{
	Time::Time()
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

	void Time::SetTimeScale(float aValue)
	{
		Time& time = Get();
		assert(aValue > 0);

		time.timeScale = aValue;
	}

	float Time::GetTimeScale()
	{
		Time& time = Get();
		return time.timeScale;
	}

	void Time::Update()
	{
		Time& time = Get();
		time.thisFrame = time.clock.now();

		time.duration = time.thisFrame - time.lastFrame;

		time.deltaTime = time.duration.count() * time.timeScale;

		time.lastFrame = time.thisFrame;
	}

	float Time::GetDeltaTime()
	{
		return Get().deltaTime;
	}

	double Time::GetTotalTime()
	{
		Time& time = Get();
		std::chrono::duration<double> totalDuration;

		time.totalTimeTimer = time.clock.now();

		totalDuration = time.totalTimeTimer - time.startTime;

		return totalDuration.count();
	}

	Time& Time::Get()
	{
		static Time time;
		return time;
	}
}
