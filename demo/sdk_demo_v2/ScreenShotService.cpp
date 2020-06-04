#include <Windows.h>

HBITMAP GetScreenShot(HWND window)
{
	//screen of window
	HDC hScreen = GetDC(window);
	RECT windowParams, clientParams;
	POINT ptDiff;
	//client sizes
	GetClientRect(window, &clientParams);
	//window sizes
	GetWindowRect(window, &windowParams);
	DWORD Width = windowParams.right - windowParams.left;
	DWORD Height = windowParams.bottom - windowParams.top;

	ptDiff.x = Width - clientParams.right;
	ptDiff.y = Height - clientParams.bottom;

	//�������� ���������� � ������
	HDC hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, Width, Height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
	//������� BitBlt ��������� �������� ������� ������ ������ � �����, ��������������� �������������� �������� �� ��������� ��������� ��������� ���������� � ������� �������� ����������
	BOOL bRet = BitBlt(hDC, 0, 0, Width - ptDiff.x, Height - ptDiff.y, hScreen, 0, 0, SRCCOPY);

	//������ ������������� ��������� (���������� � ����� ������)
	/*
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();
	*/

	//clean up
	SelectObject(hDC, hOldBitmap);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	return hBitmap;
}

void ThrowOutWindow(HWND window)
{
	RECT windowParams, screenParams;
	GetWindowRect(window, &windowParams);
	GetWindowRect(GetDesktopWindow(), &screenParams);
	DWORD Width = windowParams.right - windowParams.left;
	DWORD Height = windowParams.bottom - windowParams.top;
	SetWindowPos(window, NULL, screenParams.right, screenParams.top, Width, Height, SWP_SHOWWINDOW);
}

void ComebackWindow(HWND window)
{
	RECT windowParams;
	GetWindowRect(window, &windowParams);
	DWORD Width = windowParams.right - windowParams.left;
	DWORD Height = windowParams.bottom - windowParams.top;
	SetWindowPos(window, NULL, 0, 0, Width, Height, SWP_SHOWWINDOW);
}

void DisableMinimaizeBtn(HWND window)
{
	SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_MINIMIZEBOX);
}

void EnableMinimaizeBtn(HWND window)
{
	SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_MINIMIZEBOX);
}