#pragma once
#include <string>
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"


/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.

class rcQuery
{
public:
	rcQuery();
	~rcQuery();
	
	/// ��ʼ����
	void initConfig();

	/// ����navmesh bin�ļ�
	bool load_map_bin(const std::string& path);

	/// �ͷ�navmesh bin �ļ�
	bool release_map_bin();

	///�����ϰ����ļ�
	bool load_obs_bin(const std::string& path) const;

	///�����ϰ����ļ�
	void save_obs_bin(const std::string& path) const;

	/// �Ƿ�Ϸ�·����
	bool is_valid_point(const float* pos) const;

	/// ��������ꡢĿ�������ֱ��·������Զ�ĺϷ�����(2D check.)
	void raycast(const float* spos, const float* epos, float* hitPos, float*hitNormal);

	/// ·����ѯ
	void findpath(const float* spos, const float* epos, float* path, int& count);

	/// ��·�����y����
	void get_point_y(const float* pos, float* height) const;

	/// ���box�ϰ�
	bool add_box_obstacle(const float* bmin, const float*bmax);

	///�Ƴ��ϰ���
	bool remove_box_obstacle(const float* bmin, const float* bmax);

	///�Ƴ������ϰ���
	void remove_all_box_obstacle();

	///��ù���navmesh�������ζ�������
	int get_tri_vert_count();

	///��ù���navmesh�������ζ������
	void get_tri_vert_pos(float* pos);

	///
	int get_ob_box_count();

	void rcQuery::get_ob_boxs(float* bmin, float*bmax);

protected:
	static const int MAX_POLYS = 256;

	struct LinearAllocator* m_talloc;
	struct FastLZCompressor* m_tcomp;
	struct MeshProcess* m_tmproc;

	class dtTileCache* m_tileCache;
	dtNavMesh* m_navMesh;
	dtNavMeshQuery* m_navQuery;
	dtQueryFilter m_filter;

	float m_polyPickExt[3];

	//Ѱ·���
	int m_straightPathOptions;
	dtPolyRef m_polys[MAX_POLYS];
	int m_nstraightPath;
	float m_straightPath[MAX_POLYS * 3];
	unsigned char m_straightPathFlags[MAX_POLYS];
	dtPolyRef m_straightPathPolys[MAX_POLYS];
};
