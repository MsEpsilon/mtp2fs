# mtp2fs
A Windows console utility application that mounts a MTP(Media Transfer Protocol) to a filesystem.

[![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#)
[![Visual Studio](https://custom-icon-badges.demolab.com/badge/Visual%20Studio-5C2D91.svg?&logo=visualstudio&logoColor=white)](#)
[![MSBuild](https://github.com/MsEpsilon/mtp2fs/actions/workflows/msbuild.yml/badge.svg)](https://github.com/MsEpsilon/mtp2fs/actions/workflows/msbuild.yml)

# Usage
`mtp2fs [arguments]`
|Command | Short | Description |
|---|---|---|
|--help|-h| Shows a help message.|
|--mount|-m <device ID> <drive letter>|Mounts a MTP device to a drive letter.|
|--list|-l|Lists connected MTP devices.|

# Building from source
1. Install dokan. https://github.com/dokan-dev/dokany

2. Open the solution file `mtp2fs.sln` using Visual Studio 2022, and build from there.

# License
This project uses a MIT License.
