#include "rcHelp.h"

rcQuery query;

extern "C" int _DllExport get_tri_vert_count()
{
	return query.get_tri_vert_count();
}

extern "C" void _DllExport get_tri_vert_pos(float* pos)
{
	query.get_tri_vert_pos(pos);
}

extern "C" bool _DllExport load_map_bin(const char* path)
{
	return query.load_map_bin(path);
}

bool _DllExport release_map_bin()
{
	return false;
}

extern "C" int _DllExport get_ob_count()
{
	return query.get_ob_count();
}

extern "C" void _DllExport get_ob_info(float* pos)
{
	query.get_ob_info(pos);
}

extern "C" bool _DllExport load_ob_bin(const char* path)
{
	return query.load_obs_bin(path);
}

bool _DllExport save_obs()
{
	return false;
}

bool _DllExport release_obs()
{
	return false;
}
