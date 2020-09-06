PROJECT_GENERATOR_VERSION = 2

newoption({
	trigger = "gmcommon",
	description = "Sets the path to the garrysmod_common (https://github.com/danielga/garrysmod_common) directory",
	value = "path to garrysmod_common directory"
})

include(assert(_OPTIONS.gmcommon or os.getenv("GARRYSMOD_COMMON"),
	"you didn't provide a path to your garrysmod_common (https://github.com/danielga/garrysmod_common) directory"))

CreateWorkspace({name = "process"})
	CreateProject({serverside = true})
		IncludeLuaShared()
		includedirs("tiny-process-library")
		links("tiny-process-library")

	CreateProject({serverside = false})
		IncludeLuaShared()
		includedirs("tiny-process-library")
		links("tiny-process-library")

	project("tiny-process-library")
		kind("StaticLib")
		files({
			"tiny-process-library/process.cpp",
			"tiny-process-library/process.hpp",
		})

		filter("system:windows")
			files("tiny-process-library/process_win.cpp")

		filter("system:linux or macosx")
			files("tiny-process-library/process_unix.cpp")
