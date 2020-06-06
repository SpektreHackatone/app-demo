#include "AlbumWindow.h"
#include <list>

AlbumWindowThread::AlbumWindowThread(IDrawingThing* drawer)
	: m_thread(&AlbumWindowThread::ThreadWrapper, this)
    , m_isRunning(false)
    , m_drawer(drawer)
    , m_layoutUpdated(false)
    , m_frameUpdated(false)
    , m_lastFrameTsMs(0)
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

void AlbumWindowThread::PutLayout(const LayoutInfo& l)
{
    m_mutex.lock();
    m_lastLayout = l;
    m_layoutUpdated = true;
    m_mutex.unlock();
}

void AlbumWindowThread::PutFrame(const ImgConstPtr& img, uint32_t ts_ms)
{
    m_mutex.lock();
    m_frameUpdated = true;
    m_lastFrame = img;
    m_lastFrameTsMs = ts_ms;
    m_mutex.unlock();
}

void AlbumWindowThread::PutChatMessage(const std::wstring& str)
{
    m_mutex.lock();
    m_chatQueue.push_back(str);
    m_chatUpdated = true;
    m_mutex.unlock();
}

void AlbumWindowThread::ThreadFunc()
{
	sf::RenderWindow window(sf::VideoMode(1200, 750), "AlbumView");

    // this will add some sleep() in window.display()
    window.setFramerateLimit(30.0);

    m_drawer->Init(&window);

	while (m_isRunning && window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // check if frame data or layout is updated
        ImgConstPtr frame;
        uint32_t ts_ms = 0;
        std::list<std::wstring> strs;

        const LayoutInfo* layout = nullptr;
        {
            m_mutex.lock();
            if (m_frameUpdated) {
                frame = m_lastFrame;
                ts_ms = m_lastFrameTsMs;
                m_frameUpdated = false;
            }

            if (m_layoutUpdated) {
                layout = &m_lastLayout;
                m_layoutUpdated = false;
            }

            if (m_chatUpdated) {
                strs = m_chatQueue;
                m_chatUpdated = false;
            }
            m_mutex.unlock();
        }

        // put new frame if it is updated
        if (frame) {
            m_drawer->PutFrame(frame, ts_ms);
        }

        // put new layout if it is updated
        if (layout) {
            m_drawer->PutLayout(*layout);
        }

        if (strs.size() > 0) {
            for (const auto& s : strs) {
                m_drawer->PutChatMessage(s);
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Green);

        // draw everything here...
        m_drawer->Draw(&window);

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