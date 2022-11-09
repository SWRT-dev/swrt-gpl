#ifndef _SWRTMESH_H_
#define _SWRTMESH_H_
enum {
	EASYMESH_VER_R1 = 1,
	EASYMESH_VER_R2,
	EASYMESH_VER_R3,
	EASYMESH_VER_R4,
};
extern int get_easymesh_max_ver(void);
extern char *get_easymesh_ver_str(int ver);
extern char *get_mesh_bh_ifname(int band);
#endif
