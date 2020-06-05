#include "ScreenShotService.h"
#include <SFML/Graphics.hpp>

ImgConstPtr GetScreenShot(HWND window)
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

	//контекст устройства в памяти
	HDC hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, Width, Height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
	//Функция BitBlt выполняет передачу битовых блоков данных о цвете, соответствующих прямоугольнику пикселей из заданного исходного контекста устройства в целевой контекст устройства
	BOOL bRet = BitBlt(hDC, 0, 0, Width - ptDiff.x, Height - ptDiff.y, hScreen, 0, 0, SRCCOPY);

	//пример использования скриншота (сохранение в буфер обмена)
	/*
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();
	*/

	BITMAPINFO BMI;
	//ZeroMemory( &BMI, sizeof BMI );
	BMI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BMI.bmiHeader.biWidth = Width;
	BMI.bmiHeader.biHeight = -Height;
	BMI.bmiHeader.biPlanes = 1;
	BMI.bmiHeader.biBitCount = 32;
	BMI.bmiHeader.biCompression = BI_RGB;

	ImgPtr img(new sf::Image());
	img->create(Width, Height);
	uint8_t* const data = (uint8_t*)img->getPixelsPtr();
	size_t size = Width * Height * 4;

	GetDIBits(hDC, hBitmap, 0, Height, (uint8_t*)img->getPixelsPtr(), &BMI, DIB_RGB_COLORS);
	for (size_t i = 0; i < size; i += 4) {
		uint8_t tmp = data[i + 0];
		data[i + 0] = data[i + 2];
		data[i + 2] = tmp;
		data[i + 3] = 255; // remove alpha
	}

	//clean up
	SelectObject(hDC, hOldBitmap);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);

	return img;
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