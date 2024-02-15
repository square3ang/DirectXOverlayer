#pragma once
#include "UIElement.h"
#include <string>
class TextElement : public UIElement
{
public:
	virtual void RenderSettingsUI();
	virtual void Render();
	
	std::string text;
	std::string textNotPlaying;
	float fontSize;

	TextElement(std::string name);
};

