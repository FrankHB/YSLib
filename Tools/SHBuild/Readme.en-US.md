# Tool: Simple host build

This is a command line tool to generate binary files by recursively scanning source directories and then calling backends (i.e. compiler driver or archiver).

The output can be object/archive/executable files.

Currently it is also used for other tasks in the builds.

* It can run commands to make directories and to intall files.
* It is used as a interperter for NPLA1 language. This is necessary to host NPLA1 scripts in the YSLib builds.

## Precondition

This project depends on YSLib.

See [YSLib project user documentation (zh-CN)](https://frankhb.github.io/YSLib-book/Tools/SHBuild.zh-CN.html) for details.

## Build

See user documentation for details.

See also directory `../Scripts` for scripts used during the build.

### Build options

Define `SHBuild_NoBacktrace` to `true` and pass it to the compiler options to disable NPLA1 backtrace on exceptions. The backtrace is otherwise enabled by default.

## Test

Self host test scripts are provided.

See `../Scripts/SHBuild-self-host` and `SHBuild-self-host-DLL.sh`.

## Usage

**NOTE** The program is deployed by [Sysroot (zh-CN)](https://frankhb.github.io/YSLib-book/Sysroot.zh-CN.html).

Run the program without additional arguments (e.g. `SHBuild`) to see the help message for the detailed usage.

The program returns 0 if and only if there is no failure.

