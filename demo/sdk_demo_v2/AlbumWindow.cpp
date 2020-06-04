#include "AlbumWindow.h"

AlbumWindowThread::AlbumWindowThread()
	: m_thread(&AlbumWindowThread::ThreadWrapper, this)
    , m_isRunning(false)
{
}

void AlbumWindowThread::Start()
{
	m_isRunning = true;
	m_thread.launch();
}

void AlbumWindowThread::Stop()
{
	m_isRunning = false;
	m_thread.wait();
}

void AlbumWindowThread::ThreadFunc()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "AlbumView");

	while (m_isRunning && window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Green);

        // draw everything here...
        // window.draw(...);

        // end the current frame
        window.display();
	}

	if (window.isOpen()) {
		window.close();
	}
}

void AlbumWindowThread::ThreadWrapper(AlbumWindowThread* p)
{
    p->ThreadFunc();
}