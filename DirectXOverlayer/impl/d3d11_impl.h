#ifndef __D3D11_IMPL_H__
#define __D3D11_IMPL_H__

#pragma once

#define Log(str) ((void(*)(const char*))d3d11_impl::apiset["Log"])(str)
#define GetTranslation(key) ((const char*(*)(const char*))d3d11_impl::apiset["GetTranslation"])(key)
#define OpenEditText(str) ((void(*)(std::string*))d3d11_impl::apiset["OpenEditText"])(str)

#include "../DirectXHook/Renderer.h"
#include "../DirectXHook/IRenderCallback.h"
#include "../DirectXHook/OverlayFramework.h"

#include "../imgui/imgui.h"

class d3d11_impl
{
public:
	void Render(Renderer* renderer);
	static ImFont* GetDefaultFont(float pixel_size);
	//static ImFont* GetArialFont(float pixel_size);
	static std::unordered_map<std::string, void*> apiset;
};

#endif // __D3D11_IMPL_H__