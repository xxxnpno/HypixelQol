#pragma once
#include "../src/log_reader.h"
#include "../src/SDK/Minecraft.h"
#include "../src/utils/http/Network.h"
#include "../src/utils/path/UserPath.h"
#include "../src/utils/json/ExtractPlayer.h"
#include "../src/utils/json/ExtractSuccess.h"
#include "../src/utils/json/ExtractRank.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include <Windows.h>
#include <shlobj.h>

void init(void* instance);



