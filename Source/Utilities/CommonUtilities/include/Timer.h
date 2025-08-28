#pragma once

#include <chrono>

namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer();

		Timer(const Timer& aTime) = delete;

		Timer& operator=(const Timer& aTime) = delete;

		~Timer() = default;

		static void Update();

		static float GetTimeScale();
		static void SetTimeScale(float aValue);

		static float GetDeltaTime();
		static double GetTotalTime();

		static Timer& Get();

	private:
		std::chrono::high_resolution_clock::time_point lastFrame;
		std::chrono::high_resolution_clock::time_point thisFrame;

		std::chrono::high_resolution_clock::time_point totalTimeTimer;
		std::chrono::high_resolution_clock::time_point startTime;

		std::chrono::high_resolution_clock clock;

		std::chrono::duration<float> duration;

		 float deltaTime;

		 float timeScale;

		 double totalTime;
	};

}