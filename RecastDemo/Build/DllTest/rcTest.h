#pragma once
#include <string>

#define _DllExport __declspec( dllexport )

extern "C" int _DllExport get_tri_vert_count();

extern "C" void _DllExport get_tri_vert_pos(float* pos);

extern "C" void _DllExport load_map_bin(const std::string& path);

