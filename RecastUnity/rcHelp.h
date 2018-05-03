#pragma once

#include "rcQuery.h"

#define _DllExport __declspec( dllexport )

///获取navmesh顶点数量
extern "C" int _DllExport get_tri_vert_count();

///获取navmesh顶点位置信息(每个顶点包含3个float）
extern "C" void _DllExport get_tri_vert_pos(float* pos);

///加载navmesh文件信息
extern "C" void _DllExport load_map_bin(const char* path);

///获取障碍数量
extern "C" int _DllExport get_ob_count();

///获取障碍信息(所在位置、障碍类型待实现）
extern "C" void _DllExport get_ob_info(float* pos);

///加载障碍文件
extern "C" void _DllExport load_ob_bin(const char* path);
