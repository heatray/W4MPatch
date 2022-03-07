#include "stdafx.h"

enum Edition {
    DEMOONLINE, // Demo Online
    EURO2,      // 1.01 European No DRM
    EURO2RLD,   // 1.01 European RELOADED Crack
    POLISH2     // 1.01 Polish No DRM
};

enum AspectRatioAxis { NONE, X, Y };

struct Screen
{
    int Width;
    int Height;
    float fWidth;
    float fHeight;
    AspectRatioAxis Axis;
    float fAspectRatio;
    //float fFOV;
} Screen;

struct Window {
    signed char PosX;
    signed char PosY;
    bool Borderless;
} Window;

struct Frustum
{
    float Left;
    float Right;
    float Bottom;
    float Top;
    //float zNear;
    //float zFar;
} Frustum;

struct HUD {
    float Left;
    float Right;
    //float Bottom;
    //float Top;
    float Offset;
} HUD;

uint8_t FrameInterval;
float HUD_Blank_ScaleX, HUD_Blank_ScaleY, \
    HUD_Secondary_PosX, HUD_Secondary_PosY, \
    WXFE_ZBuffer_ScaleXY, EFMV_BorderWidth, \
    XSceneCamera_ApertureX, XSceneCamera_ApertureY;

struct GameSpy
{
    char AvailableCheck[127] = "%s.available."; // Master - Available Check
    char ServerBrowser[127] = "%s.ms%d.";       // Master - Server Browser
    char QueryReport[127] = "%s.master.";       // Master - Query Report 2
    char ConnectionManager[127] = "gpcm.";      // Presence - Connection Manager
    char SearchPlayer[127] = "gpsp.";           // Presence - Search Player
    char PeerChat[127] = "peerchat.";           // Chat
    char MotdURL[255];                          // Team17 MOTD
} GS;

void Init()
{
    CIniReader iniReader("");
    Screen.Width = iniReader.ReadInteger("Main", "ResX", 0);
    Screen.Height = iniReader.ReadInteger("Main", "ResY", 0);
    FrameInterval = iniReader.ReadInteger("Main", "FrameInterval", 16);
    Window.PosX = iniReader.ReadInteger("Window", "PosX", 100);
    Window.PosY = iniReader.ReadInteger("Window", "PosY", 100);
    Window.Borderless = iniReader.ReadInteger("Window", "Borderless", 0) == 1;
    std::string GameSpyHost = iniReader.ReadString("GameSpy", "Host", "gamespy.com");
    std::string MotdURL = iniReader.ReadString("GameSpy", "MotdURL", "http://www.team17.com/gamespy/mayhem/motd/");

    if (!Screen.Width || !Screen.Height)
        std::tie(Screen.Width, Screen.Height) = GetDesktopRes();

    if (FrameInterval != 16) {
        injector::WriteMemory<BYTE>(0x4AC715, FrameInterval, true);
        injector::WriteMemory<BYTE>(0x4AC719, FrameInterval, true);
    }

    Screen.fWidth = static_cast<float>(Screen.Width);
    Screen.fHeight = static_cast<float>(Screen.Height);
    Screen.fAspectRatio = Screen.fWidth / Screen.fHeight;
    if (Screen.fAspectRatio > (4.0f / 3.0f)) {
        Screen.Axis = X;
    }
    else if (Screen.fAspectRatio < (4.0f / 3.0f)) {
        Screen.fAspectRatio = Screen.fHeight / Screen.fWidth;
        Screen.Axis = Y;
    }

    // Frustum

    if (Screen.Axis == X) {

        Frustum.Left = Screen.fAspectRatio * -0.48f;
        Frustum.Right = Screen.fAspectRatio * 0.48f;
        // In Game & Menu Back
        injector::WriteMemory<FLOAT>(0x4ABCC7, Frustum.Left, true);
        injector::WriteMemory<FLOAT>(0x4ABCCF, Frustum.Right, true);
        // Hud & Menu Front
        injector::WriteMemory<FLOAT*>(0x4AC223, &Frustum.Right, true);
        injector::WriteMemory<FLOAT*>(0x5A8CD7, &Frustum.Right, true);

        XSceneCamera_ApertureX = Screen.fAspectRatio * 24.0f / 1.26f;
        // XSceneCamera: Bomber Scenes
        injector::WriteMemory<FLOAT*>(0x43457D, &XSceneCamera_ApertureX, true);

    }
    else if (Screen.Axis == Y) {

        Frustum.Bottom = Screen.fAspectRatio * -0.64f;
        Frustum.Top = Screen.fAspectRatio * 0.64f;
        // In Game & Menu Back
        injector::WriteMemory<FLOAT>(0x4ABCD7, Frustum.Bottom, true);
        injector::WriteMemory<FLOAT>(0x4ABCDF, Frustum.Top, true);
        // Hud & Menu Front
        injector::WriteMemory<FLOAT*>(0x4AC246, &Frustum.Top, true);
        injector::WriteMemory<FLOAT*>(0x5A8D10, &Frustum.Top, true);

        XSceneCamera_ApertureY = Screen.fAspectRatio * 32.0f / 0.94488f;
        // XSceneCamera: Bomber Scenes
        injector::WriteMemory<FLOAT*>(0x43458B, &XSceneCamera_ApertureY, true);

    }

    // Frontend & HUD

    WXFE_ZBuffer_ScaleXY = 2700.0f;
    // Menu WXFE.ZBuffer
    injector::WriteMemory<FLOAT>(0x5BE8E3, WXFE_ZBuffer_ScaleXY, true);
    injector::WriteMemory<FLOAT>(0x6C1A66, WXFE_ZBuffer_ScaleXY, true);
    injector::WriteMemory<FLOAT>(0x5E0D22, WXFE_ZBuffer_ScaleXY, true); // Popup

    if (Screen.Axis == X) {

        HUD.Left = std::floor(Screen.fAspectRatio * -240.0f);
        HUD.Right = std::floor(Screen.fAspectRatio * 240.0f);
        HUD.Offset = HUD.Right - 320.0f;

        HUD_Blank_ScaleX = 1920.0f;
        injector::WriteMemory<FLOAT>(0x54FBCA, HUD_Blank_ScaleX, true); // HUD.Blank
        
        HUD_Secondary_PosX = 220.0f + HUD.Offset;
        injector::WriteMemory<FLOAT>(0x4E58CF, HUD_Secondary_PosX, true); // HUD.Secondary
        
        EFMV_BorderWidth = 330.0f + HUD.Offset;
        injector::WriteMemory<FLOAT*>(0x54314A, &EFMV_BorderWidth, true);

    }
    //else if (Screen.Axis == Y) {
    //    HUD.Top = std::floor(Screen.fAspectRatio * -320.0f);
    //    HUD.Bottom = std::floor(Screen.fAspectRatio * 320.0f);
    //    HUD.Offset = HUD.Top - 240.0f;
    //
    //    HUD_Secondary_PosY = 220.0f + HUD.Offset;
    //
    //    HUD_Blank_ScaleX = 1920.0f;
    //    injector::WriteMemory(0x54FBCA, HUD_Blank_ScaleX, true);
    //}

    // Window
    if (Window.Borderless) {
        injector::WriteMemory<BYTE>(0x4699C0, 0, true); // Pos Y
        injector::WriteMemory<BYTE>(0x4699C2, 0, true); // Pos X
        injector::MakeNOP(0x462063, 9, true);
        injector::MakeNOP(0x462076, 6, true);
        injector::WriteMemory<BYTE>(0x462071, 0x0B, true); // dwStyle
        injector::WriteMemory<BYTE>(0x462076, 0x5E, true); // pop esi
    }

    // Euro All Languages
    injector::WriteMemory<BYTE>(0x4AB37F, 0xC, true); // cmp esi, u12
    injector::WriteMemory<WORD>(0x4AB389, 0x07EB, true); // jmp 0x9
    injector::MakeNOP(0x4AB38B, 5, true);
    injector::WriteMemory<BYTE>(0x4AD72C, 0xC, true); // cmp eax, u12
    injector::WriteMemory<WORD>(0x4AD736, 0x07EB, true); // jmp 0x9
    injector::MakeNOP(0x4AD738, 5, true);

    // GameSpy
    if (GameSpyHost != "gamespy.com") {
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
    if (MotdURL != "http://www.team17.com/gamespy/mayhem/motd/") {
        strcpy_s(GS.MotdURL, MotdURL.c_str());
        injector::WriteMemory(0x672E3F, &GS.MotdURL, true);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        // Check exe version
        char* MenuTweakLang = (char*) 0x776908;
        char* MenuTweakDemo = (char*) 0x77B004;
        if (strcmp(MenuTweakLang, "MenuTwkXPCEuro") == 0) {
            //Edition EXE = EURO2;
            Init();
        }
        //else if (strcmp(MenuTweakLang, "MenuTwkXPCPolish") == 0) {
        //    //Init();
        //}
        //else if (strcmp(MenuTweakDemo, "MenuTwkXDemoOnline") == 0) {
        //    //Init();
        //}
    }
    return TRUE;
}
