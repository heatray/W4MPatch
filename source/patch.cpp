#include "stdafx.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

bool AspectRatioFix, LandmineLodFix, EnableAllLanguages, SleepFix, Borderless;
uint8_t FrameInterval;
struct XomApp
{
    char pad_0[0x2C];
    int RenderWidth;
    int RenderHeight;
};
float AspectRatioX = 4.0f / 3.0f;
float AspectRatioY = 3.0f / 4.0f;
struct Frustum
{
    float Left;
    float Right;
    float Bottom;
    float Top;
};
struct Frustum Frustum = {-0.64f, 0.64f, -0.48f, 0.48f};
struct Frustum ShrinkHUD = {-0.7f, 0.7f, -0.525f, 0.525f};
tagRECT MovieRect;
std::string GameSpyHost, MotdUrl;
struct GameSpy
{
    std::string AvailableCheck = "%s.available."; // Master - Available Check
    std::string ServerBrowser = "%s.ms%d.";       // Master - Server Browser
    std::string QueryReport = "%s.master.";       // Master - Query Report 2
    std::string ConnectionManager = "gpcm.";      // Presence - Connection Manager
    std::string SearchPlayer = "gpsp.";           // Presence - Search Player
    std::string PeerChat = "peerchat.";           // Chat
} GS;

float HUD_SecondWeapon_Back_PosX, HUD_SecondWeapon_Back_PosY;

void __declspec(naked) AspectRatioCodeCave()
{
    static const DWORD Exit = 0x4AB89C;
    static const DWORD sub_4045B5 = 0x4045B5;
    static XomApp* App;
    static int Width, Height;

    __asm call sub_4045B5
    __asm pushad

    App = *reinterpret_cast<XomApp**>(0x8506D8);
    Width = App->RenderWidth;
    Height = App->RenderHeight;
    AspectRatioX = (float)Width / (float)Height;
    AspectRatioY = (float)Height / (float)Width;
    MovieRect.left = 0;
    MovieRect.top = 0;
    MovieRect.right = Width;
    MovieRect.bottom = Height;

    if (AspectRatioY < 3.0f / 4.0f)      // Hor+
    {
        Frustum.Left = AspectRatioX * Frustum.Bottom;
        Frustum.Right = AspectRatioX * Frustum.Top;
        ShrinkHUD.Right = AspectRatioX * ShrinkHUD.Top;
        MovieRect.right = (int)(MovieRect.bottom * 4.0f / 3.0f);
        MovieRect.left = (int)((Width - MovieRect.right) / 2.0f);
    }
    else if (AspectRatioY > 3.0f / 4.0f) // Vert+
    {
        Frustum.Bottom = AspectRatioY * Frustum.Left;
        Frustum.Top = AspectRatioY * Frustum.Right;
        ShrinkHUD.Top = AspectRatioY * ShrinkHUD.Right;
        MovieRect.bottom = (int)(MovieRect.right * 3.0f / 4.0f);
        MovieRect.top = (int)((Height - MovieRect.bottom) / 2.0f);
    }

    __asm popad
    __asm jmp Exit
}

void __declspec(naked) FrustumCodeCave()
{
    static const DWORD Exit = 0x4ABCE3;

    __asm {
        fld dword ptr ds : [Frustum.Left]
        fstp dword ptr ds : [esp + 0x24]
        fld dword ptr ds : [Frustum.Right]
        fstp dword ptr ds : [esp + 0x28]
        fld dword ptr ds : [Frustum.Bottom]
        fstp dword ptr ds : [esp + 0x2C]
        fld dword ptr ds : [Frustum.Top]
        fstp dword ptr ds : [esp + 0x30]
        jmp Exit
    }
}

void __declspec(naked) SetFromSceneCameraCodeCave()
{
    static const DWORD Exit = 0x434575;
    static float ApertureX, ApertureY;

    __asm {
        fld dword ptr ds : [ecx + 0x2C]
        fstp ApertureX
        fld dword ptr ds : [ecx + 0x30]
        fstp ApertureY
        pushad
    }

    if (AspectRatioY < 3.0f / 4.0f)      // Hor+
        ApertureX = ApertureY * AspectRatioX;
    else if (AspectRatioY > 3.0f / 4.0f) // Vert+
        ApertureY = ApertureX * AspectRatioY;

    __asm {
        popad
        fld ApertureX
        fstp dword ptr ds : [ecx + 0x2C]
        fld ApertureY
        fstp dword ptr ds : [ecx + 0x30]
        fld dword ptr ds : [0x7EEC64]
        jmp Exit
    }
}

void __declspec(naked) MoviePlayerPCCodeCave()
{
    static const DWORD Exit = 0x6B7023;

    __asm {
        mov edx, dword ptr ds : [MovieRect.bottom]
        push edx
        mov edx, dword ptr ds : [MovieRect.right]
        push edx
        mov edx, dword ptr ds : [MovieRect.top]
        push edx
        mov edx, dword ptr ds : [MovieRect.left]
        push edx
        jmp Exit
    }
}

void Init()
{
    CIniReader iniReader("");

    AspectRatioFix = iniReader.ReadBoolean("Main", "AspectRatioFix", false);
    LandmineLodFix = iniReader.ReadBoolean("Main", "LandmineLodFix", false);
    EnableAllLanguages = iniReader.ReadBoolean("Main", "EnableAllLanguages", false);
    SleepFix = iniReader.ReadBoolean("Main", "SleepFix", false);
    FrameInterval = iniReader.ReadInteger("Main", "FrameInterval", 16);

    HUD_SecondWeapon_Back_PosX = iniReader.ReadFloat("HUD", "SecondWeapon.Back.PosX", 220.0f);
    HUD_SecondWeapon_Back_PosY = iniReader.ReadFloat("HUD", "SecondWeapon.Back.PosY", 200.0f);

    Borderless = iniReader.ReadBoolean("Options", "Borderless", false);

    GameSpyHost = iniReader.ReadString("GameSpy", "Host", "gamespy.com");
    MotdUrl = iniReader.ReadString("GameSpy", "MotdUrl", "http://www.team17.com/gamespy/mayhem/motd/");

    // Main

    if (AspectRatioFix)
    {
        static float WXFE_ZBuffer_Scale = 2000.0f;
        static float HUD_Blank_Width = 1280.0f;
        // static float HUD_Blank_Height = 480.0f;
        static float EFMV_Border_Width = 900.0f;

        // AppDataService::AppInit
        injector::MakeJMP(0x4AB897, AspectRatioCodeCave);

        // AppDataService::InitScene
        injector::MakeJMP(0x4ABCC3, FrustumCodeCave); // Background
        injector::WriteMemory<FLOAT*>(0x4AC221 + 2, &Frustum.Right, true);   // Menu Front
        injector::WriteMemory<FLOAT*>(0x4AC244 + 2, &Frustum.Top, true);     // Menu Front
        injector::WriteMemory<FLOAT*>(0x4AC216 + 2, &ShrinkHUD.Right, true); // SHRINKHUD
        injector::WriteMemory<FLOAT*>(0x4AC23A + 2, &ShrinkHUD.Top, true);   // SHRINKHUD

        // FrontEndService
        injector::WriteMemory<FLOAT>(0x5BE8E2 + 1, WXFE_ZBuffer_Scale, true); // Menu BG ZBuffer

        // PopUpService
        injector::WriteMemory<FLOAT>(0x5E0D21 + 1, WXFE_ZBuffer_Scale, true); // Popup BG ZBuffer

        // WXFE_MenuEntity
        injector::WriteMemory<FLOAT>(0x6C1A65 + 1, WXFE_ZBuffer_Scale, true); // Menu Tint

        // CameraManagerService::CameraManagerService
        injector::WriteMemory<FLOAT*>(0x5A8CD5 + 2, &Frustum.Right, true);   // HUD
        injector::WriteMemory<FLOAT*>(0x5A8D0E + 2, &Frustum.Top, true);     // HUD
        injector::WriteMemory<FLOAT*>(0x5A8CCB + 2, &ShrinkHUD.Right, true); // SHRINKHUD
        injector::WriteMemory<FLOAT*>(0x5A8D04 + 2, &ShrinkHUD.Top, true);   // SHRINKHUD

        // HudService::Initialize
        injector::WriteMemory<FLOAT>(0x54FBC9 + 1, HUD_Blank_Width, true);  // HUD Tint
        // injector::WriteMemory<FLOAT>(0x54FBC4 + 1, HUD_Blank_Height, true); // HUD Tint

        // EfmvBorderEntity::EfmvBorderEntity
        injector::WriteMemory<FLOAT*>(0x543148 + 2, &EFMV_Border_Width, true);

        // XCamera::SetFromSceneCamera
        injector::MakeJMP(0x43456F, SetFromSceneCameraCodeCave);

        // MoviePlayerPC::Open
        injector::MakeJMP(0x6B7013, MoviePlayerPCCodeCave);
    }

    if (LandmineLodFix)
    {
        // PayloadGraphicEntity::CreateMesh
        injector::WriteMemory<BYTE>(0x56340A, 0xEB, true); // jmp
    }

    if (EnableAllLanguages)
    {
        // WormsXApp::SetLanguage
        injector::WriteMemory<BYTE>(0x4AB37D + 2, 0xC, true); // cmp esi, u12
        injector::MakeNOP(0x4AB389, 7);
        injector::WriteMemory<WORD>(0x4AB389, 0x07EB, true); // jmp 0x9
        // WormsXApp::DisplayLanguageSelect
        injector::WriteMemory<BYTE>(0x4AD72A + 2, 0xC, true); // cmp eax, u12
        injector::MakeNOP(0x4AD736, 7);
        injector::WriteMemory<WORD>(0x4AD736, 0x07EB, true); // jmp 0x9
    }

    if (SleepFix)
    {
        timeBeginPeriod(1);
    }

    if (FrameInterval != 16)
    {
        // WormsXApp::UpdateScene
        injector::WriteMemory<BYTE>(0x4AC713 + 2, FrameInterval, true);
        injector::WriteMemory<BYTE>(0x4AC718 + 1, FrameInterval, true);
    }

    // HUD Tweaks

    if (HUD_SecondWeapon_Back_PosX != 220.0f)
    {
        // SecondaryWeaponGraphicEntity
        injector::WriteMemory<FLOAT>(0x4E58CB + 4, HUD_SecondWeapon_Back_PosX, true);
    }
    if (HUD_SecondWeapon_Back_PosY != 200.0f)
    {
        // SecondaryWeaponGraphicEntity
        injector::WriteMemory<FLOAT>(0x4E58D3 + 4, HUD_SecondWeapon_Back_PosY, true);
    }

    // Options

    if (Borderless)
    {
        // XOpenGLRenderManager::OpenDisplay
        injector::WriteMemory<BYTE>(0x4699BF + 1, 0, true); // Pos Y
        injector::WriteMemory<BYTE>(0x4699C1 + 1, 0, true); // Pos X
        // XOglWindow::Initialize
        injector::WriteMemory<DWORD>(0x46206C + 3, 0x96000000, true); // dwStyle
        injector::MakeNOP(0x462063, 9);
        injector::MakeNOP(0x462076, 6);
        injector::WriteMemory<BYTE>(0x462076, 0x5E, true); // pop esi
    }

    // GameSpy

    if (GameSpyHost != "gamespy.com")
    {
        GS.AvailableCheck += GameSpyHost;
        GS.ServerBrowser += GameSpyHost;
        GS.QueryReport += GameSpyHost;
        GS.ConnectionManager += GameSpyHost;
        GS.SearchPlayer += GameSpyHost;
        GS.PeerChat += GameSpyHost;
        injector::WriteMemory(0x6BE109 + 1, GS.AvailableCheck.c_str(), true);
        injector::WriteMemory(0x6D837A + 1, GS.ServerBrowser.c_str(), true);
        injector::WriteMemory(0x6BDB51 + 1, GS.QueryReport.c_str(), true);
        injector::WriteMemory(0x6CA231 + 1, GS.ConnectionManager.c_str(), true);
        injector::WriteMemory(0x6CAE6F + 1, GS.SearchPlayer.c_str(), true);
        injector::WriteMemory(0x6D2E08 + 1, GS.PeerChat.c_str(), true);
        injector::WriteMemory(0x6D2E57 + 1, GS.PeerChat.c_str(), true);
        injector::WriteMemory(0x6D2E93 + 1, GS.PeerChat.c_str(), true);
    }

    if (MotdUrl != "http://www.team17.com/gamespy/mayhem/motd/")
    {
        injector::WriteMemory(0x672E3E + 1, MotdUrl.c_str(), true);
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
