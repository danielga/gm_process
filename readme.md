# gm\_process

A Garry's Mod module to create asynchronous child processes. It also allows you to use its error, input and output handles to retrieve or push data to the process.

## API reference

When return types or parameters are enclosed in square brackets, they are nillable returns or optional parameters and will contain the default value, in the case of parameters.

### Types

```lua
Process = { -- Represents a process.
    table GetTable() -- Returns a table associated with this object, where users can place data.
    boolean Write(input) -- Writes an 'input' string to the process stdin and returns a boolean representing success or not.
    CloseInput() -- Closes the process stdin handle.
    number Close() -- Waits for the process to shutdown and returns the exit code.
    Kill([force]) -- Kills the process, with an optional 'force' boolean, used on Unix-like systems to forcefully terminate the program (if false, tells the process to shutdown gracefully but it can ultimately do whatever it wants).
    [number] GetExitCode() -- Returns a number, representing the exit code of the process or 'nil', if it hasn't exited yet.
    [table] GetOutput() -- Returns a numerically ordered table of strings, with stdout output or 'nil', if not stdout output is available.
    [table] GetErrorOutput() -- Returns a numerically ordered table of strings, with stderr output or 'nil', if not stderr output is available.
}
```

### Constants

```lua
-- Holds the 'process' module version in a string form.
-- Example: "process 1.0.0"
string process.Version
```

```lua
-- Holds the 'process' module version in a numeric form, LuaJIT style.
-- Example: 10000
number process.VersionNum
```

### Functions

```lua
-- Create a process with the provided executable path and optional parameters.
-- 'executable' is a string, containing the path to the executable.
-- 'args' is a numerically ordered table of strings, containing the arguments passed to the process. Can be 'nil', meaning no arguments.
-- 'cwd' is a string, containing the current working directory for the process. Can be 'nil', defaulting to the current process working directory.
-- 'env' is a key-value table of strings, containing the environment variables passed to the process. Can be 'nil', defaulting to the current process environment variables.
-- Returns a 'Process' object if successful, otherwise returns 'nil'.
[Process] process.Create(executable, [args], [cwd], [env])
```

## Compiling

The only supported compilation platform for this project on Windows is **Visual Studio 2017**. However, it's possible it'll work with *Visual Studio 2015* and *Visual Studio 2019* because of the unified runtime.

On Linux, everything should work fine as is.

For macOS, any **Xcode (using the GCC compiler)** version *MIGHT* work as long as the **Mac OSX 10.7 SDK** is used.

These restrictions are not random; they exist because of ABI compatibility reasons.

If stuff starts erroring or fails to work, be sure to check the correct line endings (\n and such) are present in the files for each OS.

## Requirements

This project requires [garrysmod\_common][1], a framework to facilitate the creation of compilations files (Visual Studio, make, XCode, etc). Simply set the environment variable '**GARRYSMOD\_COMMON**' or the premake option '**gmcommon**' to the path of your local copy of [garrysmod\_common][1].

  [1]: https://github.com/danielga/garrysmod_common
