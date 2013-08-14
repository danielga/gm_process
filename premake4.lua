local GARRYSMOD_INCLUDES_PATH = "gmod-module-base/include"

solution("gm_process")

	language("C++")
	location("Projects/" .. os.get() .. "-" .. _ACTION)
	flags({"NoPCH", "StaticRuntime", "EnableSSE"})

	if os.is("macosx") then
		platforms({"Universal32"})
	else
		platforms({"x32"})
	end
	configurations({"Release"})

	configuration("Release")
		defines({"NDEBUG"})
		flags({"Optimize"})
		targetdir("Projects/Release")
		objdir("Projects/Intermediate")

	project("process_test")
		kind("ConsoleApp")
		includedirs({"libexecstream"})
		files({"Testing/*.c", "Testing/*.cxx", "Testing/*.cpp", "Testing/*.h", "Testing/*.hxx", "Testing/*.hpp", "libexecstream/*.cpp", "libexecstream/*.h"})
		vpaths({["Header files"] = {"**.h", "**.hxx", "**.hpp"}, ["Source files"] = {"**.c", "**.cxx", "**.cpp"}})
		targetname("process_test")

	project("gmsv_process")
		kind("SharedLib")
		defines({"PROCESS_CLIENT", "GAME_DLL", "GMMODULE"})
		includedirs({GARRYSMOD_INCLUDES_PATH, "libexecstream"})
		links(lib_files)
		files({"*.c", "*.cxx", "*.cpp", "*.h", "*.hxx", "*.hpp", "libexecstream/*.cpp", "libexecstream/*.h"})
		vpaths({["Header files"] = {"**.h", "**.hxx", "**.hpp"}, ["Source files"] = {"**.c", "**.cxx", "**.cpp"}})
		targetprefix("gmsv_") -- Just to remove prefixes like lib from Linux
		targetname("process")
		if os.is("windows") then
			targetsuffix("_win32")
		elseif os.is("linux") then
			targetsuffix("_linux")
			targetextension(".dll") -- Derp Garry, WHY
		elseif os.is("macosx") then
			targetsuffix("_mac")
			targetextension(".dll") -- Derp Garry, WHY
		end

	project("gmcl_process")
		kind("SharedLib")
		defines({"PROCESS_CLIENT", "GAME_DLL", "GMMODULE"})
		includedirs({GARRYSMOD_INCLUDES_PATH, "libexecstream"})
		links(lib_files)
		files({"*.c", "*.cxx", "*.cpp", "*.h", "*.hxx", "*.hpp", "libexecstream/*.cpp", "libexecstream/*.h"})
		vpaths({["Header files"] = {"**.h", "**.hxx", "**.hpp"}, ["Source files"] = {"**.c", "**.cxx", "**.cpp"}})
		targetprefix("gmcl_") -- Just to remove prefixes like lib from Linux
		targetname("process")
		if os.is("windows") then
			targetsuffix("_win32")
		elseif os.is("linux") then
			targetsuffix("_linux")
			targetextension(".dll") -- Derp Garry, WHY
		elseif os.is("macosx") then
			targetsuffix("_mac")
			targetextension(".dll") -- Derp Garry, WHY
		end
