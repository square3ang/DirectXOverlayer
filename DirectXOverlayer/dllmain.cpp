// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "minhook/include/MinHook.h"
#include "DirectXHook/DirectXHook.h"

#include "windows.h"

DWORD WINAPI HookThread(LPVOID lpParam)
{
    static Renderer renderer;
    static DirectXHook dxHook(&renderer);
    //static d3d11_impl impl;
    //dxHook.AddRenderCallback(&impl);
    dxHook.SetDrawExampleTriangle(false);
    dxHook.Hook();
    return 0;
}

void OpenDebugTerminal()
{
    if (AllocConsole())
    {
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        SetWindowText(GetConsoleWindow(), L"DirectXOverlayer Debug");
    }
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        //OpenDebugTerminal();
    }
    return TRUE;
}



extern "C" __declspec(dllexport) void Start() {
    MH_Initialize();
    CreateThread(0, 0, &HookThread, 0, 0, NULL);
}
