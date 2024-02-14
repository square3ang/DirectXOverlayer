#pragma once

#include <string>

class UIElement
{
public:
	virtual void RenderSettingsUI();
	virtual void Render();
	
	bool inited = false;
	std::string name;
	float x;
	float y;

	float actualX;
	float actualY;

	bool isSettingOpen = false;
	bool useTextInput = false;

	UIElement(std::string name);
};

