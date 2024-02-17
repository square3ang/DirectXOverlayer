#include "d3d11_impl.h"
#include <d3d11.h>
#include <assert.h>




#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"

#include "../csscolorparser.hpp"

#include <unordered_map>



#include <stdio.h>
#include <bitset>
#include <regex>

#include <string>

#include "UIElement.h"
#include "TextElement.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"
#include <queue>


std::unordered_map<std::string, void*> d3d11_impl::apiset;
bool d3d11_impl::simulateInGame = false;
static std::unordered_map<std::string, ImFont*> fontmap;

static std::list<UIElement*> elements;

static int lastScreenY = 0;
static int lastScreenX = 0;

static bool lastIsSetting = false;

WNDPROC oWndProc;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



extern "C" __declspec(dllexport) void RegisterAPI(void* func, const char* APIName) {
	printf("API REGISTER: %s, %p\n", APIName, func);
	d3d11_impl::apiset[APIName] = func;
}

extern "C" __declspec(dllexport) void SetStringWithReference(std::string * reference, const char* str) {
	*reference = std::string(str);
}

extern "C" __declspec(dllexport) const char* GetStringWithReference(std::string * reference) {
	return reference->c_str();
}

extern "C" __declspec(dllexport) char* Save() {
	auto savefile = rapidjson::Document(rapidjson::kObjectType);
	auto allocator = savefile.GetAllocator();

	rapidjson::Value elems(rapidjson::kArrayType);
	for (auto elem : elements) {
		auto sav = elem->Save(&savefile);
		sav->AddMember(rapidjson::Value("type"), rapidjson::Value(elem->GetType().c_str(), allocator), allocator);
		sav->AddMember(rapidjson::Value("name"), rapidjson::Value(elem->name.c_str(), allocator), allocator);
		sav->AddMember(rapidjson::Value("x"), rapidjson::Value(elem->x), allocator);
		sav->AddMember(rapidjson::Value("y"), rapidjson::Value(elem->y), allocator);
		sav->AddMember(rapidjson::Value("pivotX"), rapidjson::Value(elem->pivotX), allocator);
		sav->AddMember(rapidjson::Value("pivotY"), rapidjson::Value(elem->pivotY), allocator);
		elems.PushBack(*sav, allocator);
		delete sav;
	}
	savefile.AddMember(rapidjson::Value("elements"), elems, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	savefile.Accept(writer);
	auto str = buffer.GetString();
	auto len = strlen(str)+1;
	auto mem = (char*)malloc(len);
	assert(mem != 0);
	strcpy_s(mem, len, str);
	return mem;
}

extern "C" __declspec(dllexport) void FreeMemory(void* ptr) {
	free(ptr);
}

UIElement* MakeElementByType(std::string type, std::string name) {
	if (type == "TextElement") {
		return new TextElement(name);
	}

}

extern "C" __declspec(dllexport) void LoadSave(const char* json) {
	rapidjson::Document d;
	d.Parse(json);

	auto elems = d["elements"].GetArray();

	for (rapidjson::Value::ConstValueIterator itr = elems.Begin(); itr != elems.End(); ++itr) { // Ok
		auto ob = itr->GetObj();
		
		auto newelem = MakeElementByType(ob["type"].GetString(), ob["name"].GetString());

		if (ob.HasMember("x"))
			newelem->x = ob["x"].GetFloat();
		if (ob.HasMember("y"))
			newelem->y = ob["y"].GetFloat();
		if (ob.HasMember("pivotX"))
			newelem->pivotX = ob["pivotX"].GetFloat();
		if (ob.HasMember("pivotY"))
			newelem->pivotY = ob["pivotY"].GetFloat();

		newelem->LoadSettings(const_cast<rapidjson::Value*>(itr));

		elements.push_back(newelem);
	}
}

static ImFontAtlas* alt;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	//printf("a %d\n", uMsg);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

ImFont* AddDefaultFont(float pixel_size, bool sdf)
{
	ImGuiIO& io = ImGui::GetIO();
	auto modpath = std::string(((const char* (*)())d3d11_impl::apiset["GetModPath"])());
	ImFontConfig config;
    config.SignedDistanceFont = sdf;
	ImFont* font = (sdf ? alt : io.Fonts)->AddFontFromFileTTF(((modpath + "\\godoMaum.ttf").c_str()), pixel_size, &config, io.Fonts->GetGlyphRangesKorean());
	return font;
}


static bool fntLoaded = false;
static bool actReload = false;

/*ImFont* d3d11_impl::GetArialFont(float pixel_size) {
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = pixel_size / 64.0f;
	return fontmap[fntLoaded ? "Arial" : "ArialNP"];
}*/

ImFont* d3d11_impl::GetDefaultFont(float pixel_size) {
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = pixel_size / 32.0f;
	return fontmap[fntLoaded ? "Default" : "DefaultNP"];
}

bool d3d11_impl::GetIsSimulatingInGame()
{
	auto issetting = ((bool(*)())apiset["GetIsSetting"])();
	return d3d11_impl::simulateInGame && issetting;
}



DWORD WINAPI LoadFontAsync(LPVOID lpParam)
{
	if (alt == nullptr) {
		alt = new ImFontAtlas();
	}
	Log("Loading godoMaum (Default, Hangul and English)");
	fontmap["Default"] = AddDefaultFont(32, true);
	Log("godoMaum Loaded!");
	ImGuiIO& io = ImGui::GetIO();

	Log("Building Fonts...");
	alt->Build();
	Log("Done!");

	fontmap.erase("ArialNP");
	fontmap.erase("DefaultNP");

	fntLoaded = true;
	actReload = true;
	

	((void(*)(bool))d3d11_impl::apiset["SetInitialized"])(true);

	return 0;
}

void d3d11_impl::Render(Renderer* renderer)
{
	static bool inited = false;



	if (!inited) {
		//InitFramework(renderer->d3d11Device, spriteBatch, window);

		ImGui::CreateContext();

		ImGui_ImplWin32_Init(renderer->window);
		ImGui_ImplDX11_Init(renderer->d3d11Device.Get(), renderer->d3d11Context.Get(), ImGuiBackendFlags_DefaultDesktop);


		oWndProc = (WNDPROC)SetWindowLongPtr(renderer->window, GWLP_WNDPROC, (LONG_PTR)WndProc);



		((void(*)())apiset["HelloWorld"])();

		//fontmap["ArialNP"] = AddArialFont(32, false);
		fontmap["DefaultNP"] = AddDefaultFont(32, false);

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Build();

		CreateThread(0, 0, &LoadFontAsync, 0, 0, NULL);


		/*io.Fonts->Build();
		ImGui_ImplDX11_CreateDeviceObjects();*/

		inited = true;
	}

	//if (!fntLoaded) return;

	ImGuiIO& io = ImGui::GetIO();

	if (actReload) {
		io.Fonts->Clear();
		delete io.Fonts;
		io.Fonts = alt;
		ImGui_ImplDX11_CreateDeviceObjects();
		actReload = false;
	}



	if (((bool(*)())apiset["GetIsEditingText"])()) return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();


	ImGui::NewFrame();



	//auto sz = ImGui::CalcTextSize(teststring);

	//ImGui::SetNextWindowSize(sz, 0);
	auto issetting = ((bool(*)())apiset["GetIsSetting"])();

	// Show Setting Window
	if (issetting) {

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.25f, 0.25f, 1.0f));

		ImGui::PushFont(GetDefaultFont(25.0f * io.DisplaySize.y / 900.0f));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 5, io.DisplaySize.y / 2));
		ImGui::Begin(GetTranslation("DXOverlayerSetting"), NULL, ImGuiWindowFlags_NoResize);
		if (ImGui::Button(GetTranslation("Done")) || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
			((void(*)())apiset["EndSetting"])();
		}

		if (ImGui::Button(GetTranslation("AddText"))) {
			auto name = std::string(GetTranslation("Text")) + " " + std::to_string(elements.size());
			auto elem = new TextElement(name);
			elem->x = 0.5;
			elem->y = 0.5;

			elements.push_back(elem);
			Log((name + " added").c_str());
		}

		ImGui::Checkbox(GetTranslation("SimulateInGame"), &simulateInGame);

		ImGui::TextUnformatted(GetTranslation("Elements"));


		std::queue<UIElement*> deleted;
		for (auto elem : elements) {
			std::ostringstream address;
			address << (void const*)elem;
			std::string addrstr = address.str();

			ImGui::SetNextItemWidth(io.DisplaySize.x / 5 - (200 * 900 / io.DisplaySize.y));

			ImGui::TextUnformatted(elem->name.c_str());
			ImGui::SameLine();
			if (ImGui::Button((std::string(GetTranslation("EditName")) + "##" + addrstr).c_str())) {
				OpenEditText(&elem->name);
			}
			ImGui::SameLine();
			if (ImGui::Button((std::string(GetTranslation("Settings")) + "##" + addrstr).c_str())) {
				elem->isSettingOpen = true;
			}
			ImGui::SameLine();
			if (ImGui::Button((std::string(GetTranslation("Delete")) + "##" + addrstr).c_str())) {
				deleted.push(elem);
			}
		}


		while (!deleted.empty()) {
			auto fst = deleted.front();
			elements.remove(fst);
			delete fst;
			deleted.pop();
		}


		ImGui::End();

		for (auto elem : elements) {
			if (elem->isSettingOpen) {
				std::ostringstream address;
				address << (void const*)elem;
				std::string addrstr = address.str();

				ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5, 0.5));
				ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 5, io.DisplaySize.y / 2));
				ImGui::Begin((elem->name + " " + std::string(GetTranslation("Settings")) + "###" + addrstr + "_Settings").c_str(), NULL, ImGuiWindowFlags_NoResize);

				if (ImGui::Button(GetTranslation("Close"))) {
					elem->isSettingOpen = false;
				}

				ImGui::Checkbox(GetTranslation("UseTextInput"), &elem->useTextInput);

				if (elem->useTextInput) {
					ImGui::InputFloat("X", &elem->x);
					ImGui::InputFloat("Y", &elem->y);
					ImGui::InputFloat("Pivot X", &elem->pivotX);
					ImGui::InputFloat("Pivot Y", &elem->pivotY);
				}
				else {
					ImGui::SliderFloat("X", &elem->x, 0, 1);
					ImGui::SliderFloat("Y", &elem->y, 0, 1);
					ImGui::SliderFloat("Pivot X", &elem->pivotX, 0, 1);
					ImGui::SliderFloat("Pivot Y", &elem->pivotY, 0, 1);
				}
				
				

				elem->RenderSettingsUI();

				ImGui::End();
			}

		}

		ImGui::PopFont();

		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();

		// Will Change the Next Window
		ImGui::PushStyleVar(ImGuiStyleVar_WindowShadowSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	

	
	
	// Render
	for (auto elem : elements) {
		if (issetting) ImGui::SetNextWindowBgAlpha(0.0f);
		
		
		std::ostringstream address;
		address << (void const*)elem;
		std::string name = address.str();

		auto wind = ImGui::FindWindowByName(name.c_str());

		if (issetting != lastIsSetting && issetting && wind != nullptr && elem->inited && io.DisplaySize.x == lastScreenX && io.DisplaySize.y == lastScreenY && (wind->Pos.x != elem->actualX || wind->Pos.y != elem->actualY)) {
			auto posafter = ImVec2(wind->Pos);
			posafter = ImVec2(posafter.x + wind->Size.x * elem->pivotX, posafter.y + wind->Size.y * elem->pivotY);
			elem->x = posafter.x / io.DisplaySize.x;
			elem->y = posafter.y / io.DisplaySize.y;
			elem->actualX = wind->Pos.x;
			elem->actualY = wind->Pos.y;
		}
		else {
			ImGui::SetNextWindowPos(ImVec2(elem->x * io.DisplaySize.x, elem->y * io.DisplaySize.y), NULL, ImVec2(elem->pivotX, elem->pivotY));
		}

		if (!elem->inited) {
			elem->inited = true;
		}

		if (wind != nullptr) {
			ImGui::SetNextWindowSize(elem->GetSize());
		}
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
		ImGui::Begin(name.c_str(), NULL, (issetting ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		elem->Render();
		
		ImGui::End();
		ImGui::PopStyleVar();


	}

	


	
	if (issetting) {
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);
	}

	lastScreenX = io.DisplaySize.x;
	
	lastScreenY = io.DisplaySize.y;

	ImGui::Render();

	renderer->d3d11Context->OMSetRenderTargets(1, renderer->d3d11RenderTargetViews[renderer->bufferIndex].GetAddressOf(), renderer->depthStencilView.Get());

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	lastIsSetting = issetting;

}

