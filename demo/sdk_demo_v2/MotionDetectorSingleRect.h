#pragma once

#include <opencv2/core.hpp>

#include <unordered_map>
#include <queue>


using InterestRects = std::vector<cv::Rect>;

enum class MDEventType {
	IN1,
	OUT1,
	IN2,
	OUT2,
	IN12,
	OUT12,
};

struct ZoneInfo {
	bool in_detected_{ false };
	cv::Point center_of_in_;
	std::deque<cv::Mat> last_images_;
	int last_processed_ts_{ -10000 };
	cv::Mat background_;


	cv::Point last_motion_out_center_;
	int ts_of_out{ -1000 };
	cv::Point center_of_out_;
	bool out_in_progress_{ false };

};

using SignalFunc = std::function<void(MDEventType, cv::Point ,cv::Point)>;

std::string EventTypeToString(MDEventType type);

InterestRects getStandart2Rects(cv::Size size);

class MotionDetectorWithInterestRects
{
public:
	/// <param name="rects"> - зоны интереса по id (ровно 2!)</param>
	/// <param name="detection_period"> - определяет периодичность детектирования движения, в миллисекундах</param>
	MotionDetectorWithInterestRects(int in_detection_period = 1000);

	void SetRects(InterestRects rects) {
		interest_rects_ = rects;
	}

	/// <param name="ts"> - в миллисекундах</param>
	void Detect(cv::Mat img, int ts, SignalFunc func);

	static const int kThresholdForIn{ 15 };
	static const int kThresholdForMotionOut{ 5 };
	static const int kThresholdForOut{ 5 };

	static const int kTimeout{ 5000 };


	static const double kInAreaCoef;
	static const double kMotionOutAreaCoef;
	static const double kOutAreaCoef;

	const int kInDetectionPeriod; //в ms
	static const int kOutDetectionPeriod{ 100 }; //в ms


private:
	InterestRects interest_rects_;

	cv::Mat background_;
	bool background_initialized_{ false };

	std::vector<ZoneInfo> zones_{ ZoneInfo(), ZoneInfo() };

	int last_proceesed_ts_{ -1000 };
	size_t current_detection_img_num_{ 0 };

	bool in12_signaled_{ false };


};

