#pragma once

#include <string>
#include <vector>
#include "stdafx.h"

#define TIMER_RATE 30

class AlbumWindowThread;
class DrawerDemo;

struct UserInLayout {
	unsigned id = 0;
	std::wstring username;
	RECT rect{ 0 };
};

typedef std::vector<UserInLayout> Layout;

class ILayoutGrabberEvent {
public:
	virtual void onMeetingChanged(bool start) = 0;
	virtual void onLayoutChanged(const Layout& layout) = 0;
	virtual void onHwndChanged(const HWND& hwnd) = 0;
};

class ILayoutGrabberAndFrameEvent : public ILayoutGrabberEvent {
public:
	virtual void onFrameReceived() = 0;
};

class LayoutGrabber : public ILayoutGrabberEvent
{
public:
	LayoutGrabber();
	~LayoutGrabber();

	void onMeetingChanged(bool start) override;
	void onLayoutChanged(const Layout& layout) override;
	void onHwndChanged(const HWND& hwnd) override;

private:
	void onTimerFired(uint32_t ts);

	void StartTimer();
	void StopTimer();

	static void CALLBACK TimerWrapper(HWND hwnd, UINT msg, UINT idTimer, DWORD dwTime);

	static LayoutGrabber* m_timerGrabber;

	HWND m_layoutHwnd;
	UINT_PTR m_timerId;

	DrawerDemo* m_drawerDemo;
	AlbumWindowThread* m_awThread;
};

