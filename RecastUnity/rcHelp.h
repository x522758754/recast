#pragma once

#include "rcQuery.h"

#define _DllExport __declspec( dllexport )

///��ȡnavmesh��������
extern "C" int _DllExport get_tri_vert_count();

///��ȡnavmesh����λ����Ϣ(ÿ���������3��float��
extern "C" void _DllExport get_tri_vert_pos(float* pos);

///����navmesh�ļ���Ϣ
extern "C" bool _DllExport load_map_bin(const char* path);

///�ͷ�navmesh�ļ���Ϣ,������ϰ�����ϢҲ��һ���ͷ�
extern "C" bool _DllExport release_map_bin();

///��ȡ�ϰ�����
extern "C" int _DllExport get_ob_count();

///��ȡ�ϰ���Ϣ(����λ�á��ϰ����ʹ�ʵ�֣�
extern "C" void _DllExport get_ob_info(float* pos);

///�����ϰ��ļ�
extern "C" bool _DllExport load_ob_bin(const char* path);

///�����ϰ�����Ϣ
extern "C" bool _DllExport save_obs();

///�ϰ�����Ϣ
extern "C" bool _DllExport release_obs();


