#pragma once
#include "UIElement.h"
#include <string>
class TextElement : public UIElement
{
public:
	virtual void RenderSettingsUI();
	virtual void Render();
	
	std::string text;
	std::string textCache;

	TextElement(std::string name, std::string text);
};

