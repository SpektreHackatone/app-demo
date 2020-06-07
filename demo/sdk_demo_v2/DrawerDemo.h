#pragma once
#include "AlbumWindow.h"
#include <SFML/Graphics.hpp>
#include "SceneElements.h"
#include "MotionDetectorSingleRect.h"

class LayoutGrabber;

class DrawerDemo : public IDrawingThing
{
public:
	DrawerDemo(LayoutGrabber* g);
	virtual ~DrawerDemo();

	// this function is called once when window is opened
	void Init(sf::RenderWindow* window) override;

	// this function is called periodically
	void Draw(sf::RenderWindow* window) override;

	// this function puts last screenshot in thread-safe way
	void PutFrame(const ImgConstPtr& img, uint32_t ts_ms) override;

	// this function puts layout update in thread-safe way
	void PutLayout(const LayoutInfo& l) override;

	void PutChatMessage(const std::wstring& str) override;

	void OnMotionDetected(MDEventType ev, cv::Point p1, cv::Point p2);

private:
	IFlyingObject::Ptr SpawnTomato(const cv::Point& p);
	void LaunchTomato(const IFlyingObject::Ptr& obj, const cv::Point& p1, const cv::Point& p2);

	Scene::Ptr m_scene;
	sf::Vector2u m_windowSize;

	ImgConstPtr m_img;
	LayoutInfo m_layout;

	IFlyingObject::Ptr m_leftLauncher, m_rightLauncher;

	MotionDetectorWithInterestRects m_detector;
	bool m_detectorInitialized;

	LayoutGrabber* m_g;
};

