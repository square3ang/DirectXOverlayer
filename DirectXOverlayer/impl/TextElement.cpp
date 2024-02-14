#include "TextElement.h"
#include "../imgui/imgui.h"
#include "d3d11_impl.h"
#include <regex>
#include "../csscolorparser.hpp"
#include "../imgui/imgui_stdlib.h"

static std::unordered_map<std::string, std::pair< std::vector<std::pair<std::pair<int, int>, ImVec4>>, std::string>> colcache;
static std::regex rgx("<color\\s*=\\s*(.*?)[^>]*>(.*?)<\\/color>");

#define ApplyTags(str) ((const char*(*)(const char*))d3d11_impl::apiset["ApplyTags"])(str)


void TextElement::Render() {

	auto& io = ImGui::GetIO();

	

	ImVec2 startCurPos = ImGui::GetCursorPos();

	auto text = std::string(ApplyTags(this->text.c_str()));

	auto isincache = colcache.find(text) != colcache.end();

	auto lstsuf = text;

	std::vector<std::pair<std::pair<int, int>, ImVec4>> cols;

	std::vector<std::string> others;
	std::vector<std::string> matches;

	if (!isincache) {

		std::smatch match;

		while (std::regex_search(lstsuf, match, rgx)) {

			others.push_back(match.prefix());
			matches.push_back(match.str());

			lstsuf = match.suffix();
		}

		cols.reserve(matches.size());
	}

	
	

	std::string norichstring;
	if (isincache) {
		auto p = colcache[text];
		cols = p.first;
		norichstring = p.second;
	}
	else if (others.size() == 0) {
		norichstring = text;
	}
	else {
		auto i = 0;
		float xoff = 0;
		for (auto o : others) {
			norichstring += o;

			auto coltex = matches[i];

			//((void(*)(const char*))apiset["Log"])(coltex.c_str());

			std::string color = "";

			auto j = coltex.find_first_of('>');
			color = coltex.substr(7, j-7);
			j++;

			std::string just = "";

			just = coltex.substr(j, coltex.find_last_of('<')-j);

			//((void(*)(const char*))apiset["Log"])(color.c_str());

			auto col = CSSColorParser::parse(color);
			if (!col.has_value()) {
				norichstring += coltex;
			}
			else {
				auto colreal = col.value();

				auto start = norichstring.length();

				norichstring += just;

				auto end = norichstring.length() - 1;

				cols.push_back(std::make_pair(std::make_pair(start, end), ImVec4(colreal.r / 255.0f, colreal.g / 255.0f, colreal.b / 255.0f, colreal.a)));
			}


			i++;
		}

		norichstring += lstsuf;

		colcache[text] = std::make_pair(cols, norichstring);




	}

	auto norichcstr = norichstring.c_str();
	ImGui::SetCursorPos(ImVec2(startCurPos.x + 3 * io.DisplaySize.y / 900.0f, startCurPos.y + 3 * io.DisplaySize.y / 900.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.25f));
	ImGui::TextUnformatted(norichcstr);
	ImGui::PopStyleColor();


	ImGui::SetCursorPos(startCurPos);

	ImGui::TextUnformatted(norichcstr, NULL, &cols);

	
}

void TextElement::RenderSettingsUI() {
	ImGui::InputTextMultiline(GetTranslation("Text"), &textCache);
	if (ImGui::Button(GetTranslation("ApplyText"))) {
		text = textCache;
	}
}

TextElement::TextElement(std::string name, std::string text) : UIElement(name) {
	this->text = std::string(text);
	this->textCache = this->text;
}