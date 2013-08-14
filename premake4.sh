#!/bin/sh
./premake4nix --os=windows vs2010 || die "Premake failed creating project for Windows (VS2010)"
./premake4nix --os=macosx codeblocks || die "Premake failed creating project for MacOSX (CodeBlocks)"
./premake4nix --os=linux codeblocks || die "Premake failed creating project for Linux (CodeBlocks)"