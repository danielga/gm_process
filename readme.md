gm_process
==========

A Garry's Mod module to create asynchronous child processes. It also allows you to use its error, input and output handles to retrieve or push data to the process.

Info
-------

Mac was not tested at all (sorry but I'm poor).

If stuff starts erroring or fails to work, be sure to check the correct line endings (\n and such) are present in the files for each OS.

This project requires garrysmod_common (https://bitbucket.org/danielga/garrysmod_common), a framework to facilitate the creation of compilations files (Visual Studio, make, XCode, etc). Simply set the environment variable 'GARRYSMOD_COMMON' or the premake option 'gmcommon' to the path of your local copy of garrysmod_common.