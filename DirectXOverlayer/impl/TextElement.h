#pragma once
#include "UIElement.h"
#include "../imgui/imgui.h"
#include <string>
class TextElement : public UIElement
{
public:
	virtual void RenderSettingsUI();
	virtual void Render();
	virtual rapidjson::Value* Save(rapidjson::Document* savefile);
	virtual std::string GetType();
	virtual void LoadSettings(rapidjson::Value* obj);
	virtual ImVec2 GetSize();
	
	std::string text;
	std::string textNotPlaying;
	float fontSize;

	TextElement(std::string name);
};

