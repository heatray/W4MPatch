workspace "W4MPatch"
   configurations { "Release", "Debug" }
   platforms { "Win32" }
   architecture "x32"
   location "build"
   buildoptions {"-std:c++latest"}

project "W4MPatch"
   kind "SharedLib"
   language "C++"
   characterset ("UNICODE")
   staticruntime "On"

   defines {
      "rsc_MajorVersion=0",
      "rsc_MinorVersion=3",
      "rsc_RevisionID=0",
      "rsc_BuildID=1",
      "rsc_StringVersion=\"0.3.0.1\"",
      "rsc_CompanyName=\"heatray\"",
      "rsc_FileDescription=\"Patch for Worms 4 Mayhem\"",
      "rsc_InternalName=\"%{prj.name}\"",
      "rsc_LegalCopyright=\"Copyright (C) heatray\"",
      "rsc_OriginalFilename=\"%{prj.name}.dll\"",
      "rsc_ProductName=\"Anniversary Patch\"",
      "rsc_UpdateUrl=\"https://github.com/heatray/W4MPatch/releases\""
   }

   files { "source/dllmain.cpp" }
   files { "source/stdafx.h", "source/stdafx.cpp" }
   -- files { "source/hooking/Hooking.Patterns.h", "source/hooking/Hooking.Patterns.cpp" }
   files { "source/resources/VersionInfo.rc" }

   includedirs { "source" }
   -- includedirs { "source/hooking" }
   includedirs { "source/injector/include" }
   includedirs { "source/inireader" }

   filter "configurations:Debug*"
      defines "DEBUG"
      symbols "On"

   filter "configurations:Release*"
      defines "NDEBUG"
      optimize "On"
