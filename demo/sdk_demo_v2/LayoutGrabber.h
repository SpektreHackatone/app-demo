#pragma once

#include <string>
#include <vector>
#include "stdafx.h"
#include "AlbumWindow.h"

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

class LayoutGrabber : public ILayoutGrabberEvent
{
public:
	LayoutGrabber();

	void onMeetingChanged(bool start) override;
	void onLayoutChanged(const Layout& layout) override;
	void onHwndChanged(const HWND& hwnd) override;

	void onTimerFired();

private:
	HWND m_layoutHwnd;
	Layout m_layout;

	AlbumWindowThread m_awThread;
};

