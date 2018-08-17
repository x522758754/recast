#include "rcQuery.h"
#include "DetourNavMesh.h"
#include "DetourTileCache.h"
#include "DetourNavMeshQuery.h"
#include "DetourCrowd.h"
#include "DetourCommon.h"
#include "DetourAlloc.h"
#include "InputGeom.h"
#include "fastlz.h"
#include "DetourNavMeshBuilder.h"
#include "DetourTileCacheBuilder.h"

/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};

enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

struct TileCacheObstacleHeader
{
	dtObstacleRef obRef;
	unsigned char type;
	union
	{
		dtObstacleCylinder cylinder;
		dtObstacleBox box;
		dtObstacleOrientedBox orientedBox;
	};
};

struct TileCacheSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams meshParams;
	dtTileCacheParams cacheParams;
};

struct TileCacheTileHeader
{
	dtCompressedTileRef tileRef;
	int dataSize;
};

struct FastLZCompressor : public dtTileCacheCompressor
{
	virtual int maxCompressedSize(const int bufferSize)
	{
		return (int)(bufferSize* 1.05f);
	}

	virtual dtStatus compress(const unsigned char* buffer, const int bufferSize,
		unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize)
	{
		*compressedSize = fastlz_compress((const void *const)buffer, bufferSize, compressed);
		return DT_SUCCESS;
	}

	virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize,
		unsigned char* buffer, const int maxBufferSize, int* bufferSize)
	{
		*bufferSize = fastlz_decompress(compressed, compressedSize, buffer, maxBufferSize);
		return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
	}
};

struct LinearAllocator : public dtTileCacheAlloc
{
	unsigned char* buffer;
	size_t capacity;
	size_t top;
	size_t high;

	LinearAllocator(const size_t cap) : buffer(0), capacity(0), top(0), high(0)
	{
		resize(cap);
	}

	~LinearAllocator()
	{
		dtFree(buffer);
	}

	void resize(const size_t cap)
	{
		if (buffer) dtFree(buffer);
		buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
		capacity = cap;
	}

	virtual void reset()
	{
		high = dtMax(high, top);
		top = 0;
	}

	virtual void* alloc(const size_t size)
	{
		if (!buffer)
			return 0;
		if (top + size > capacity)
			return 0;
		unsigned char* mem = &buffer[top];
		top += size;
		return mem;
	}

	virtual void free(void* /*ptr*/)
	{
		// Empty
	}
};

struct MeshProcess : public dtTileCacheMeshProcess
{
	InputGeom* m_geom;

	inline MeshProcess() : m_geom(0)
	{
	}

	inline void init(InputGeom* geom)
	{
		m_geom = geom;
	}

	virtual void process(struct dtNavMeshCreateParams* params,
		unsigned char* polyAreas, unsigned short* polyFlags)
	{
		// Update poly flags from areas.
		for (int i = 0; i < params->polyCount; ++i)
		{
			if (polyAreas[i] == DT_TILECACHE_WALKABLE_AREA)
				polyAreas[i] = SAMPLE_POLYAREA_GROUND;

			if (polyAreas[i] == SAMPLE_POLYAREA_GROUND ||
				polyAreas[i] == SAMPLE_POLYAREA_GRASS ||
				polyAreas[i] == SAMPLE_POLYAREA_ROAD)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (polyAreas[i] == SAMPLE_POLYAREA_WATER)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (polyAreas[i] == SAMPLE_POLYAREA_DOOR)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}

		// Pass in off-mesh connections.
		if (m_geom)
		{
			params->offMeshConVerts = m_geom->getOffMeshConnectionVerts();
			params->offMeshConRad = m_geom->getOffMeshConnectionRads();
			params->offMeshConDir = m_geom->getOffMeshConnectionDirs();
			params->offMeshConAreas = m_geom->getOffMeshConnectionAreas();
			params->offMeshConFlags = m_geom->getOffMeshConnectionFlags();
			params->offMeshConUserID = m_geom->getOffMeshConnectionId();
			params->offMeshConCount = m_geom->getOffMeshConnectionCount();
		}
	}
};


static const int TILECACHESET_MAGIC = 'T' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'TSET';
static const int TILECACHESET_VERSION = 1;


static bool isectSegAABB(const float* sp, const float* sq,
	const float* amin, const float* amax,
	float& tmin, float& tmax)
{
	static const float EPS = 1e-6f;

	float d[3];
	dtVsub(d, sq, sp);
	tmin = 0;  // set to -FLT_MAX to get first hit on line
	tmax = FLT_MAX;		// set to max distance ray can travel (for segment)

						// For all three slabs
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(d[i]) < EPS)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (sp[i] < amin[i] || sp[i] > amax[i])
				return false;
		}
		else
		{
			// Compute intersection t value of ray with near and far plane of slab
			const float ood = 1.0f / d[i];
			float t1 = (amin[i] - sp[i]) * ood;
			float t2 = (amax[i] - sp[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) dtSwap(t1, t2);
			// Compute the intersection of slab intersections intervals
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
	}

	return true;
}

dtObstacleRef hitTestObstacle(const dtTileCache* tc, const float* sp, const float* sq)
{
	float tmin = FLT_MAX;
	const dtTileCacheObstacle* obmin = 0;
	for (int i = 0; i < tc->getObstacleCount(); ++i)
	{
		const dtTileCacheObstacle* ob = tc->getObstacle(i);
		if (ob->state == DT_OBSTACLE_EMPTY)
			continue;

		float bmin[3], bmax[3], t0, t1;
		tc->getObstacleBounds(ob, bmin, bmax);

		if (isectSegAABB(sp, sq, bmin, bmax, t0, t1))
		{
			if (t0 < tmin)
			{
				tmin = t0;
				obmin = ob;
			}
		}
	}
	return tc->getObstacleRef(obmin);
}

rcQuery::rcQuery() :
	m_tileCache(0),
	m_navMesh(0),
	m_navQuery(0),
	m_straightPathOptions(0),
	m_nstraightPath(0)
{
	m_polyPickExt[0] = 2;
	m_polyPickExt[1] = 4;
	m_polyPickExt[2] = 2;

	m_filter.setIncludeFlags(0xffff ^ 0x10);
	m_filter.setExcludeFlags(0);

	m_talloc = new LinearAllocator(32000);
	m_tcomp = new FastLZCompressor;
	m_tmproc = new MeshProcess;

	m_navQuery = dtAllocNavMeshQuery();
}

rcQuery::~rcQuery()
{
	dtFreeNavMesh(m_navMesh);
	m_navMesh = 0;
	m_tileCache = 0;
	m_navMesh = 0;
}

void rcQuery::initConfig()
{

}

bool rcQuery::load_map_bin(const std::string& path)
{
	FILE* fp;
	fopen_s(&fp, path.c_str(), "rb");
	if (!fp) return false;

	TileCacheSetHeader header;
	size_t headerReadReturnCode = fread(&header, sizeof(TileCacheSetHeader), 1, fp);
	if (headerReadReturnCode != 1 ||
		header.magic != TILECACHESET_MAGIC ||
		header.version != TILECACHESET_VERSION)
	{
		fclose(fp);
		return false;
	}
	
	m_navMesh = dtAllocNavMesh();
	m_tileCache = dtAllocTileCache();
	if (!m_navMesh || !m_tileCache)
	{
		fclose(fp);
		return false;
	}

	dtStatus status = m_navMesh->init(&header.meshParams);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return false;
	}

	status = m_tileCache->init(&header.cacheParams, m_talloc, m_tcomp, m_tmproc);

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		TileCacheTileHeader tileHeader;
		size_t tileHeaderReadReturnCode = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (tileHeaderReadReturnCode != 1)
		{
			fclose(fp);
			return;
		}
		if(!tileHeader.tileRef || !tileHeader.dataSize)
			break;
		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if(!data) break;
		memset(data, 0, tileHeader.dataSize);
		size_t tileDataReturnCode = fread(data, tileHeader.dataSize, 1, fp);
		if (tileDataReturnCode != 1)
		{
			dtFree(data);
			fclose(fp);
			return;
		}

		dtCompressedTileRef tile = 0;
		dtStatus addTileStatus = m_tileCache->addTile(data, tileHeader.dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tile);
		if (dtStatusFailed(addTileStatus))
		{
			dtFree(data);
		}
		if (tile)
			m_tileCache->buildNavMeshTile(tile, m_navMesh);
	}
	fclose(fp);

	m_navQuery->init(m_navMesh, 2048);

	return true;
}

bool rcQuery::load_obs_bin(const std::string& path) const
{
	if (!m_tileCache) return false;

	FILE* fp;
	fopen_s(&fp, path.c_str(), "rb");
	if (!fp) return false;

	int count;
	size_t headerReadReturnCode = fread(&count, sizeof(unsigned int), 1, fp);
	if (headerReadReturnCode != 1)
	{
		fclose(fp);
		return false;
	}

	for (int i = 0; i != count; ++i)
	{
		TileCacheObstacleHeader obHeader;
		size_t obHeaderReadReturnCode = fread(&obHeader, sizeof(obHeader), 1, fp);
		if (obHeaderReadReturnCode != 1)
		{
			fclose(fp);
			return false;
		}
		if (!obHeader.obRef) break;
		switch (obHeader.type)
		{
		case DT_OBSTACLE_BOX: 
			m_tileCache->addBoxObstacle(obHeader.box.bmin, obHeader.box.bmax, &obHeader.obRef); 
			break;
		case DT_OBSTACLE_CYLINDER:
			m_tileCache->addObstacle(obHeader.cylinder.pos, obHeader.cylinder.radius, obHeader.cylinder.height, &obHeader.obRef); 
			break;
		case DT_OBSTACLE_ORIENTED_BOX:
			//m_tileCache->addBoxObstacle(obHeader.orientedBox.center, obHeader.orientedBox.halfExtents, obHeader.orientedBox.rotAux);
			break;
		default: break;
		}
	}
	fclose(fp);

	return true;
}

void rcQuery::save_obs_bin(const std::string& path) const
{
	if (!m_tileCache) return;

	FILE* fp;
	fopen_s(&fp, path.c_str(), "wb");
	if (!fp) return;

	unsigned int count = 0;
	for (int i=0;i != m_tileCache->getObstacleCount(); ++i)
	{
		const dtTileCacheObstacle* ob = m_tileCache->getObstacle(i);
		if (ob->state == DT_OBSTACLE_EMPTY) continue;
		count++;
	}
	fwrite(&count, sizeof(unsigned int), 1, fp);

	for (int i = 0; i != m_tileCache->getObstacleCount(); ++i)
	{
		const dtTileCacheObstacle* ob = m_tileCache->getObstacle(i);
		if (ob->state == DT_OBSTACLE_EMPTY) continue;
		TileCacheObstacleHeader obHeader;

		obHeader.obRef = m_tileCache->getObstacleRef(ob);
		obHeader.type = ob->type;
		if (DT_OBSTACLE_BOX == ob->type)
		{
			memcpy(&obHeader.box, &ob->box, sizeof(ob->box));
		}
		else if (DT_OBSTACLE_CYLINDER == ob->type)
		{
			memcpy(&obHeader.cylinder, &ob->cylinder, sizeof(ob->box));
		}
		else if (DT_OBSTACLE_ORIENTED_BOX == ob->type)
		{
			memcpy(&obHeader.orientedBox, &ob->orientedBox, sizeof(ob->box));
		}
		else
		{
			continue;
		}
		fwrite(&obHeader, sizeof(obHeader), 1, fp);
	}
	fclose(fp);
}

bool rcQuery::is_valid_point(const float* pos) const
{
	dtPolyRef targetRef(0);
	dtStatus status(0);
	float targetPos [3];
	
	status = m_navQuery->findNearestPoly(pos, m_polyPickExt, &m_filter, &targetRef, targetPos);
	if (dtStatusSucceed(status) && targetRef > 0)
	{
		m_navQuery->closestPointOnPoly(targetRef, pos, targetPos, 0);// more accurate
		if (!(abs(targetPos[0] - pos[0]) <= 1e-6 && abs(targetPos[2] - pos[2]) <= 1e-6))
		{
			return false;
		}
	}

	return dtStatusSucceed(status) && targetRef > 0;
}

void rcQuery::raycast(const float* spos, const float* epos, float* path)
{
	dtPolyRef startRef;
	//dtPolyRef endRef;
	
	m_navQuery->findNearestPoly(spos, m_polyPickExt, &m_filter, &startRef, 0);

	if (startRef)
	{
		float t = 0;
		float hitNoraml[3];
		int npolys = 0;
		float hitPos[3];

		m_navQuery->raycast(startRef, spos, epos, &m_filter, &t, hitNoraml, m_polys, &npolys, MAX_POLYS);
		if (t > 1) // No hit
		{
			dtVcopy(hitPos, epos);
		}
		else // hit
		{
			dtVlerp(hitPos, spos, epos, t);
			if (npolys)
			{
				float h = 0;
				m_navQuery->getPolyHeight(m_polys[npolys - 1], hitPos, &h);
				hitPos[1] = h;
			}
		}
	}

}

void rcQuery::findpath(const float* spos, const float* epos, float* path, int& count)
{
	dtPolyRef startRef(0);
	dtPolyRef endRef(0);
	int nPolys(0);

	m_navQuery->findNearestPoly(spos, m_polyPickExt, &m_filter, &startRef, 0);
	m_navQuery->findNearestPoly(epos, m_polyPickExt, &m_filter, &endRef, 0);
	if (startRef && endRef)
	{
		m_navQuery->findPath(startRef, endRef, spos, epos, &m_filter, m_polys, &nPolys, MAX_POLYS);
		if (nPolys)
		{
			dtPolyRef arriveEndRef = m_polys[nPolys - 1]; //可到达最远的
			float arriveEndpos[3];

			if (arriveEndRef != endRef)
				m_navQuery->closestPointOnPoly(arriveEndRef, epos, arriveEndpos, 0);

			m_navQuery->findStraightPath(spos, arriveEndpos, m_polys, nPolys, 
										 m_straightPath, m_straightPathFlags,
										 m_straightPathPolys, &m_nstraightPath, MAX_POLYS, m_straightPathOptions);
			
		}
	}
	else
	{
		m_nstraightPath = 0;
	}
	//
	count = m_nstraightPath;

	for (int i=0; i != m_nstraightPath; ++i)
	{
// 		path[i*3]	= m_straightPath[i*3];
// 		path[i*3+1]	= m_straightPath[i*3+1];
// 		path[i*3+2]	= m_straightPath[i*3+2];
		dtVcopy(path + i * 3, m_straightPath + i * 3);
	}
}

void rcQuery::get_point_y(const float* pos, float* height) const
{
	dtPolyRef targetRef(0);

	m_navQuery->findNearestPoly(pos, m_polyPickExt, &m_filter, &targetRef, 0);
	if (targetRef)
	{
		m_navQuery->getPolyHeight(targetRef, pos, height);
	}
}

void rcQuery::add_obstacle(const unsigned char type, const float* pos)
{
	if (!m_tileCache) return;
	
	float p[3];
	dtVcopy(p, pos);
	if (DT_OBSTACLE_CYLINDER == type)
	{
		p[1] = -0.5f;
		m_tileCache->addObstacle(pos, 1.0, 2.0f, 0);
	}
	else if (DT_OBSTACLE_BOX == type)
	{
		float bmin[3], bmax[3];
		bmin[0] = pos[0] - 0.5f;
		bmin[1] = pos[1] - 0.5f;
		bmin[2] = pos[2] - 0.5f;
		bmax[0] = pos[0] + 0.5f;
		bmax[1] = pos[1] + 0.5f;
		bmax[2] = pos[2] + 0.5f;
		m_tileCache->addBoxObstacle(bmin, bmax, 0);
	}

}

void rcQuery::remove_obstacle(const float* pos)
{
	if (!m_tileCache) return;

	dtObstacleRef ref = hitTestObstacle(m_tileCache, pos, pos);

	m_tileCache->removeObstacle(ref);
}

int rcQuery::get_tri_vert_count()
{
	int count = 0;
	if (!m_navMesh)
		return 0;
	const dtNavMesh* navMesh = m_navMesh;

	for (int a = 0; a != navMesh->getMaxTiles(); ++a)
	{
		const dtMeshTile* tile = navMesh->getTile(a);
		if (!tile->header) continue;

		for (int i = 0; i != tile->header->polyCount; ++i)
		{
			const dtPoly* p = &tile->polys[i];
			if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) continue;

			const dtPolyDetail* pd = &tile->detailMeshes[i];
			for (int j = 0; j != pd->triCount; ++j)
			{
				for (int k = 0; k < 3; ++k)
				{
					const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];
					if (t[k] < p->vertCount)
					{
						count++;
					}
				}
			}
		}
	}

	return count;
}

void rcQuery::get_tri_vert_pos(float* vertexPos)
{
	const dtNavMesh* navMesh = m_navMesh;
	if (!vertexPos || !navMesh)
	{
		return;
	}
	int vertCount = 0;
	for (int a=0; a != navMesh->getMaxTiles(); ++a)
	{
		const dtMeshTile* tile = navMesh->getTile(a);
		if (!tile->header) continue;

		for (int i=0; i != tile->header->polyCount; ++i)
		{
			const dtPoly* p = &tile->polys[i];
			if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) continue;

			const dtPolyDetail* pd = &tile->detailMeshes[i];
			for (int j=0; j != pd->triCount; ++j)
			{
				for (int k=0; k < 3; ++k)
				{
					const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4]; //获取当前三角形的组成的索引数组
																					   //k								-- 当前三角形中的索引，表示三角形第k（k < 3）个顶点的索引
																					   //t[k]							-- 当前多边形顶点数组的索引
																					   //p->verts[t[k]]				-- 当前MeshTile中的位置信息(顶点)的索引
																					   //p->verts[t[k]]*3				-- 每个顶点的位置占用3个float，tile->verts为float数组
																					   //tile->verts[p->verts[t[k]]*3]	-- 查询当前MeshTile中对应的位置信息
					if (t[k] < p->vertCount)
					{
						dtVcopy(vertexPos + vertCount * 3, &tile->verts[p->verts[t[k]] * 3]);
						vertCount++;
					}
				}
			}
		}
	}
}

int rcQuery::get_ob_count()
{
	int count = 0;
	if (!m_tileCache)
		return 0;
	for (int i = 0; i < m_tileCache->getObstacleCount(); ++i)
	{
		const dtTileCacheObstacle* ob = m_tileCache->getObstacle(i);
		if (ob->state == DT_OBSTACLE_EMPTY) continue;
		if (ob->type == DT_OBSTACLE_CYLINDER) count++;
	}

	return count;
}

void rcQuery::get_ob_info(float* pos)
{
	if (!m_tileCache || !pos)
		return;
	for (int i = 0; i < m_tileCache->getObstacleCount(); ++i)
	{
		const dtTileCacheObstacle* ob = m_tileCache->getObstacle(i);
		if (ob->state == DT_OBSTACLE_EMPTY) continue;
		if (ob->type == DT_OBSTACLE_CYLINDER)
			dtVcopy(pos + i * 3, ob->cylinder.pos);
	}
}
