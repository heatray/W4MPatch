#include "stdafx.h"

float AspectRatio43 = 4.0f / 3.0f;
float AspectRatio34 = 3.0f / 4.0f;
float F2 = 2.0f;
struct Screen
{
    int Width = 800;
    int Height = 600;
    float AspectRatioX = 4.0f / 3.0f;
    float AspectRatioY = 3.0f / 4.0f;
} Screen;
struct Frustum
{
    float Left;
    float Right;
    float Bottom;
    float Top;
};
struct Frustum Frustum = { -0.64f, 0.64f, -0.48f, 0.48f };
struct Frustum ShrinkHUD = { -0.7f, 0.7f, -0.525f, 0.525f };
struct MovieRect
{
    int vX = 0;
    int vY = 0;
    int vW = 640;
    int vH = 480;
} MovieRect;
bool AspectRatioFix, LandmineLodFix, EnableAllLanguages;
uint8_t FrameInterval;

bool Borderless;

std::string GameSpyHost, MotdUrl;
struct GameSpy
{
    char AvailableCheck[127] = "%s.available."; // Master - Available Check
    char ServerBrowser[127] = "%s.ms%d.";       // Master - Server Browser
    char QueryReport[127] = "%s.master.";       // Master - Query Report 2
    char ConnectionManager[127] = "gpcm.";      // Presence - Connection Manager
    char SearchPlayer[127] = "gpsp.";           // Presence - Search Player
    char PeerChat[127] = "peerchat.";           // Chat
    char MotdUrl[255];                          // Team17 MOTD
} GS;

float WXFE_ZBuffer_Scale = 2000.0f;
float HUD_Blank_Width = 1280.0f;
//float HUD_Blank_Height = 480.0f;
float EFMV_Border_Width = 900.0f;

float HUD_SecondWeapon_Back_PosX, \
      HUD_SecondWeapon_Back_PosY;

DWORD** XomApp__c_pTheInstance = (DWORD**)0x8506D8;
DWORD AspectRatioCodeCaveExit = 0x4AB897;
DWORD FrustumCodeCaveExit = 0x4ABCE3;
float* XCamera__Aperture = (float*)0x7EED38;
DWORD SetFromSceneCameraCodeCaveExit = 0x43458F;
DWORD MoviePlayerPCCodeCaveExit = 0x6B7023;

void __declspec(naked) AspectRatioCodeCave()
{
    __asm {
        mov     edx, dword ptr ds : [0x8506D8]
        mov     edx, [edx + 0x2C]
        mov     Screen.Width, edx
        mov     edx, dword ptr ds : [0x8506D8]
        mov     edx, [edx + 0x30]
        mov     Screen.Height, edx
        mov     edx, 0x7E0114
        fild    dword ptr ds : [Screen.Height]
        fidiv   dword ptr ds : [Screen.Width]
        fld     dword ptr ds : [AspectRatio34]
        fcomip  st(0), st(1)
        fstp    st(0)
        jne     AspectRatioX
        jmp     AspectRatioCodeCaveExit

    AspectRatioX :
        jb      AspectRatioY
        fild    dword ptr ds : [Screen.Width]
        fidiv   dword ptr ds : [Screen.Height]
        fst     dword ptr ds : [Screen.AspectRatioX]
        fld     st(0)
        fld     st(0)
        fmul    dword ptr ds : [Frustum.Bottom]
        fstp    dword ptr ds : [Frustum.Left]
        fmul    dword ptr ds : [Frustum.Top]
        fstp    dword ptr ds : [Frustum.Right]
        fmul    dword ptr ds : [ShrinkHUD.Top]
        fstp    dword ptr ds : [ShrinkHUD.Right]
        jmp     AspectRatioCodeCaveExit

    AspectRatioY :
        fild    dword ptr ds : [Screen.Height]
        fidiv   dword ptr ds : [Screen.Width]
        fst     dword ptr ds : [Screen.AspectRatioY]
        fld     st(0)
        fld     st(0)
        fmul    dword ptr ds : [Frustum.Left]
        fstp    dword ptr ds : [Frustum.Bottom]
        fmul    dword ptr ds : [Frustum.Right]
        fstp    dword ptr ds : [Frustum.Top]
        fmul    dword ptr ds : [ShrinkHUD.Right]
        fstp    dword ptr ds : [ShrinkHUD.Top]
        jmp     AspectRatioCodeCaveExit
    }
}

void __declspec(naked) FrustumCodeCave()
{
    __asm {
        fld     dword ptr ds : [Frustum.Left]
        fstp    dword ptr ds : [esp + 0x24]
        fld     dword ptr ds : [Frustum.Right]
        fstp    dword ptr ds : [esp + 0x28]
        fld     dword ptr ds : [Frustum.Bottom]
        fstp    dword ptr ds : [esp + 0x2C]
        fld     dword ptr ds : [Frustum.Top]
        fstp    dword ptr ds : [esp + 0x30]
        jmp     FrustumCodeCaveExit
    }
}

void __declspec(naked) SetFromSceneCameraCodeCave()
{
    __asm {
        fld     dword ptr ds : [ecx + 0x30]
        mov     eax, esi
        fmul    dword ptr ds : [0x7EED38]
        fmul    dword ptr ds : [Screen.AspectRatioX]
        fdiv    st(0), st(1)
        fstp    dword ptr ss : [ebp - 0x4]
        fld     dword ptr ds : [ecx + 0x2C]
        fmul    dword ptr ds : [0x7EED38]
        fmul    dword ptr ds : [Screen.AspectRatioY]
        jmp     SetFromSceneCameraCodeCaveExit
    }
}

void __declspec(naked) MoviePlayerPCCodeCave()
{
    __asm {
        mov     edx, dword ptr ss : [esp + 0x20] // Right
        mov     dword ptr ds : [MovieRect.vW], edx
        mov     edx, dword ptr ss : [esp + 0x24] // Bottom
        mov     dword ptr ds : [MovieRect.vH], edx
        fild    dword ptr ds : [MovieRect.vH]
        fidiv   dword ptr ds : [MovieRect.vW]
        fld     dword ptr ds : [AspectRatio34]
        fcomip  st(0), st(1)
        fstp    st(0)
        jne     AspectRatioX
        jmp     MoviePlayerEnd

    AspectRatioX :
        // vW = B * (4 / 3)     vH = B
        // vX = (R - vW) / 2    vY = 0
        jb      AspectRatioY
        fild    dword ptr ss : [esp + 0x20] // Width
        fild    dword ptr ss : [esp + 0x24] // Height
        fld     dword ptr ds : [AspectRatio43]
        fmul    st(0), st(1)
        fistp   dword ptr ds : [MovieRect.vW]
        fistp   dword ptr ds : [MovieRect.vH]
        fisub   dword ptr ds : [MovieRect.vW]
        fld     dword ptr ds : [F2]
        fdivp   st(1), st(0)
        fistp   dword ptr ds : [MovieRect.vX]
        jmp     MoviePlayerEnd

    AspectRatioY :
        // vW = R    vH = R * (3 / 4)
        // vX = 0    vY = (B - vH) / 2
        fild    dword ptr ss : [esp + 0x24] // Height
        fild    dword ptr ss : [esp + 0x20] // Width
        fld     dword ptr ds : [AspectRatio34]
        fmul    st(0), st(1)
        fistp   dword ptr ds : [MovieRect.vH]
        fistp   dword ptr ds : [MovieRect.vW]
        fisub   dword ptr ds : [MovieRect.vH]
        fld     dword ptr ds : [F2]
        fdivp   st(1), st(0)
        fistp   dword ptr ds : [MovieRect.vY]
        jmp     MoviePlayerEnd

    MoviePlayerEnd :
        mov     edx, dword ptr ds : [MovieRect.vH]
        push    edx
        mov     edx, dword ptr ds : [MovieRect.vW]
        push    edx
        mov     edx, dword ptr ds : [MovieRect.vY]
        push    edx
        mov     edx, dword ptr ds : [MovieRect.vX]
        push    edx
        jmp     MoviePlayerPCCodeCaveExit
    }
}

void Init()
{
    CIniReader iniReader("");

    AspectRatioFix = iniReader.ReadInteger("Main", "AspectRatioFix", 0) == 1;
    LandmineLodFix = iniReader.ReadInteger("Main", "LandmineLodFix", 0) == 1;
    EnableAllLanguages = iniReader.ReadInteger("Main", "EnableAllLanguages", 0) == 1;
    FrameInterval = iniReader.ReadInteger("Main", "FrameInterval", 16);
    Borderless = iniReader.ReadInteger("Window", "Borderless", 0) == 1;
    GameSpyHost = iniReader.ReadString("GameSpy", "Host", "gamespy.com");
    MotdUrl = iniReader.ReadString("GameSpy", "MotdUrl", "http://www.team17.com/gamespy/mayhem/motd/");
    HUD_SecondWeapon_Back_PosX = iniReader.ReadFloat("HUD", "SecondWeapon.Back.PosX", 220.0f);
    HUD_SecondWeapon_Back_PosY = iniReader.ReadFloat("HUD", "SecondWeapon.Back.PosY", 200.0f);

    // Main
    if (AspectRatioFix)
    {
        // AppDataService::AppInit
        injector::MakeJMP(0x4AB892, AspectRatioCodeCave);

        // AppDataService::InitScene
        injector::MakeJMP(0x4ABCC3, FrustumCodeCave);                    // Background
        injector::WriteMemory<FLOAT*>(0x4AC223, &Frustum.Right, true);   // Menu Front
        injector::WriteMemory<FLOAT*>(0x4AC246, &Frustum.Top, true);     // Menu Front
        injector::WriteMemory<FLOAT*>(0x4AC218, &ShrinkHUD.Right, true); // SHRINKHUD
        injector::WriteMemory<FLOAT*>(0x4AC23C, &ShrinkHUD.Top, true);   // SHRINKHUD

        // FrontEndService
        injector::WriteMemory<FLOAT>(0x5BE8E3, WXFE_ZBuffer_Scale, true); // Menu BG ZBuffer

        // PopUpService
        injector::WriteMemory<FLOAT>(0x5E0D22, WXFE_ZBuffer_Scale, true); // Popup BG ZBuffer

        // WXFE_MenuEntity
        injector::WriteMemory<FLOAT>(0x6C1A66, WXFE_ZBuffer_Scale, true); // Menu Tint

        // CameraManagerService::CameraManagerService
        injector::WriteMemory<FLOAT*>(0x5A8CD7, &Frustum.Right, true);   // HUD
        injector::WriteMemory<FLOAT*>(0x5A8D10, &Frustum.Top, true);     // HUD
        injector::WriteMemory<FLOAT*>(0x5A8CCD, &ShrinkHUD.Right, true); // SHRINKHUD
        injector::WriteMemory<FLOAT*>(0x5A8D06, &ShrinkHUD.Top, true);   // SHRINKHUD

        // HudService::Initialize
        injector::WriteMemory<FLOAT>(0x54FBCA, HUD_Blank_Width, true);  // HUD Tint
        // injector::WriteMemory<FLOAT>(0x54FBC5, HUD_Blank_Height, true); // HUD Tint

        // EfmvBorderEntity::EfmvBorderEntity
        injector::WriteMemory<FLOAT*>(0x54314A, &EFMV_Border_Width, true);

        // XCamera::SetFromSceneCamera
        injector::MakeJMP(0x434576, SetFromSceneCameraCodeCave);

        // MoviePlayerPC::Open
        injector::MakeJMP(0x6B7013, MoviePlayerPCCodeCave);
    }

    if (LandmineLodFix)
    {
        injector::WriteMemory<BYTE>(0x56340A, 0xEB, true);
    }

    if (EnableAllLanguages)
    {
        injector::WriteMemory<BYTE>(0x4AB37F, 0xC, true);    // cmp esi, u12
        injector::WriteMemory<WORD>(0x4AB389, 0x07EB, true); // jmp 0x9
        injector::MakeNOP(0x4AB38B, 5);
        injector::WriteMemory<BYTE>(0x4AD72C, 0xC, true);    // cmp eax, u12
        injector::WriteMemory<WORD>(0x4AD736, 0x07EB, true); // jmp 0x9
        injector::MakeNOP(0x4AD738, 5);
    }

    if (FrameInterval != 16)
    {
        injector::WriteMemory<BYTE>(0x4AC715, FrameInterval, true);
        injector::WriteMemory<BYTE>(0x4AC719, FrameInterval, true);
    }

    // Window
    if (Borderless)
    {
        injector::WriteMemory<BYTE>(0x4699C0, 0, true);    // Pos Y
        injector::WriteMemory<BYTE>(0x4699C2, 0, true);    // Pos X
        injector::MakeNOP(0x462063, 9);
        injector::MakeNOP(0x462076, 6);
        injector::WriteMemory<BYTE>(0x462071, 0x0B, true); // dwStyle
        injector::WriteMemory<BYTE>(0x462076, 0x5E, true); // pop esi
    }

    // GameSpy
    if (GameSpyHost != "gamespy.com")
    {
        strcat_s(GS.AvailableCheck, GameSpyHost.c_str());
        strcat_s(GS.ServerBrowser, GameSpyHost.c_str());
        strcat_s(GS.QueryReport, GameSpyHost.c_str());
        strcat_s(GS.ConnectionManager, GameSpyHost.c_str());
        strcat_s(GS.SearchPlayer, GameSpyHost.c_str());
        strcat_s(GS.PeerChat, GameSpyHost.c_str());
        injector::WriteMemory(0x6BE10A, &GS.AvailableCheck, true);
        injector::WriteMemory(0x6D837B, &GS.ServerBrowser, true);
        injector::WriteMemory(0x6BDB52, &GS.QueryReport, true);
        injector::WriteMemory(0x6CA232, &GS.ConnectionManager, true);
        injector::WriteMemory(0x6CAE70, &GS.SearchPlayer, true);
        injector::WriteMemory(0x6D2E09, &GS.PeerChat, true);
        injector::WriteMemory(0x6D2E58, &GS.PeerChat, true);
        injector::WriteMemory(0x6D2E94, &GS.PeerChat, true);
    }

    // Team17 GameSpy Stuff
    if (MotdUrl != "http://www.team17.com/gamespy/mayhem/motd/")
    {
        strcpy_s(GS.MotdUrl, MotdUrl.c_str());
        injector::WriteMemory(0x672E3F, &GS.MotdUrl, true);
    }

    // HUD Tweaks
    if (HUD_SecondWeapon_Back_PosX != 220.0f)
    {
        // SecondaryWeaponGraphicEntity
        injector::WriteMemory<FLOAT>(0x4E58CF, HUD_SecondWeapon_Back_PosX, true);
    }
    if (HUD_SecondWeapon_Back_PosY != 200.0f)
    {
        // SecondaryWeaponGraphicEntity
        injector::WriteMemory<FLOAT>(0x4E58D7, HUD_SecondWeapon_Back_PosY, true);
    }
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
