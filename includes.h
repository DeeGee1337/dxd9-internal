#pragma once

#include <Windows.h>
#include <iostream>
#include <cstdint>
#include <math.h>
#include <chrono>
#include <string.h>
#include <sstream>
#include <atomic>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "dx.h"
#include "hook.h"
#include "drawing.h"
#include "hack.h"
#include "offsets.h"
#include "player.h"
#include "localplayer.h"
#include "csgotrace.h"
#include "csgoVector.h"

extern Hack* hack;