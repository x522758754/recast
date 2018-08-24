#pragma once

#include "rcQuery.h"

#define _DllExport __declspec( dllexport )

#include "rcHelp.h"

/// ���ص�ͼnavmesh�ļ�
/// @param[in] path �ļ�·��
extern "C" bool _DllExport load_map_bin(const char* path);

/// ��ȡnavmesh�����񶥵�����
extern "C" int _DllExport get_mesh_vert_count();

/// ��ȡnavmesh�����񶥵�����
///  @param[out] pos ��������[(x,y,z)]
extern "C" void _DllExport get_mesh_vert_pos(float* pos);

/// �ͷŵ�����Ϣ
extern "C" bool _DllExport release_map_bin();

/// ���ض�Ӧ��ͼ���ϰ����ļ�
/// ���ȼ��ص�ͼ��navmesh�ļ�
extern "C" bool _DllExport load_ob_bin(const char* path);

extern "C" bool _DllExport save_ob_bin(char* path);

/// ��ȡbox�ϰ�������
extern "C" int _DllExport get_ob_boxs_count();

/// ��ȡbox�ϰ������Ϣ,�������128
/// @param[out] bmin box��С������[(x,y,z)]
/// @param[out] bmax box��������[(x,y,z)]
/// @param[out] count box�ϰ�������
extern "C" bool _DllExport get_ob_boxs(float *bmin, float *bmax, int* count);

/// ���һ��box�ϰ���
/// @param[in] bmin box��С������[(x,y,z)]
/// @param[in] bmax box��������[(x,y,z)]
extern "C" bool _DllExport add_ob_box(float *bmin, float *bmax);

/// ɾ��һ��box�ϰ���
/// @param[in] bmin box��С������[(x,y,z)]
/// @param[in] bmax box��������[(x,y,z)]
extern "C" bool _DllExport del_ob_boxs(float *bmin, float *bmax);

/// ɾ������box�ϰ���
extern "C" void _DllExport del_all_ob();

/// �Ƿ���navmesh�ϵĵ�
extern "C" bool _DllExport is_valid_pos(float* pos);

/// Ѱ·����·���б�
/// @param[in] start ��ʼ��
/// @param[in] end Ŀ���
/// @param[out] paths ·���б�
/// @param[out] pathCount ·�㳤��
extern "C" void _DllExport find_path(float* start, float* end, float* paths, int* pathCount);

/// startPos��endPos������һ������(ע��ֻ��2d���)
/// ����navmesh����(������navmesh��y����ֻҪ����navmesh��xz�������Ѱ·),�������·�ϻ��߲���Ѱ·��(ǽ)�������
/// @param[in] start ������ʼ��
/// @param[in] end ���߽�����
/// @param[out] hitpos ���ߴ���ǽ�ĵ������
/// @param[out] hitnormal ���ж���Ĵ��ķ���
extern "C" bool _DllExport raycast(float* start, float* end, float* hitpos, float* hitnormal);

/// ��ȡĳ����εĸ߶�
/// @param[in] pos ·��
/// @param[out] height ���θ߶�
extern "C" void _DllExport get_pos_height(float* pos, float* height);


