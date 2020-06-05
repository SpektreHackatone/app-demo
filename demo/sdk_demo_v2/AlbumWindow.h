#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "LayoutGrabber.h"
#include <Windows.h>

using ImgPtr = std::shared_ptr<sf::Image>;
using ImgConstPtr = std::shared_ptr<const sf::Image>;

class IDrawingThing {
public:
	// this function is called once when window is opened
	virtual void Init() = 0;

	// this function is called periodically
	virtual void Draw(sf::RenderWindow* window) = 0;

	// this function puts last screenshot in thread-safe way
	virtual void PutFrame(const ImgConstPtr& img, uint32_t ts_ms) = 0;

	// this function puts layout update in thread-safe way
	virtual void PutLayout(const Layout& l) = 0;
};

class AlbumWindowThread {
public:
	AlbumWindowThread(IDrawingThing* drawer);

	void Start();
	void Stop();

	void PutLayout(const Layout& l);
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
	Layout m_lastLayout;

	IDrawingThing* m_drawer;

	bool m_isRunning;
};

