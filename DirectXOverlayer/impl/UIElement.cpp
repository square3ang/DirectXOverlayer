#include "UIElement.h"
#include <optional>
#include "../imgui/imgui.h"


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

ImVec2 UIElement::GetSize() {
	return ImVec2(0,0);
}



UIElement::UIElement(std::string name) {
	this->name = std::string(name);
}