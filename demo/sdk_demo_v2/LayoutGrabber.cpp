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
	m_chatControllerWorkflow = nullptr;

	m_awThread->Start();
	StartTimer();
}

LayoutGrabber::~LayoutGrabber() {
	m_awThread->Stop();
	StopTimer();

	delete m_drawerDemo;
	delete m_awThread;
	delete m_chatControllerWorkflow;
}

void LayoutGrabber::SendChatMessage(uint32_t id, const std::wstring& str)
{
	m_chatControllerWorkflow->SendChatMsg(id, (wchar_t*)str.c_str());
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

	// update users info here?
	LayoutInfo copy = layout;

	if (m_chatControllerWorkflow) {
		for (auto& user : copy) {
			const auto& info = m_chatControllerWorkflow->GetUserInfoByID(user.id);
			if (!info) {
				continue;
			}

			if (info->IsMySelf()) {
				user.is_me = true;
			}
			if (info->IsHost()) {
				user.is_host = true;
			}
		}
	}

	m_awThread->PutLayout(copy);
}

void LayoutGrabber::onMeetingChanged(bool start)
{
	OutputDebugString(L"meeting changed\n");

	if (start) {
		//m_awThread->Start();
		//StartTimer();

		if (m_chatControllerWorkflow) {
			delete m_chatControllerWorkflow;
		}
		m_chatControllerWorkflow = new CSDKChatControllerWorkFlow();
		m_chatControllerWorkflow->SetEvent(this);
	}
	else {
		//m_awThread->Stop();
		//StopTimer();
	}
}

void LayoutGrabber::onChatMsgNotifcation(ZOOM_SDK_NAMESPACE::IChatMsgInfo* chatMsg, const wchar_t* ccc) {
	OutputDebugString(L"Message received!\n");

	if (chatMsg->IsChatToAll()) {
		const std::wstring str = chatMsg->GetContent();
		if (str[0] == L'$') {
			m_awThread->PutChatMessage(str);
		}
	}
}

void LayoutGrabber::onChatStautsChangedNotification(ZOOM_SDK_NAMESPACE::ChatStatus* status_) {

}

void LayoutGrabber::onTimerFired(uint32_t ts)
{
	if (m_layoutHwnd) {
		ImgConstPtr img = GetScreenShot(m_layoutHwnd);
		m_awThread->PutFrame(img, ts);
	}

	if (m_chatControllerWorkflow) {
		// get chat messages
		std::list<std::wstring> data = m_awThread->TakeMsgList();

		for (auto& m : data) {
			m_chatControllerWorkflow->SendChatMsg(0, (wchar_t*)m.c_str());
		}
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