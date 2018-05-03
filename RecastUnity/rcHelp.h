#pragma once

#include "rcQuery.h"

#define _DllExport __declspec( dllexport )

///��ȡnavmesh��������
extern "C" int _DllExport get_tri_vert_count();

///��ȡnavmesh����λ����Ϣ(ÿ���������3��float��
extern "C" void _DllExport get_tri_vert_pos(float* pos);

///����navmesh�ļ���Ϣ
extern "C" void _DllExport load_map_bin(const char* path);

///��ȡ�ϰ�����
extern "C" int _DllExport get_ob_count();

///��ȡ�ϰ���Ϣ(����λ�á��ϰ����ʹ�ʵ�֣�
extern "C" void _DllExport get_ob_info(float* pos);

///�����ϰ��ļ�
extern "C" void _DllExport load_ob_bin(const char* path);
