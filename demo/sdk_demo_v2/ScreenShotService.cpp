#include "ScreenShotService.h"
#include <SFML/Graphics.hpp>

ImgConstPtr GetScreenShot(HWND window)
{
	if (window) {
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

		//uint8_t* bitmap_data = new uint8_t[Width * Height * 4];
		//GetDIBits(hDC, hBitmap, 0, Height, bitmap_data, &BMI, DIB_RGB_COLORS);

		//sf::Image frame;
		//frame.create(Width, Height, bitmap_data);

		//clean up
		SelectObject(hDC, hOldBitmap);
		DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);
	}

	// FIXME
	ImgPtr img(new sf::Image());
	img->loadFromFile("Lenna.png");

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