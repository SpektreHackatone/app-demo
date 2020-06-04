#pragma once
#include <Windows.h>

/*
//пример использования таймера

void callback(HWND window)
{
	std::cout << rand() << std::endl;
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, GetScreenShot(window));
	CloseClipboard();
}

int main()
{
	HWND console = GetForegroundWindow();
	ThrowOutWindow(console);
	//убрать кнопку Свернуть
	DisableMinimaizeBtn(console);

	TimerWinApi t = TimerWinApi(&callback, console, 50);
	t.Start();
	
	//для того, чтобы таймер работал
	MSG msg;          // message structure

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg); // translates virtual-key codes
		DispatchMessage(&msg);  // dispatches message to window
	}
	return 0;
}
*/

class TimerWinApi
{
public:
	TimerWinApi(void (*pFunc)(HWND), HWND window, int rate);
	void Start();
	void Stop();
	~TimerWinApi();

private:
	static void (*_pFunc)(HWND);
	static HWND _window;
	static void CALLBACK _timerProc(HWND hwnd, UINT msg, UINT idTimer, DWORD dwTime);
	UINT_PTR _timerId;
	int _rate;
};


void (*TimerWinApi::_pFunc)(HWND);
HWND TimerWinApi::_window;

void CALLBACK TimerWinApi::_timerProc(HWND hwnd, UINT msg, UINT idTimer, DWORD dwTime)
{
	_pFunc(_window);
}

TimerWinApi::TimerWinApi(void (*pFunc)(HWND), HWND window, int rate)
{
	_pFunc = pFunc;
	_window = window;
	_rate = rate;
}

TimerWinApi::~TimerWinApi()
{
	_pFunc = nullptr;
}

void TimerWinApi::Start()
{
	_timerId = SetTimer(NULL, 0, _rate, (TIMERPROC)_timerProc);
}
void TimerWinApi::Stop()
{
	KillTimer(_window, _timerId);
}