#include "stdafx.h"

const char* DataPath = "Data2";

DWORD XString__XString = 0x402285;
DWORD XomHelp__XomAppBase__GetInstance = 0x413444;

void __declspec(naked) InitOptionalPathsCodeCave()
{
    __asm {
        push    DataPath
        lea     ecx, dword ptr ss : [esp + 0x4]
        call    XString__XString
        call    XomHelp__XomAppBase__GetInstance
        mov     ecx, dword ptr ds : [eax]
        lea     edx, dword ptr ss : [esp]
        push    edx
        push    eax
        call    dword ptr ds : [ecx + 0xC]
        pop     ecx
        retn
    }
}

void Init()
{
    // WormsXApp::InitOptionalPaths
    injector::MakeJMP(0x4AB34C, InitOptionalPathsCodeCave);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        if (strcmp((char*)0x7766A8, "Worms 4: Mayhem!") == 0)
        {
            if (strcmp((char*)0x776908, "MenuTwkXPCEuro") == 0)
            {
                Init();
            }
        }
    }

    return TRUE;
}
