// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include <iostream>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//ImGui files
#include "imgui_files/imgui.h"
#include "imgui_files/imgui_impl_dx9.h"
#include "imgui_files/imgui_impl_win32.h"
#include "imgui_files/imgui_internal.h"

#endif //PCH_H
