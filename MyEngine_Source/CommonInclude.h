#pragma once

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h> 
#include <ws2tcpip.h>
#include <Windows.h>
#include<vector>
#include<list>
#include<stack>
#include<queue>
#include<string>
#include<algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include<functional>
#include<filesystem>
#include<bitset>
#include<string>
#include <iostream>
#include <fstream>

#include <thread>
#include <mutex>

#include "MEMath.h"
#include "MEEnums.h"
#include "assert.h"

#include <mmsystem.h>
#include <dinput.h>
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "winmm.lib")

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")



#define CAST_UINT(value) static_cast<UINT>(value)
#define CAST_FLOAT(value) static_cast<float>(value)