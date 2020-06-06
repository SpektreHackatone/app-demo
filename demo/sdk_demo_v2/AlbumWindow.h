#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "LayoutGrabber.h"
#include <Windows.h>
#include "DrawingCommon.h"

class IDrawingThing {
public:
	// this function is called once when window is opened
	virtual void Init(sf::RenderWindow* window) = 0;

	// this function is called periodically
	virtual void Draw(sf::RenderWindow* window) = 0;

	// this function puts last screenshot in thread-safe way
	virtual void PutFrame(const ImgConstPtr& img, uint32_t ts_ms) = 0;

	// this function puts layout update in thread-safe way
	virtual void PutLayout(const LayoutInfo& l) = 0;
};

class AlbumWindowThread {
public:
	AlbumWindowThread(IDrawingThing* drawer);

	void Start();
	void Stop();

	void PutLayout(const LayoutInfo& l);
	void PutFrame(const ImgConstPtr& img, uint32_t ts_ms);

private:
	static void ThreadWrapper(AlbumWindowThread* p);
	void ThreadFunc();

	sf::Thread m_thread;
	sf::Mutex m_mutex;

	bool m_frameUpdated;
	ImgConstPtr m_lastFrame;
	uint32_t m_lastFrameTsMs;

	bool m_layoutUpdated;
	LayoutInfo m_lastLayout;

	IDrawingThing* m_drawer;

	bool m_isRunning;
};

