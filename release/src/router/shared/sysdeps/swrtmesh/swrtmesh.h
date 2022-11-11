#ifndef _SWRTMESH_H_
#define _SWRTMESH_H_

#define SWRTMESH_DEBUG             "/tmp/SWRTMESH_DEBUG"
#define SWRTMESH_DEBUG_FILE                  "/tmp/SWRTMESH_DEBUG.log"
#define SWRTMESH_DBG(fmt, args...) ({ \
	if (f_exists(SWRTMESH_DEBUG) > 0){ \
		char info[1024]; \
        char msg[1024]; \
		snprintf(msg, sizeof(msg), "[%s:(%d)]: "fmt, __FUNCTION__, __LINE__, ##args); \
		snprintf(info, sizeof(info), "echo \"%s\" >> %s", msg, SWRTMESH_DEBUG_FILE); \
		system(info); \
	} \
})

enum {
	EASYMESH_VER_R1 = 1,
	EASYMESH_VER_R2,
	EASYMESH_VER_R3,
	EASYMESH_VER_R4,
};
extern int get_easymesh_max_ver(void);
extern char *get_easymesh_ver_str(int ver);
extern char *get_mesh_bh_ifname(int band);
extern void swrtmesh_autoconf(void);
#endif
