newoption({
	trigger = "gmcommon",
	description = "Sets the path to the garrysmod_common (https://bitbucket.org/danielga/garrysmod_common) directory",
	value = "path to garrysmod_common dir"
})

local gmcommon = _OPTIONS.gmcommon or os.getenv("GARRYSMOD_COMMON")
if gmcommon == nil then
	error("you didn't provide a path to your garrysmod_common (https://bitbucket.org/danielga/garrysmod_common) directory")
end

include(gmcommon)

local LIBEXECSTREAM_FOLDER = "../libexecstream"

CreateSolution("luaerror")
	CreateProject(SERVERSIDE)
		includedirs({LIBEXECSTREAM_FOLDER})
		files({LIBEXECSTREAM_FOLDER .. "/exec-stream.cpp"})

	CreateProject(CLIENTSIDE)
		includedirs({LIBEXECSTREAM_FOLDER})
		files({LIBEXECSTREAM_FOLDER .. "/exec-stream.cpp"})