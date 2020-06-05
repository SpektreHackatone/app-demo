#pragma once

#include <Windows.h>
#include <SFML/Graphics.hpp>
#include "AlbumWindow.h"

ImgConstPtr GetScreenShot(HWND window);
void ThrowOutWindow(HWND window);
void ComebackWindow(HWND window);
void DisableMinimaizeBtn(HWND window);
void EnableMinimaizeBtn(HWND window);