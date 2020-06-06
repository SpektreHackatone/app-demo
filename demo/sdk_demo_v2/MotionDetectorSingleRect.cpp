#include "MotionDetectorSingleRect.h"

#include <opencv2/opencv.hpp>

const double MotionDetectorWithInterestRects::kInAreaCoef{ 0.2 };
const double MotionDetectorWithInterestRects::kMotionOutAreaCoef{ 0.02 };
const double MotionDetectorWithInterestRects::kOutAreaCoef{ 0.05 };

namespace {
	cv::Mat getImgsDiff(cv::Mat img1, cv::Mat img2, cv::Mat img3, int threshold) {

		cv::cvtColor(img1, img1, cv::COLOR_BGR2GRAY);
		cv::cvtColor(img2, img2, cv::COLOR_BGR2GRAY);
		cv::cvtColor(img3, img3, cv::COLOR_BGR2GRAY);

		cv::Mat d1, d2, res;

		cv::absdiff(img1, img2, d1);
		cv::absdiff(img1, img3, d2);
		cv::bitwise_and(d1, d2, res);

		cv::blur(res, res, cv::Size(3, 3));

		cv::threshold(res, res, threshold, 255, cv::THRESH_BINARY);

		return res;
	}

	cv::Mat getImgsDiff(cv::Mat img1, cv::Mat img2, int threshold) {

		cv::cvtColor(img1, img1, cv::COLOR_BGR2GRAY);
		cv::cvtColor(img2, img2, cv::COLOR_BGR2GRAY);

		cv::Mat d1, d2;

		cv::absdiff(img1, img2, d1);

		cv::blur(d1, d1, cv::Size(3, 3));

		cv::threshold(d1, d1, threshold, 255, cv::THRESH_BINARY);

		return d1;
	}

	std::pair<bool, cv::Point> DetectMotion(cv::Mat motion_img, double motion_area_coef) {
		std::vector<std::vector<cv::Point>> contours;

		double true_contour_area = motion_img.total() * motion_area_coef;

		cv::findContours(motion_img, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

		bool motion_tracked = false;

		double max_contour_area{ 0.0 };
		cv::Point center_of_max_contour;
		for (size_t i = 0; i < contours.size(); ++i) {
			double this_contour_area = cv::contourArea(contours[i]);
			if (this_contour_area > true_contour_area) {
				motion_tracked = true;

				if (this_contour_area > max_contour_area) {
					max_contour_area = this_contour_area;
					auto m = moments(contours[i]);
					center_of_max_contour = cv::Point(m.m10 / m.m00, m.m01 / m.m00);
				}

				//      cv::drawContours(show, contours, i, cv::Scalar(0, 0, 255), 3);
			}
		}

		return std::make_pair(motion_tracked, center_of_max_contour);
	}
}

MotionDetectorWithInterestRects::MotionDetectorWithInterestRects(int detection_period) :
	kInDetectionPeriod(detection_period)
{
}

void MotionDetectorWithInterestRects::Detect(cv::Mat img, int ts, SignalFunc func) {
	if (!background_initialized_) {
		//background_ = img;
		zones_[0].background_ = img(interest_rects_[0]).clone();
		zones_[1].background_ = img(interest_rects_[1]).clone();
		background_initialized_ = true;
		return;
	}

	for (size_t i = 0; i < zones_.size();++i) {
		if (zones_[i].out_in_progress_ && (ts - zones_[i].ts_of_out > kTimeout)) {
			zones_[i].out_in_progress_ = false;
		}

		if (!zones_[i].out_in_progress_ && !zones_[i].in_detected_) {
			if (ts - zones_[i].last_processed_ts_ > kInDetectionPeriod) {
				zones_[i].last_images_.push_back(img(interest_rects_[i]).clone());
				zones_[i].last_processed_ts_ = ts;
			}

			if (zones_[i].last_images_.size() >= 2) {

				auto motion_img = getImgsDiff(zones_[i].background_, zones_[i].last_images_.front(), zones_[i].last_images_.back(), MotionDetectorWithInterestRects::kThresholdForIn);
				auto has_motion_and_center = DetectMotion(motion_img, MotionDetectorWithInterestRects::kInAreaCoef);

				if (has_motion_and_center.first) {
					zones_[i].in_detected_ = true;
					zones_[i].center_of_in_ = has_motion_and_center.second;
					zones_[i].center_of_in_.x += interest_rects_[i].x;
					zones_[i].center_of_in_.y += interest_rects_[i].y;

					if (i == 0)
						func(MDEventType::IN1, cv::Point(), cv::Point());
					if (i == 1)
						func(MDEventType::IN2, cv::Point(), cv::Point());

				}

				zones_[i].last_images_.pop_front();

			}
		} 

		if (zones_[i].in_detected_) {

			if (ts - zones_[i].last_processed_ts_ > kOutDetectionPeriod) {
				zones_[i].last_images_.push_back(img(interest_rects_[i]).clone());
				zones_[i].last_processed_ts_ = ts;
			}

			if (zones_[i].last_images_.size() >= 3) {

				//Движение на выход 
				{
					auto diff = getImgsDiff(zones_[i].last_images_[0], zones_[i].last_images_[1], zones_[i].last_images_[2], MotionDetectorWithInterestRects::kThresholdForMotionOut);
					auto has_motion_and_center = DetectMotion(diff, MotionDetectorWithInterestRects::kMotionOutAreaCoef);
					if (has_motion_and_center.first)
						zones_[i].last_motion_out_center_ = has_motion_and_center.second;
				}

				//Наличие в зоне инородных элементов
				{
					auto diff_background = getImgsDiff(zones_[i].background_, zones_[i].last_images_[0], zones_[i].last_images_[1], MotionDetectorWithInterestRects::kThresholdForOut);
					auto has_out_motion_and_center = DetectMotion(diff_background, MotionDetectorWithInterestRects::kOutAreaCoef);

					if (!has_out_motion_and_center.first) {
						zones_[i].in_detected_ = false;
						zones_[i].ts_of_out = ts;

						zones_[i].out_in_progress_ = true;

						zones_[i].center_of_out_ = zones_[i].last_motion_out_center_;
						zones_[i].center_of_out_.x += interest_rects_[i].x;
						zones_[i].center_of_out_.y += interest_rects_[i].y;

						in12_signaled_ = false;


						if (i == 0)
							func(MDEventType::OUT1, zones_[0].center_of_in_, zones_[0].center_of_out_);
						if (i == 1)
							func(MDEventType::OUT2, zones_[1].center_of_in_, zones_[1].center_of_out_);
					}
				}

				zones_[i].last_images_.pop_front();

			}
		}

	}

	if (zones_[0].in_detected_ && zones_[1].in_detected_ && !in12_signaled_) {
		func(MDEventType::IN12, cv::Point(), cv::Point());
		in12_signaled_ = true;
	}

	/* //отрисовка

	cv::rectangle(img, interest_rects_[0], cv::Scalar(0, 255, 0));
	cv::rectangle(img, interest_rects_[1], cv::Scalar(0, 255, 0));

	if (zones_[0].in_detected_)
		cv::circle(img, zones_[0].center_of_in_, 3, cv::Scalar(0, 0, 255), cv::FILLED);
	if (zones_[1].in_detected_)
		cv::circle(img, zones_[1].center_of_in_, 3, cv::Scalar(0, 0, 255), cv::FILLED);

	if (zones_[0].out_in_progress_)
		cv::circle(img, zones_[0].center_of_out_, 3, cv::Scalar(255, 0, 255), cv::FILLED);
	if (zones_[1].out_in_progress_)
		cv::circle(img, zones_[1].center_of_out_, 3, cv::Scalar(255, 0, 255), cv::FILLED);

	if (zones_[0].out_in_progress_)
		cv::arrowedLine(img, zones_[0].center_of_in_, zones_[0].center_of_out_, cv::Scalar(0, 255, 255), 5);
	if (zones_[1].out_in_progress_)
		cv::arrowedLine(img, zones_[1].center_of_in_, zones_[1].center_of_out_, cv::Scalar(0, 255, 255), 5);

	cv::imshow("img", img);
	cv::waitKey(1);

	*/
}

std::string EventTypeToString(MDEventType type)
{
	std::string res;
	switch (type)
	{
	case MDEventType::IN1:
		res = "IN1";
		break;
	case MDEventType::OUT1:
		res = "OUT1";
		break;
	case MDEventType::IN2:
		res = "IN2";
		break;
	case MDEventType::OUT2:
		res = "OUT2";
		break;
	case MDEventType::IN12:
		res = "IN12";
		break;
	case MDEventType::OUT12:
		res = "OUT12";
		break;
	default:
		break;
	}
	return res;
}

InterestRects getStandart2Rects(cv::Size size)
{
	return InterestRects{ cv::Rect(0.1*size.width,0.1 * size.height,0.25 * size.width,0.25 * size.height ), 
		cv::Rect(0.6 * size.width,0.1 * size.height,0.25 * size.width,0.25 * size.height ) };
}
