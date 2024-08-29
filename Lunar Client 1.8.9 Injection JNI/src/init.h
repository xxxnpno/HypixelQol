#pragma once
#include "../log_reader.h"
#include "../SDK/Minecraft.h"


#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>

#include <Windows.h>
#include <shlobj.h>

void init(void* instance);
std::string getUserPath();


