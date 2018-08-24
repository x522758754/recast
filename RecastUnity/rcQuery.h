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
	
	/// 初始配置
	void initConfig();

	/// 加载navmesh bin文件
	bool load_map_bin(const std::string& path);

	/// 释放navmesh bin 文件
	bool release_map_bin();

	///加载障碍物文件
	bool load_obs_bin(const std::string& path) const;

	///保存障碍物文件
	void save_obs_bin(const std::string& path) const;

	/// 是否合法路径点
	bool is_valid_point(const float* pos) const;

	/// 以起点坐标、目标坐标的直线路径上最远的合法坐标(2D check.)
	void raycast(const float* spos, const float* epos, float* hitPos, float*hitNormal);

	/// 路径查询
	void findpath(const float* spos, const float* epos, float* path, int& count);

	/// 获路径点的y坐标
	void get_point_y(const float* pos, float* height) const;

	/// 添加box障碍
	bool add_box_obstacle(const float* bmin, const float*bmax);

	///移除障碍物
	bool remove_box_obstacle(const float* bmin, const float* bmax);

	///移除所有障碍物
	void remove_all_box_obstacle();

	///获得构成navmesh的三角形顶点数量
	int get_tri_vert_count();

	///获得构成navmesh的三角形顶点组成
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

	//寻路相关
	int m_straightPathOptions;
	dtPolyRef m_polys[MAX_POLYS];
	int m_nstraightPath;
	float m_straightPath[MAX_POLYS * 3];
	unsigned char m_straightPathFlags[MAX_POLYS];
	dtPolyRef m_straightPathPolys[MAX_POLYS];
};
