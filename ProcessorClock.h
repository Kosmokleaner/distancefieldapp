#pragma once

#include <windows.h>
#include <winnt.h>



class CProcessorClock  
{
public:

	//! constructor, calls Init()
	CProcessorClock();

	//! Init (set time=0)
	//! \return true=success, false=failed(no CPU with PerformanceCounter)
	bool Init();

	//! \return  in 1/1024ms
	__int64 GetActualTime() const;

private: // -------------------------------------------------------------

	static LARGE_INTEGER		m_PerformanceFrequency;					//!< CPU ticks per second
	LARGE_INTEGER						m_PerformanceLevelStart;				//!< CPU start time
};



// convenience class for frametime, and average frametime
class CTimer
{
public:
	CTimer();
	
	//! needs to be called each frame
	void UpdateTimer();
	
	//! \return in seconds, FrameRate = 1.0f/GetFrameTime()
	float GetFrameTime() const;

	//! changes not that frequently - easier to read with high fps
	//! \return in seconds
	float GetAverageFrameTime() const;

private: // -----------------------------------------------------------

	CProcessorClock		m_ProcessorClock;

	__int64						m_LastAbsTimeTZ;				//!< 1/1024ms units

	float							m_fFrameTime;						//! in seconds
	float							m_fAveragedFrameTime;		//! in seconds
	float							m_fSumFrameTime;				//!< to compute m_AveragedFrameTime
	int								m_iAccumFrameSamples;		//!<
};
