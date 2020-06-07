#pragma once

#include "SceneElements.h"

class TomatoLaunchApi
{
public:
	static ISplashObject::Ptr MakeSplash(const std::wstring& type, float scale, float rot);
	static IFlyingObject::Ptr MakeTomato(const Scene::WeakPtr& scene, const std::wstring& type, const std::wstring& splash_type, float t_scale, float s_scale, float s_rot);

	static std::wstring GetTomatoTypeDesc(const IFlyingObject::Ptr& obj);
	static IFlyingObject::Ptr MakeTomatoFromDesc(const Scene::WeakPtr& scene, const std::wstring& str);

	static IFlyingObject::Ptr GetTomatoFromMsg(const Scene::WeakPtr& scene, const std::wstring& msg);
	static std::wstring LaunchTomatoMsg(const IFlyingObject::Ptr& obj, float dx, float dy);
};

