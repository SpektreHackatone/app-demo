#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class AlbumWindow
{
};

class AlbumWindowThread {
public:
	AlbumWindowThread();

	void Start();
	void Stop();

private:
	static void ThreadWrapper(AlbumWindowThread* p);
	void ThreadFunc();

	sf::Thread m_thread;
	sf::Mutex m_mutex;

	bool m_isRunning;
};

