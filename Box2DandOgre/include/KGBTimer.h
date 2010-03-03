/*=============================================================================

		KGBTimer.h

		Author: Matt King

=============================================================================*/

#ifndef KGBTIMER_H
#define KGBTIMER_H

#include "windows.h"

class KGBTimer
{
public:
	KGBTimer()
	{
		totalTime_ = 0;
		count_ = 0;
		t1.QuadPart = t2.QuadPart = 0;

		QueryPerformanceFrequency(&frequency);
	}

	~KGBTimer()
	{

	}

	void StartTimer()
	{
		lastTime = t1;
		QueryPerformanceCounter(&t1);
		
		// Get the processor time.
		LONGLONG qpart = (t1.QuadPart - lastTime.QuadPart);
		// There is some weird math error here, I don't know what it is but
		// I had to multiply by 10000000 or it gave wrong results.
		double time = LONGLONG( (qpart * 10000000) / frequency.QuadPart );
		time /= 10000000.0;
		totalTime_ += time;

		count_++;
	}

	double EndTimer()
	{
		return totalTime_ / count_;
	}


private:

	LARGE_INTEGER frequency;
	LARGE_INTEGER t1;
	LARGE_INTEGER lastTime;
	LARGE_INTEGER t2;

	double totalTime_;
	double count_;
	
};

#endif