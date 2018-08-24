#include "rcHelp.h"

rcQuery* query = nullptr;

/// ���ص�ͼnavmesh�ļ�
/// @param[in] path �ļ�·��
extern "C" bool _DllExport load_map_bin(const char* path)
{
	query = new rcQuery();
	if(query)
		return query->load_map_bin(path);

	return false;
}

/// ��ȡnavmesh�����񶥵�����
extern "C" int _DllExport get_mesh_vert_count()
{
	if (query)
		return query->get_tri_vert_count() * 9;

	return 0;
}

/// ��ȡnavmesh�����񶥵�����
///  @param[out] pos ��������[(x,y,z)]
extern "C" void _DllExport get_mesh_vert_pos(float* pos)
{
	query->get_tri_vert_pos(pos);
}

/// �ͷŵ�����Ϣ
extern "C" bool _DllExport release_map_bin()
{
	if (query)
	{
		delete query;
		query = nullptr;
	}
	return false;
}

/// ���ض�Ӧ��ͼ���ϰ����ļ�
/// ���ȼ��ص�ͼ��navmesh�ļ�
extern "C" bool _DllExport load_ob_bin(const char* path)
{
	if(query)
		return query->load_obs_bin(path);
	return false;
}

extern "C" bool _DllExport save_ob_bin(char* path)
{
	if (query)
		query->save_obs_bin(path);

	return false;
}

/// ��ȡbox�ϰ�������
extern "C" int _DllExport get_ob_boxs_count()
{
	if (query)
		return query->get_ob_box_count();
	return -1;
}

/// ��ȡbox�ϰ������Ϣ,�������128
/// @param[out] bmin box��С������[(x,y,z)]
/// @param[out] bmax box��������[(x,y,z)]
/// @param[out] count box�ϰ�������
extern "C" bool _DllExport get_ob_boxs(float *bmin, float *bmax, int* count)
{
	if (query)
	{
		*count = query->get_ob_box_count();
		query->get_ob_boxs(bmin, bmax);
		return true;
	}

	return false;
}

/// ���һ��box�ϰ���
/// @param[in] bmin box��С������[(x,y,z)]
/// @param[in] bmax box��������[(x,y,z)]
extern "C" bool _DllExport add_ob_box(float *bmin, float *bmax)
{
	if (query)
		return query->add_box_obstacle(bmin, bmax);
	return false;
}

/// ɾ��һ��box�ϰ���
/// @param[in] bmin box��С������[(x,y,z)]
/// @param[in] bmax box��������[(x,y,z)]
extern "C" bool _DllExport del_ob_boxs(float *bmin, float *bmax)
{
	if (query)
		return query->remove_box_obstacle(bmin, bmax);
	return false;
}

/// ɾ������box�ϰ���
extern "C" void _DllExport del_all_ob()
{
	if (query)
		query->remove_all_box_obstacle();
}

/// �Ƿ���navmesh�ϵĵ�
extern "C" bool _DllExport is_valid_pos(float* pos)
{
	if (query)
		return query->is_valid_point(pos);
	return false;
}

/// Ѱ·����·���б�
/// @param[in] start ��ʼ��
/// @param[in] end Ŀ���
/// @param[out] paths ·���б�
/// @param[out] pathCount ·�㳤��
extern "C" void _DllExport find_path(float* start, float* end, float* paths, int* pathCount)
{
	*pathCount = 0;
	if (query)
		query->findpath(start, end, paths, *pathCount);
}

/// startPos��endPos������һ������(ע��ֻ��2d���)
/// ����navmesh����(������navmesh��y����ֻҪ����navmesh��xz�������Ѱ·),�������·�ϻ��߲���Ѱ·��(ǽ)�������
/// @returns hitPos �Ƿ�ΪendPos
/// @param[in] start ������ʼ��
/// @param[in] end ���߽�����
/// @param[out] hitpos ���ߴ���ǽ�ĵ������
/// @param[out] hitnormal ���ж���Ĵ��ķ���
extern "C" bool _DllExport raycast(float* start, float* end, float* hitpos, float* hitnormal)
{
	if (query)
	{
		query->raycast(start, end, hitpos, hitnormal);
		return true;
	}
	return false;
}

/// ��ȡĳ����εĸ߶�
/// @param[in] pos ·��
/// @param[out] height ���θ߶�
extern "C" void _DllExport get_pos_height(float* pos, float* height)
{
	if (query)
		query->get_point_y(pos, height);
}