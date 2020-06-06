#include "LayoutGrabber.h"
#include "AlbumWindow.h"
#include "DrawerDemo.h"
#include "ScreenShotService.h"

LayoutGrabber* LayoutGrabber::m_timerGrabber;

LayoutGrabber::LayoutGrabber() {
	m_drawerDemo = new DrawerDemo();
	m_awThread = new AlbumWindowThread(m_drawerDemo);
	m_layoutHwnd = NULL;
	m_timerId = NULL;
	m_timerGrabber = this;

	m_awThread->Start();
	StartTimer();
}

LayoutGrabber::~LayoutGrabber() {
	m_awThread->Stop();
	StopTimer();

	delete m_drawerDemo;
	delete m_awThread;
}

void LayoutGrabber::onHwndChanged(const HWND& hwnd)
{
	m_layoutHwnd = hwnd;
	OutputDebugString(L"hwnd changed\n");
	DisableMinimaizeBtn(hwnd);
}

void LayoutGrabber::onLayoutChanged(const LayoutInfo& layout)
{
	OutputDebugString(L"layout changed\n");
	m_awThread->PutLayout(layout);
}

void LayoutGrabber::onMeetingChanged(bool start)
{
	return;
	OutputDebugString(L"meeting changed\n");

	if (start) {
		m_awThread->Start();
		StartTimer();
	}
	else {
		m_awThread->Stop();
		StopTimer();
	}
}

void LayoutGrabber::onTimerFired(uint32_t ts)
{
	if (m_layoutHwnd) {
		ImgConstPtr img = GetScreenShot(m_layoutHwnd);
		m_awThread->PutFrame(img, ts);
	}
}

void LayoutGrabber::StartTimer()
{
	m_timerId = SetTimer(NULL, 0, TIMER_RATE, (TIMERPROC)LayoutGrabber::TimerWrapper);
}

void LayoutGrabber::StopTimer()
{
	if (m_timerId && m_layoutHwnd) {
		KillTimer(m_layoutHwnd, m_timerId);
	}
}

void CALLBACK LayoutGrabber::TimerWrapper(HWND hwnd, UINT msg, UINT idTimer, DWORD dwTime)
{
	if (m_timerGrabber) {
		m_timerGrabber->onTimerFired(dwTime);
	}
}