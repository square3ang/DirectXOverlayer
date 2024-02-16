#include "UIElement.h"
#include <optional>


void UIElement::Render() {

}

rapidjson::Value* UIElement::Save(rapidjson::Document* savefile)
{
	return nullptr;
}

std::string UIElement::GetType()
{
	return "Invalid Type";
}

void UIElement::LoadSettings(rapidjson::Value* obj)
{
}

void UIElement::RenderSettingsUI() {
	
}



UIElement::UIElement(std::string name) {
	this->name = std::string(name);
}