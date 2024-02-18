#include "TextElement.h"
#include "../imgui/imgui.h"
#include "d3d11_impl.h"
#include <regex>
#include "../csscolorparser.hpp"
#include "../imgui/imgui_stdlib.h"


#define defaultText "<color=#&[FOHex]>&[CurOverload]</color> <color=#&[TEHex]>&[CurTE]</color> <color=#&[VEHex]>&[CurVE]</color> <color=#&[EPHex]>&[CurEP]</color> <color=#&[PHex]>&[CurP]</color> <color=#&[LPHex]>&[CurLP]</color> <color=#&[VLHex]>&[CurVL]</color> <color=#&[TLHex]>&[CurTL]</color> <color=#&[FMHex]>&[CurMiss]</color>"
static std::unordered_map<std::string, std::pair< std::vector<std::pair<std::pair<int, int>, ImVec4>>, std::string>> colcache;
static std::list<std::string> used;
static std::regex rgx("<color\\s*=\\s*(.*?)[^>]*>(.*?)<\\/color>");

#define ApplyTags(str, smig) ((const char*(*)(const char*, bool))d3d11_impl::apiset["ApplyTags"])(str, smig)


rapidjson::Value* TextElement::Save(rapidjson::Document* savefile)
{
	auto v = new rapidjson::Value(rapidjson::kObjectType);
	auto allocator = savefile->GetAllocator();
	v->AddMember(rapidjson::Value("text"), rapidjson::Value(text.c_str(), allocator), allocator);
	v->AddMember(rapidjson::Value("textNotPlaying"), rapidjson::Value(textNotPlaying.c_str(), allocator), allocator);
	v->AddMember(rapidjson::Value("fontSize"), rapidjson::Value(fontSize), allocator);
	v->AddMember(rapidjson::Value("textPivotX"), rapidjson::Value(textPivotX), allocator);

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

		used.push_back(text);
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
		used.push_back(text);



	}

	return std::make_pair(norichstring, cols);
}

std::vector<std::string> split(std::string input, char delimiter) {
	std::vector<std::string> answer;
	std::stringstream ss(input);
	std::string temp;

	while (getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}

float calcCurPosX(TextElement* element, std::string str, std::string fullstr) {
	auto& io = ImGui::GetIO();
	return (ImGui::CalcTextSize(fullstr.c_str()).x - ImGui::CalcTextSize(str.c_str()).x) * element->textPivotX;
}

void TextElement::Render() {


	auto& io = ImGui::GetIO();

	ImGui::PushFont(d3d11_impl::GetDefaultFont(fontSize * io.DisplaySize.y / 900.0f));


	

	auto isPlaying = ((bool*(*)())d3d11_impl::apiset["GetIsPlaying"])();

	auto simulatingInGame = d3d11_impl::GetIsSimulatingInGame();

	auto text = std::string(ApplyTags((isPlaying || simulatingInGame) ? this->text.c_str() : this->textNotPlaying.c_str(), simulatingInGame));

	

	auto pc = ParseColor(text);
	auto norichstring = pc.first;
	
	auto spl = split(norichstring, '\n');

	auto off = ImGui::GetCursorPos();

	auto inc = 3 * fontSize / 60 * io.DisplaySize.y / 900.0f;

	ImGui::SetCursorPos(ImVec2(0, inc));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.35f));
	
	

	for (auto a : spl) {
		
		ImGui::SetCursorPosX(off.x + calcCurPosX(this, a, norichstring) + inc);

		ImGui::TextUnformatted(a.c_str());
	}
	
	ImGui::PopStyleColor();

	ImGui::SetCursorPos(off);

	auto wow = std::vector<std::vector<std::pair<std::pair<int, int>, ImVec4>>>();
	auto spllst = std::vector<int>();

	for (auto a : spl) {
		//Log(std::to_string(a.length()).c_str());
		spllst.push_back(a.length());
	}

	auto c = std::vector<std::pair<std::pair<int, int>, ImVec4>>();
	auto idx = 0;
	for (auto a : pc.second) {
		
		if (a.first.first >= spllst[idx]) { 
			idx++;
			wow.push_back(std::vector<std::pair<std::pair<int, int>, ImVec4>>(c));
			c.clear();
		}

		if (idx - 1 > -1) {
			a.first.first -= spllst[idx - 1] + 1;
			a.first.second -= spllst[idx - 1] + 1;
		}


		c.push_back(a);
	}
	wow.push_back(c);
	

	
	auto i = 0;
	for (auto a : spl) {

		ImGui::SetCursorPosX(off.x + calcCurPosX(this, a, norichstring));

		if (wow.size() <= i) ImGui::TextUnformatted(a.c_str());
		else ImGui::TextUnformatted(a.c_str(), NULL, &wow[i]);
		i++;
	}


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

	if (useTextInput) {
		ImGui::InputFloat("Text Pivot X", &textPivotX);
	}
	else {
		ImGui::SliderFloat("Text Pivot X", &textPivotX, 0, 1);
	}
}

std::string TextElement::GetType() {
	return "TextElement";
}

void TextElement::LoadSettings(rapidjson::Value* obj)
{
	if (obj->HasMember("text"))
		text = (*obj)["text"].GetString();
	if (obj->HasMember("textNotPlaying"))
		textNotPlaying = (*obj)["textNotPlaying"].GetString();
	if (obj->HasMember("fontSize"))
		fontSize = (*obj)["fontSize"].GetFloat();
	if (obj->HasMember("textPivotX"))
		textPivotX = (*obj)["textPivotX"].GetFloat();
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
	auto inc = 3 * fontSize / 60 * io.DisplaySize.y / 900.0f;
	siz.x += inc;
	siz.y += inc;

	ImGui::PopFont();
	return siz;
}



void TextElement::CheckColCache()
{
	for (auto c : colcache) {
		if (std::find(used.begin(), used.end(), c.first) == used.end()) {
			colcache.erase(c.first);
		}
		
	}
	used.clear();
}

TextElement::TextElement(std::string name) : UIElement(name) {
	this->text = defaultText;
	this->textNotPlaying = "";
}