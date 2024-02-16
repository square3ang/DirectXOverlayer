#pragma once

#include <string>
#include "../rapidjson/document.h"

class UIElement
{
public:
	virtual void RenderSettingsUI();
	virtual void Render();
	virtual rapidjson::Value* Save(rapidjson::Document* savefile);
	virtual std::string GetType();
	virtual void LoadSettings(rapidjson::Value* obj);
	
	bool inited = false;
	std::string name;
	float x = 0;
	float y = 0;

	float actualX = 0;
	float actualY = 0;

	bool isSettingOpen = false;
	bool useTextInput = false;

	UIElement(std::string name);
};

