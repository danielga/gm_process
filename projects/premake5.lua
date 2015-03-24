GARRYSMOD_MODULE_BASE_FOLDER = "../gmod-module-base"
SOURCE_FOLDER = "../source"
LIBEXECSTREAM_FOLDER = "../libexecstream"
PROJECT_FOLDER = os.get() .. "/" .. _ACTION

solution("gm_process")
	language("C++")
	location(PROJECT_FOLDER)
	warnings("Extra")
	flags({"NoPCH", "StaticRuntime"})
	platforms({"x86"})
	configurations({"Release", "Debug"})

	filter("platforms:x86")
		architecture("x32")

	filter("configurations:Release")
		optimize("On")
		vectorextensions("SSE2")
		objdir(PROJECT_FOLDER .. "/intermediate")
		targetdir(PROJECT_FOLDER .. "/release")

	filter("configurations:Debug")
		flags({"Symbols"})
		objdir(PROJECT_FOLDER .. "/intermediate")
		targetdir(PROJECT_FOLDER .. "/debug")

	project("gmsv_process")
		kind("SharedLib")
		defines({"GMMODULE", "PROCESS_SERVER"})
		includedirs({
			SOURCE_FOLDER,
			GARRYSMOD_MODULE_BASE_FOLDER .. "/include",
			LIBEXECSTREAM_FOLDER
		})
		files({
			SOURCE_FOLDER .. "/*.cpp",
			SOURCE_FOLDER .. "/*.hpp",
			LIBEXECSTREAM_FOLDER .. "/exec-stream.cpp",
			LIBEXECSTREAM_FOLDER .. "/exec-stream.h"
		})
		vpaths({
			["Header files"] = {
				SOURCE_FOLDER .. "/**.hpp",
				LIBEXECSTREAM_FOLDER .. "/**.h"
			},
			["Source files"] = {
				SOURCE_FOLDER .. "/**.cpp",
				LIBEXECSTREAM_FOLDER .. "/**.cpp"
			}
		})

		targetprefix("")
		targetextension(".dll")

		filter("system:windows")
			targetsuffix("_win32")

		filter("system:linux")
			targetsuffix("_linux")

		filter({"system:macosx"})
			targetsuffix("_mac")

	project("gmcl_process")
		kind("SharedLib")
		defines({"GMMODULE", "PROCESS_CLIENT"})
		includedirs({
			SOURCE_FOLDER,
			GARRYSMOD_MODULE_BASE_FOLDER .. "/include",
			LIBEXECSTREAM_FOLDER
		})
		files({
			SOURCE_FOLDER .. "/*.cpp",
			SOURCE_FOLDER .. "/*.hpp",
			LIBEXECSTREAM_FOLDER .. "/exec-stream.cpp",
			LIBEXECSTREAM_FOLDER .. "/exec-stream.h"
		})
		vpaths({
			["Header files"] = {
				SOURCE_FOLDER .. "/**.hpp",
				LIBEXECSTREAM_FOLDER .. "/**.h"
			},
			["Source files"] = {
				SOURCE_FOLDER .. "/**.cpp",
				LIBEXECSTREAM_FOLDER .. "/**.cpp"
			}
		})

		targetprefix("")
		targetextension(".dll")

		filter("system:windows")
			targetsuffix("_win32")

		filter("system:linux")
			targetsuffix("_linux")

		filter({"system:macosx"})
			targetsuffix("_mac")