#include "stdafx.h"
#include <string>
#include <vector>

struct ConfigDataEntry {
    const char* Section;
    const char* Key;
    const char* DefaultValue;
};

const ConfigDataEntry ConfigData[] = {
    {"XDataResourceManager", "DRM.MaxInstances", "8000"},
    {"XDataResourceManager", "DRM.MaxHooks", "512"},
    {"MessageRegistrationService", "MRS.MaxMessages", "1105"},
    {"MessageRegistrationService", "MRS.MaxInterests", "4096"},
    {"XGraphicalResourceManager", "GRM.MaxBitmaps", "700"},
    {"XGraphicalResourceManager", "GRM.MaxMeshes", "550"},
    {"XGraphicalResourceManager", "GRM.MaxCustom", "100"},
    {"XGraphicalResourceManager", "GRM.MaxSpriteSets", "250"},
    {"XGraphicalResourceManager", "GRM.MaxText", "500"},
    {"XGraphicalResourceManager", "GRM.MaxNulls", "400"},
    {"XEventLogContext", "XOM.EventLog.BufferSize", "1000000"},
    {"XEventLogContext", "XOM.EventLog.UseHighMemory", "1"},
    {"XEventLogContext", "XOM.EventLog.Pipe.MaxSendInterval", "10000"},
    {"XEventLogContext", "XOM.EventLog.Pipe.MaxPacketSize", "1000"},
    {"XEventLogContext", "XOM.EventLog.Pipe.BufferSize", "100000"},
    {"TaskManager", "Core.MaxTasks", "800"},
    {"TaskManager", "Core.MessageBufferSize", "32768"},
    {"TaskManager", "Core.MaxLogicTasks", "192"},
    {"TaskManager", "Core.MaxServiceTasks", "80"},
    {"Other", "XOM.XomClass.InstanceIdTag", "0"},
    {"Other", "XOM.XomClass.StacktraceTag", "0"},
    {"Other", "XOM.ExportClasses", "0"},
    {"Other", "XOM.LogFile", "1"},
};

struct ConfigEntry {
    const char* Key;
    const char* Value;
};

std::vector<ConfigEntry> Config;

void Init()
{
    CIniReader iniReader("");

    Config.reserve(std::size(ConfigData) + 1);
    for (auto& e : ConfigData) {
        std::string string = iniReader.ReadString(e.Section, e.Key, e.DefaultValue);
        if (string.empty()) continue;
        Config.emplace_back(ConfigEntry{ e.Key, _strdup(string.c_str()) });
    }
    Config.emplace_back(nullptr, nullptr);

    // l_Config
    injector::WriteMemory(0x7409B0 + 1, Config.data(), true);
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
