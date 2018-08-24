#include "rcQuery.h"

void main()
{
	rcQuery query;
	query.load_map_bin("../RecastDemo/Bin/bins/nav_test.bin");
 	query.load_obs_bin("../RecastDemo/Bin/All_TempObstaclesCache.bin");
}