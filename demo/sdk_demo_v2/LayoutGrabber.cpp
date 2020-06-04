#include "LayoutGrabber.h"
#include <iostream>

void LayoutGrabber::onHwndChanged(const HWND& hwnd)
{
	m_layoutHwnd = hwnd;
	OutputDebugString(L"hwnd changed");
}

void LayoutGrabber::onLayoutChanged(const Layout& layout)
{
	m_layout = layout;
	OutputDebugString(L"layout changed");
}

void LayoutGrabber::onMeetingChanged(bool start)
{
	OutputDebugString(L"meeting changed");
}

void LayoutGrabber::onTimerFired()
{

}