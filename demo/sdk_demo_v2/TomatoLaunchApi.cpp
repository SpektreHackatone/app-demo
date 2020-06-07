#include "TomatoLaunchApi.h"
#include "AnimatedSprite.h"
#include <iomanip>
#include <sstream>

ISplashObject::Ptr TomatoLaunchApi::MakeSplash(const std::wstring& type, float scale, float rot) {
	ISplashObject::Ptr ret;
	if (type == L"ts1") {
		ret = AnimatedSpriteSplash::Ptr(new AnimatedSpriteSplash("images/klaksa1.png"));
	}
	else if (type == L"ts2") {
		ret = AnimatedSpriteSplash::Ptr(new AnimatedSpriteSplash("images/klaksa2.png"));
	}

	if (ret) {
		ret->setScale(scale, scale);
		ret->setRotation(rot);
	}

	return ret;
}

IFlyingObject::Ptr TomatoLaunchApi::MakeTomato(const Scene::WeakPtr& scene, const std::wstring& type, const std::wstring& splash_type, float t_scale, float s_scale, float s_rot) {
	ISplashObject::Ptr splash = MakeSplash(splash_type, s_scale, s_rot);

	if (!splash) {
		return nullptr;
	}

	IFlyingObject::Ptr ret;

	if (type == L"t") {
		ret = AnimatedSprite::Ptr(new AnimatedSprite(scene,
			"images/tomato_128_128.png",
			sf::IntRect(0, 0, 128, 128),
			splash));
		ret->setScale(sf::Vector2f(t_scale, t_scale));
	}

	return ret;
}

std::wstring TomatoLaunchApi::GetTomatoTypeDesc(const IFlyingObject::Ptr& obj) {
	if (obj) {
		std::wstringstream ss;
		// type - tomato
		ss << "t,";

		// splash - s1 (FIXME)
		ss << "ts1,";

		// scale
		ss.precision(2);
		ss << obj->getScale().x << ",";

		// splash scale (FIXME)
		ss << 0.3 << ",";

		// splash rotation (FIXME)
		ss << 0.0 << ",";

		return ss.str();
	}

	return std::wstring(L"");
}

IFlyingObject::Ptr TomatoLaunchApi::MakeTomatoFromDesc(const Scene::WeakPtr& scene, const std::wstring& str) {
	std::wstringstream ss;
	std::wstring t_type, s_type, t_scale, s_scale, s_rot;
	ss << str;

	if (!std::getline(ss, t_type, L',') ||
		!std::getline(ss, s_type, L',') ||
		!std::getline(ss, t_scale, L',') ||
		!std::getline(ss, s_scale, L',') ||
		!std::getline(ss, s_rot, L',')) {
		return nullptr;
	}

	float ft_scale = std::stof(t_scale);
	float fs_scale = std::stof(s_scale);
	float fs_rot = std::stof(s_rot);

	return MakeTomato(scene, t_type, s_type, ft_scale, fs_scale, fs_rot);
}

IFlyingObject::Ptr TomatoLaunchApi::GetTomatoFromMsg(const Scene::WeakPtr& scene, const std::wstring& msg) {
	std::wstringstream ss;
	std::wstring tomato, dx, dy, cx, cy;
	ss << msg.substr(2);

	if (!std::getline(ss, tomato, L';') ||
		!std::getline(ss, cx, L';') ||
		!std::getline(ss, cy, L';') ||
		!std::getline(ss, dx, L';') ||
		!std::getline(ss, dy, L';')) {
		return nullptr;
	}

	float fcx = std::stof(cx);
	float fcy = std::stof(cy);
	float fdx = std::stof(dx);
	float fdy = std::stof(dy);

	auto tt = MakeTomatoFromDesc(scene, tomato);
	if (!tt) {
		return nullptr;
	}

	tt->setPosition(fcx, fcy);
	tt->SetSpeed(sf::Vector2f(fdx, fdy));

	return tt;
}

std::wstring TomatoLaunchApi::LaunchTomatoMsg(const IFlyingObject::Ptr& obj, float dx, float dy) {
	std::wstringstream ss;

	ss << "$L" << GetTomatoTypeDesc(obj) << L";";
	
	ss.precision(2);
	ss << obj->getPosition().x << L";";
	ss << obj->getPosition().y << L";";
	ss << dx << L";";
	ss << dy << L";";

	return ss.str();
}