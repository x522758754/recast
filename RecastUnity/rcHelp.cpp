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
