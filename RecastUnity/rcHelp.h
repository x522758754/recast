#pragma once

#include "rcQuery.h"

#define _DllExport __declspec( dllexport )

#include "rcHelp.h"

/// 加载地图navmesh文件
/// @param[in] path 文件路径
extern "C" bool _DllExport load_map_bin(const char* path);

/// 获取navmesh的网格顶点数量
extern "C" int _DllExport get_mesh_vert_count();

/// 获取navmesh的网格顶点坐标
///  @param[out] pos 网格坐标[(x,y,z)]
extern "C" void _DllExport get_mesh_vert_pos(float* pos);

/// 释放导航信息
extern "C" bool _DllExport release_map_bin();

/// 加载对应地图的障碍物文件
/// 须先加载地图的navmesh文件
extern "C" bool _DllExport load_ob_bin(const char* path);

extern "C" bool _DllExport save_ob_bin(char* path);

/// 获取box障碍物数量
extern "C" int _DllExport get_ob_boxs_count();

/// 获取box障碍物的信息,最大数量128
/// @param[out] bmin box最小点坐标[(x,y,z)]
/// @param[out] bmax box最大点坐标[(x,y,z)]
/// @param[out] count box障碍物数量
extern "C" bool _DllExport get_ob_boxs(float *bmin, float *bmax, int* count);

/// 添加一个box障碍物
/// @param[in] bmin box最小点坐标[(x,y,z)]
/// @param[in] bmax box最大点坐标[(x,y,z)]
extern "C" bool _DllExport add_ob_box(float *bmin, float *bmax);

/// 删除一个box障碍物
/// @param[in] bmin box最小点坐标[(x,y,z)]
/// @param[in] bmax box最大点坐标[(x,y,z)]
extern "C" bool _DllExport del_ob_boxs(float *bmin, float *bmax);

/// 删除所有box障碍物
extern "C" void _DllExport del_all_ob();

/// 是否是navmesh上的点
extern "C" bool _DllExport is_valid_pos(float* pos);

/// 寻路所有路点列表
/// @param[in] start 起始点
/// @param[in] end 目标点
/// @param[out] paths 路点列表
/// @param[out] pathCount 路点长度
extern "C" void _DllExport find_path(float* start, float* end, float* paths, int* pathCount);

/// startPos到endPos，发出一条射线(注意只是2d检测)
/// 沿着navmesh表面(不考虑navmesh的y方向，只要沿着navmesh的xz方向可以寻路),如果遇到路障或者不可寻路处(墙)，则会打断
/// @param[in] start 射线起始点
/// @param[in] end 射线结束点
/// @param[out] hitpos 射线打中墙的点的坐标
/// @param[out] hitnormal 击中顶点的处的法线
extern "C" bool _DllExport raycast(float* start, float* end, float* hitpos, float* hitnormal);

/// 获取某点地形的高度
/// @param[in] pos 路点
/// @param[out] height 地形高度
extern "C" void _DllExport get_pos_height(float* pos, float* height);


