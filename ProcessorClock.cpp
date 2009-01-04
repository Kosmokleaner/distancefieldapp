#include "stdafx.h"														// for precompiled headers (has to be in first place)
#include "ProcessorClock.h"										// CProcessorClock
#include <assert.h>														// assert()
//#include "../../common/ITimer.h"							// CTimeValue


LARGE_INTEGER CProcessorClock::m_PerformanceFrequency;




// constructor
CProcessorClock::CProcessorClock()
{
	Init();
}


__int64 CProcessorClock::GetActualTime() const
{
	LARGE_INTEGER b;

	assert(m_PerformanceFrequency.QuadPart);			// forgot to call Init() ?

	if(QueryPerformanceCounter(&b))
		return (1000*1024*(b.QuadPart-m_PerformanceLevelStart.QuadPart)) / m_PerformanceFrequency.QuadPart;

	assert(0);		// no PerformanceCounter available
	return 0;
}




// initialization
bool CProcessorClock::Init()
{
	if(m_PerformanceFrequency.QuadPart==0)
	{
		QueryPerformanceFrequency(&m_PerformanceFrequency);		// no error check here

		m_PerformanceLevelStart.QuadPart=0;
		return(QueryPerformanceCounter(&m_PerformanceLevelStart)!=0);
	}

	return true;
}









CTimer::CTimer() :m_fAveragedFrameTime(0.001f), m_fFrameTime(0.001f), m_fSumFrameTime(0), m_iAccumFrameSamples(0)
{
	m_LastAbsTimeTZ = m_ProcessorClock.GetActualTime();
}



float CTimer::GetFrameTime() const
{
	assert(m_fFrameTime>0);

	return m_fFrameTime;
}
	
float CTimer::GetAverageFrameTime() const
{
	assert(m_fAveragedFrameTime>0);

	return m_fAveragedFrameTime;
}




void CTimer::UpdateTimer()
{
	__int64 AbsTimeTZ = m_ProcessorClock.GetActualTime();			// in 1/1024ms

	__int64 FrameTimeTZ = AbsTimeTZ-m_LastAbsTimeTZ;

	m_fFrameTime = (float)((double)FrameTimeTZ / (double)(1024*1000));

	m_LastAbsTimeTZ = AbsTimeTZ;

	// ------------------------

	m_fSumFrameTime += m_fFrameTime;	
	++m_iAccumFrameSamples;

	if(m_fSumFrameTime>0.5f)
	{
		m_fAveragedFrameTime = m_fSumFrameTime/m_iAccumFrameSamples;
		m_iAccumFrameSamples=0;m_fSumFrameTime=0;
	}
}