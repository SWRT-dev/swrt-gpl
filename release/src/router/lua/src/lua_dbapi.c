#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <dirent.h>
#include <assert.h>
#include <math.h>
#include <sys/types.h>
#include <sys/file.h>
#include <limits.h>
#include "lua.h"
#include "lauxlib.h"


#include "dbapi.h"

#ifndef SKIPD_MODNAME
#define SKIPD_MODNAME   "skipd"
#endif

#ifndef SKIPD_VERSION
#define SKIPD_VERSION   "1.0.0"
#endif

#ifdef _MSC_VER
#define SKIPD_EXPORT    __declspec(dllexport)
#else
#define SKIPD_EXPORT    extern
#endif

/* Workaround for Solaris platforms missing isinf() */
#if !defined(isinf) && (defined(USE_INTERNAL_ISINF) || defined(MISSING_ISINF))
#define isinf(x) (!isnan(x) && isnan((x) - (x)))
#endif

static int skipd_get(lua_State *L)
{
    char db_buf[999];
	const char *name = luaL_checkstring(L, 1);
	dbclient client;
	dbclient_start(&client);
//	lua_newtable(L);
    memset(db_buf, 0, sizeof(db_buf));
	dbclient_get(&client, name, db_buf, sizeof(db_buf));
	dbclient_end(&client);
//    lua_pushstring(L, "result");  /* push key */
    lua_pushstring(L, db_buf);  /* push value */
//    lua_settable(L, -3);
    return 1;
}

static int all_offset = 0;
static char all_buf[65535];
static int db_print(dbclient* client, char* prefix, char* key, char* value) {
    all_offset += snprintf(&all_buf[all_offset], sizeof(all_buf) - all_offset, "%s=%s\n", key, value);
	return 0;
}

static int skipd_getall(lua_State *L)
{
	const char *prefix = luaL_checkstring(L, 1);
	dbclient client;
	dbclient_start(&client);
//	lua_newtable(L);
    memset(all_buf, 0, sizeof(all_buf));
    all_offset = 0;
    dbclient_list(&client, prefix, db_print);
	dbclient_end(&client);
//    lua_pushstring(L, "result");  /* push key */
    lua_pushstring(L, all_buf);  /* push value */
//    lua_settable(L, -3);
    return 1;
}

static int get_pid_by_name(const char* task_name)
{
    int pid_value = 0;
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[512] = { 0 };
    char cur_task_name[512] = { 0 };
    char buf[1024] = { 0 };

    dir = opendir("/proc");
    if (NULL != dir) {
        while((ptr = readdir(dir)) != NULL) {
            if((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))            
                continue;

            if(DT_DIR != ptr->d_type)
                continue;

            sprintf(filepath, "/proc/%s/status", ptr->d_name);

            fp = fopen(filepath, "r");
            if(NULL != fp) {
                if(fgets(buf, 1023, fp) == NULL) {
                    fclose(fp);
                    continue;
                }

                sscanf(buf, "%*s %s", cur_task_name);
                if (!strcmp(task_name, cur_task_name)) {
                    printf("PID:%s\n", ptr->d_name);
                    pid_value = atoi(ptr->d_name);
                }

                fclose(fp);
            }
        }
        closedir(dir);
    } else {
        printf("open proc failed!\n");
    }
    
    return pid_value;
}

static int skipd_pid(lua_State *L)
{
	int pid = 0;
	const char *name = luaL_checkstring(L, 1);
	pid = get_pid_by_name(name);
    lua_pushnumber(L, pid);

    return 1;
}

static int f_exists(const char *path)	// note: anything but a directory
{
	struct stat st;
	return (stat(path, &st) == 0) && (!S_ISDIR(st.st_mode));
}

static int d_exists(const char *path)	//  directory only
{
	struct stat st;
	return (stat(path, &st) == 0) && (S_ISDIR(st.st_mode));
}

static int _file_lock(const char *dir, const char *tag)
{
	char cmd[70];
	char path[64];

	snprintf(path, sizeof(path), "%s/%s.lock", dir, tag);
	snprintf(cmd, sizeof(cmd), "touch %s", path);
	/* check if we already hold a lock */
	if (f_exists(path))
		return -1;
	system(cmd);
	return 0;
}

static int file_lock(const char *tag)
{
	return _file_lock("/var/lock", tag);
}

static int _file_unlock(const char *dir, const char *tag)
{
	char path[64];

	snprintf(path, sizeof(path), "%s/%s.lock", dir, tag);
	unlink(path);
	return 0;
}

static int file_unlock(const char *tag)
{
	return _file_unlock("/var/lock", tag);
}


static int _file_islocked(const char *dir, const char *tag)
{
	char path[64];

	snprintf(path, sizeof(path), "%s/%s.lock", dir, tag);

	/* check if we already hold a lock */
	if (f_exists(path))
		return 1;

	return 0;
}

static int file_islocked(const char *tag)
{
	return _file_islocked("/var/lock", tag);
}

static int skipd_lock(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);

	file_lock(name);
    lua_pushnumber(L, 1);

    return 1;
}

static int skipd_unlock(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	file_unlock(name);
	lua_pushnumber(L, 1);

    return 1;
}

static int skipd_islocked(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, file_islocked(name));

    return 1;
}

static int prefix_len;
static int db_nodelist(dbclient* client, char* prefix, char* key, char* value) {
	all_offset += snprintf(&all_buf[all_offset], sizeof(all_buf) - all_offset, "%s ", key + prefix_len);
	return 0;
}

static int skipd_get_nodelist(lua_State *L)
{
	const char *prefix = luaL_checkstring(L, 1);
	dbclient client;
	dbclient_start(&client);
	prefix_len = strlen(prefix);
	all_offset = 0;
    memset(all_buf, 0, sizeof(all_buf));
    dbclient_list(&client, prefix, db_nodelist);
	dbclient_end(&client);
    lua_pushstring(L, all_buf);

    return 1;
}

/* ===== INITIALISATION ===== */

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
/* Compatibility for Lua 5.1.
 *
 * luaL_setfuncs() is used to create a module table where the functions have
 * json_config_t as their first upvalue. Code borrowed from Lua 5.2 source. */
static void luaL_setfuncs (lua_State *l, const luaL_Reg *reg, int nup)
{
    int i;

    luaL_checkstack(l, nup, "too many upvalues");
    for (; reg->name != NULL; reg++) {  /* fill the table with given functions */
        for (i = 0; i < nup; i++)  /* copy upvalues to the top */
            lua_pushvalue(l, -nup);
        lua_pushcclosure(l, reg->func, nup);  /* closure with those upvalues */
        lua_setfield(l, -(nup + 2), reg->name);
    }
    lua_pop(l, nup);  /* remove upvalues */
}
#endif

/* Call target function in protected mode with all supplied args.
 * Assumes target function only returns a single non-nil value.
 * Convert and return thrown errors as: nil, "error message" */
static int json_protect_conversion(lua_State *l)
{
    int err;

    /* Deliberately throw an error for invalid arguments */
    luaL_argcheck(l, lua_gettop(l) == 1, 1, "expected 1 argument");

    /* pcall() the function stored as upvalue(1) */
    lua_pushvalue(l, lua_upvalueindex(1));
    lua_insert(l, 1);
    err = lua_pcall(l, 1, 1, 0);
    if (!err)
        return 1;

    if (err == LUA_ERRRUN) {
        lua_pushnil(l);
        lua_insert(l, -2);
        return 2;
    }

    /* Since we are not using a custom error handler, the only remaining
     * errors are memory related */
    return luaL_error(l, "Memory allocation error in CJSON protected call");
}

/* Return skipd module table */
static int lua_skipd_new(lua_State *l)
{
    luaL_Reg reg[] = {
        { "get", skipd_get },
        { "get_all", skipd_getall },
        { "pid", skipd_pid },
        { "lock", skipd_lock },
        { "unlock", skipd_unlock },
        { "islocked", skipd_islocked },
        { "get_nodelist", skipd_get_nodelist },
        { NULL, NULL }
    };


    /* skipd module table */
    lua_newtable(l);

    /* Register functions with config data as upvalue */
    luaL_setfuncs(l, reg, 0);

    /* Set skipd.null */
    lua_pushlightuserdata(l, NULL);
    lua_setfield(l, -2, "null");

    /* Set module name / version fields */
    lua_pushliteral(l, SKIPD_MODNAME);
    lua_setfield(l, -2, "_NAME");
    lua_pushliteral(l, SKIPD_VERSION);
    lua_setfield(l, -2, "_VERSION");

    return 1;
}

/* Return skipd.safe module table */
static int lua_skipd_safe_new(lua_State *l)
{
    const char *func[] = { "get", "get_all", NULL };
    int i;

    lua_skipd_new(l);

    /* Fix new() method */
    lua_pushcfunction(l, lua_skipd_safe_new);
    lua_setfield(l, -2, "new");

    for (i = 0; func[i]; i++) {
        lua_getfield(l, -1, func[i]);
        lua_pushcclosure(l, json_protect_conversion, 1);
        lua_setfield(l, -2, func[i]);
    }

    return 1;
}

LUALIB_API int luaopen_skipd(lua_State *l)
{
    lua_skipd_new(l);

    /* Register a global "skipd" table. */
    lua_pushvalue(l, -1);
    lua_setglobal(l, SKIPD_MODNAME);

    /* Return skipd table */
    return 1;
}

LUALIB_API int luaopen_skipd_safe(lua_State *l)
{
    lua_skipd_safe_new(l);

    /* Return skipd.safe table */
    return 1;
}

