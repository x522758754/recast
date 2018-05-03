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

extern "C" void _DllExport load_map_bin(const char* path)
{
	query.load_map_bin(path);
}

extern "C" int _DllExport get_ob_count()
{
	return query.get_ob_count();
}

extern "C" void _DllExport get_ob_info(float* pos)
{
	return query.get_ob_info(pos);
}

extern "C" void _DllExport load_ob_bin(const char* path)
{
	query.load_obs_bin(path);
}