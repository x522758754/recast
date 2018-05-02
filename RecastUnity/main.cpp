#include "rcQuery.h"

void main()
{
	rcQuery query;
	query.load_map_bin("nav_test.bin");
	query.load_obs_bin("../RecastDemo/Bin/All_TempObstaclesCache.bin");
	float pos[3] = { 36.f, -2.2f, -10.1f };
	query.add_obstacle(0, pos);
	query.save_obs_bin("../RecastDemo/Bin/All_TempObstaclesCache.bin");
}