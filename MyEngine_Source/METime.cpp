#include "METime.h"
namespace ME {


	LARGE_INTEGER Time::CpuFrequency = {};
	LARGE_INTEGER Time::PrevFrequency = {};
	LARGE_INTEGER Time::CurrentFrequency = {};
	float Time::DeltaTimeValue = 0.0f;


	void ME::Time::Intialize()
	{
		//Cpu 고유 진동수
		QueryPerformanceFrequency(&CpuFrequency);

		//프로그램이 시작 했을 때 현재 진동 수
		QueryPerformanceCounter(&PrevFrequency);
	}

	void ME::Time::Update()
	{
		QueryPerformanceCounter(&CurrentFrequency);

		float differenceFrequency
			= static_cast<float>(CurrentFrequency.QuadPart - PrevFrequency.QuadPart);//한 프레임에서의 차이

		DeltaTimeValue = differenceFrequency / static_cast<float>(CpuFrequency.QuadPart);

		PrevFrequency.QuadPart = CurrentFrequency.QuadPart;
	}
	void Time::Render() 
	{
	//static float time = 0.0f;
	//
	//time += DeltaTimeValue;
	//
	//float fps = 1.0f / DeltaTimeValue;

	}
}
