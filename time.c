#include "time.h"

#include "platform.h"

double get_time()
{
	LARGE_INTEGER performance_count;
	
	QueryPerformanceCounter(&performance_count);

	LARGE_INTEGER frequency;
	
	QueryPerformanceFrequency(&frequency);

	return (double)performance_count.QuadPart / (double)frequency.QuadPart;
}

void sleep(double duration)
{
	static HANDLE timer;

	if (timer == NULL)
	{
		timer = CreateWaitableTimer(NULL, TRUE, NULL);
	}

	if (duration <= 0.0)
	{
		return;
	}

	LARGE_INTEGER due_time;

	due_time.QuadPart = -10000000 * duration;

	SetWaitableTimer(timer, &due_time, 0, NULL, NULL, FALSE);

	WaitForSingleObject(timer, INFINITE);
}
