#include "TextElement.h"
#include "../imgui/imgui.h"
#include "d3d11_impl.h"
#include <regex>
#include "../csscolorparser.hpp"
#include "../imgui/imgui_stdlib.h"


#define defaultText "<color=#FF0000>D</color><color=#FF5F00>i</color><color=#FFBF00>r</color><color=#DFFF00>e</color><color=#7FFF00>c</color><color=#1FFF00>t</color><color=#00FF3F>X</color><color=#00FF9F>O</color><color=#00FEFF>v</color><color=#009FFF>e</color><color=#003FFF>r</color><color=#1F00FF>l</color><color=#7F00FF>a</color><color=#DF00FF>y</color><color=#FF00BF>e</color><color=#FF005F>r</color>"
static std::unordered_map<std::string, std::pair< std::vector<std::pair<std::pair<int, int>, ImVec4>>, std::string>> colcache;
static std::regex rgx("<color\\s*=\\s*(.*?)[^>]*>(.*?)<\\/color>");

#define ApplyTags(str, smig) ((const char*(*)(const char*, bool))d3d11_impl::apiset["ApplyTags"])(str, smig)


rapidjson::Value* TextElement::Save(rapidjson::Document* savefile)
{
	auto v = new rapidjson::Value(rapidjson::kObjectType);
	auto allocator = savefile->GetAllocator();
	v->AddMember(rapidjson::Value("text"), rapidjson::Value(text.c_str(), allocator), allocator);
	v->AddMember(rapidjson::Value("textNotPlaying"), rapidjson::Value(textNotPlaying.c_str(), allocator), allocator);
	v->AddMember(rapidjson::Value("fontSize"), rapidjson::Value(fontSize), allocator);

	return v;
}

std::pair<std::string, std::vector<std::pair<std::pair<int, int>, ImVec4>>> ParseColor(std::string text) {
	std::vector<std::pair<std::pair<int, int>, ImVec4>> cols;

	std::vector<std::string> others;
	std::vector<std::string> matches;

	auto lstsuf = text;

	auto isincache = colcache.find(text) != colcache.end();

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
			color = coltex.substr(7, j - 7);
			j++;

			std::string just = "";

			just = coltex.substr(j, coltex.find_last_of('<') - j);

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

	return std::make_pair(norichstring, cols);
}

void TextElement::Render() {


	auto& io = ImGui::GetIO();

	ImGui::PushFont(d3d11_impl::GetDefaultFont(fontSize * io.DisplaySize.y / 900.0f));

	ImVec2 startCurPos = ImGui::GetCursorPos();

	auto isPlaying = ((bool*(*)())d3d11_impl::apiset["GetIsPlaying"])();

	auto simulatingInGame = d3d11_impl::GetIsSimulatingInGame();

	auto text = std::string(ApplyTags((isPlaying || simulatingInGame) ? this->text.c_str() : this->textNotPlaying.c_str(), simulatingInGame));

	

	auto pc = ParseColor(text);
	auto norichstring = pc.first;
	

	auto norichcstr = norichstring.c_str();
	ImGui::SetCursorPos(ImVec2(startCurPos.x + 3 * io.DisplaySize.y / 900.0f, startCurPos.y + 3 * io.DisplaySize.y / 900.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.25f));
	ImGui::TextUnformatted(norichcstr);
	ImGui::PopStyleColor();


	ImGui::SetCursorPos(startCurPos);

	ImGui::TextUnformatted(norichcstr, NULL, &pc.second);

	ImGui::PopFont();
}

void TextElement::RenderSettingsUI() {
	if (ImGui::Button(GetTranslation("TextEdit"))) {
		OpenEditText(&text);
	}
	if (ImGui::Button(GetTranslation("NotPlayingTextEdit"))) {
		OpenEditText(&textNotPlaying);
	}
	ImGui::InputFloat(GetTranslation("FontSize"), &fontSize);
}

std::string TextElement::GetType() {
	return "TextElement";
}

void TextElement::LoadSettings(rapidjson::Value* obj)
{
	text = (*obj)["text"].GetString();
	textNotPlaying = (*obj)["textNotPlaying"].GetString();
	fontSize = (*obj)["fontSize"].GetFloat();
}

ImVec2 TextElement::GetSize()
{
	auto& io = ImGui::GetIO();

	ImGui::PushFont(d3d11_impl::GetDefaultFont(fontSize * io.DisplaySize.y / 900.0f));
	auto isPlaying = ((bool* (*)())d3d11_impl::apiset["GetIsPlaying"])();

	auto simulatingInGame = d3d11_impl::GetIsSimulatingInGame();

	auto text = ApplyTags((isPlaying || simulatingInGame) ? this->text.c_str() : this->textNotPlaying.c_str(), simulatingInGame);

	auto pc = ParseColor(text);
	auto norichstring = pc.first;

	auto siz = ImGui::CalcTextSize(norichstring.c_str());
	ImGui::PopFont();
	return siz;
}

TextElement::TextElement(std::string name) : UIElement(name) {
	this->text = defaultText;
	this->textNotPlaying = defaultText;
	this->fontSize = 60;
}