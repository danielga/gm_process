local GARRYSMOD_INCLUDES_PATH = "gmod-module-base/include"

solution("gm_process")

	language("C++")
	location("Projects/" .. os.get() .. "-" .. _ACTION)
	flags({"NoPCH"})

	if os.is("macosx") then
		platforms({"Universal32"})
	else
		platforms({"x32"})
	end

	configuration("windows")
		libdirs({SDK_PATH .. "/lib/public"})
		linkoptions({"/NODEFAULTLIB:libc", "/NODEFAULTLIB:libcd", "/NODEFAULTLIB:libcmt"})

	configuration("macosx")
		libdirs({SDK_PATH .. "/lib/public/osx32"})
		linkoptions({"-nostdlib"})

	configuration("linux")
		libdirs({SDK_PATH .. "/lib/public/linux32"})
		-- remove standard libraries from build
	
	configurations({"Release"})

	configuration("Release")
		defines({"NDEBUG"})
		flags({"Optimize", "EnableSSE"})
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
		files({"*.c", "*.cxx", "*.cpp", "*.h", "*.hxx", "*.hpp", "libexecstream/*.cpp", "libexecstream/*.h"})
		vpaths({["Header files"] = {"**.h", "**.hxx", "**.hpp"}, ["Source files"] = {"**.c", "**.cxx", "**.cpp"}})

		targetprefix("gmsv_") -- Just to remove prefixes like lib from Linux
		targetname("process")

		links({"tier0", "tier1", "tier2"})

		configuration("windows")
			targetsuffix("_win32")

		configuration("linux")
			targetsuffix("_linux")
			targetextension(".dll") -- Derp Garry, WHY

		configuration("macosx")
			targetsuffix("_mac")
			targetextension(".dll") -- Derp Garry, WHY

	project("gmcl_process")
		kind("SharedLib")
		defines({"PROCESS_CLIENT", "CLIENT_DLL", "GMMODULE"})
		includedirs({GARRYSMOD_INCLUDES_PATH, "libexecstream"})
		files({"*.c", "*.cxx", "*.cpp", "*.h", "*.hxx", "*.hpp", "libexecstream/*.cpp", "libexecstream/*.h"})
		vpaths({["Header files"] = {"**.h", "**.hxx", "**.hpp"}, ["Source files"] = {"**.c", "**.cxx", "**.cpp"}})

		targetprefix("gmcl_") -- Just to remove prefixes like lib from Linux
		targetname("process")

		links({"tier0", "tier1", "tier2"})

		configuration("windows")
			targetsuffix("_win32")

		configuration("linux")
			targetsuffix("_linux")
			targetextension(".dll") -- Derp Garry, WHY

		configuration("macosx")
			targetsuffix("_mac")
			targetextension(".dll") -- Derp Garry, WHY