#include "rcQuery.h"

void main()
{
	rcQuery query;
	query.load_map_bin("../RecastDemo/Bin/bins/nav_test.bin");
 	//query.load_obs_bin("../RecastDemo/Bin/All_TempObstaclesCache.bin");
// 	float pos[3] = { 36.f, -2.2f, -10.1f };
// 	query.add_obstacle(0, pos);
// 	query.save_obs_bin("../RecastDemo/Bin/All_TempObstaclesCache.bin");
	int count = query.get_tri_vert_count() * 3;
	float *pos;
	pos = new float[count];
	query.get_tri_vert_pos(pos);

	int obCount = query.get_ob_count();
	float* obPos = NULL;
	obPos = new float[obCount * 3];
	query.get_ob_info(obPos);
}