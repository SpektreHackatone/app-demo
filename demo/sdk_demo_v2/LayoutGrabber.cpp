#include "LayoutGrabber.h"
#include <iostream>

LayoutGrabber::LayoutGrabber() {}

void LayoutGrabber::onHwndChanged(const HWND& hwnd)
{
	m_layoutHwnd = hwnd;
	OutputDebugString(L"hwnd changed\n");
}

void LayoutGrabber::onLayoutChanged(const Layout& layout)
{
	m_layout = layout;
	OutputDebugString(L"layout changed\n");
}

void LayoutGrabber::onMeetingChanged(bool start)
{
	OutputDebugString(L"meeting changed\n");

	if (start) {
		m_awThread.Start();
	}
	else {
		m_awThread.Stop();
	}
}

void LayoutGrabber::onTimerFired()
{

}