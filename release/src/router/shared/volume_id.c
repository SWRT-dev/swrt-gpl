#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <errno.h>

/****************************************************/
/* Use busybox routines to get labels for fat & ext */
/* Probe for label the same way that mount does.    */
/****************************************************/
#include "autoconf.h"
#include "volume_id_internal.h"
#include <rtconfig.h>

#ifdef DEBUG_NOISY
extern void cprintf(const char *format, ...);
#define _dprintf		cprintf
#define csprintf		cprintf
#else
#define _dprintf(args...)	do { } while(0)
#define csprintf(args...)	do { } while(0)
#endif

#ifdef __GLIBC__
#undef errno
#define errno (*__errno_location ())
#endif

#pragma GCC visibility push(hidden)

#ifndef DMALLOC
void* FAST_FUNC xmalloc(size_t size)
{
	return malloc(size);
}

void* FAST_FUNC xrealloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}
#endif /* DMALLOC */

ssize_t FAST_FUNC safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

/*
 * Read all of the supplied buffer from a file.
 * This does multiple reads as necessary.
 * Returns the amount read, or -1 on an error.
 * A short read is returned on an end of file.
 */
ssize_t FAST_FUNC full_read(int fd, void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_read(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already have some! */
				/* user can do another read to know the error code */
				return total;
			}
			return cc; /* read() returns -1 on failure. */
		}
		if (cc == 0)
			break;
		buf = ((char *)buf) + cc;
		total += cc;
		len -= cc;
	}

	return total;
}

#pragma GCC visibility pop

typedef char *(*probefunc)(struct volume_id *id, char *dev);

static char *probe_mbr(struct volume_id *id, char *dev)
{
	unsigned char *buf;

	buf = (unsigned char *)dev + strlen(dev) - 1;
	if (buf > (unsigned char *)dev && *buf <= '9')
		return NULL;

	buf = volume_id_get_buffer(id, 0, 0x200);
	if (buf == NULL)
		return NULL;

	/* signature */
	if (buf[0x1fe] != 0x55 || buf[0x1ff] != 0xaa)
		return NULL;

	/* boot flags */
	if ((buf[0x1be] | buf[0x1ce] | buf[0x1de] | buf[0x1ee]) & 0x7f)
		return NULL;

	return "mbr";
}

static char *probe_vfat(struct volume_id *id, char *dev)
{
	return volume_id_probe_vfat(id) ? NULL : "vfat";
}

static char *probe_swap(struct volume_id *id, char *dev)
{
	return volume_id_probe_linux_swap(id) ? NULL : "swap";
}

static char *probe_ext(struct volume_id *id, char *dev)
{
#define EXT3_FEATURE_COMPAT_HAS_JOURNAL		0x0004
#define EXT4_FEATURE_RO_COMPAT_HUGE_FILE	0x0008
#define EXT4_FEATURE_RO_COMPAT_DIR_NLINK	0x0020
#define EXT4_FEATURE_INCOMPAT_EXTENTS		0x0040
#define EXT4_FEATURE_INCOMPAT_64BIT		0x0080

	unsigned char *buf;

	if (volume_id_probe_ext(id) != 0)
		return NULL;

	buf = volume_id_get_buffer(id, 0x400, 0x200);
	if (buf == NULL)
		return NULL;

	if (*(uint32_t *)(buf + 0x64) & cpu_to_le32(EXT4_FEATURE_RO_COMPAT_HUGE_FILE | EXT4_FEATURE_RO_COMPAT_DIR_NLINK) ||
	    *(uint32_t *)(buf + 0x60) & cpu_to_le32(EXT4_FEATURE_INCOMPAT_EXTENTS | EXT4_FEATURE_INCOMPAT_64BIT))
		return "ext4";

	if (*(uint32_t *)(buf + 0x5c) & cpu_to_le32(EXT3_FEATURE_COMPAT_HAS_JOURNAL))
		return "ext3";

	return "ext2";
}

static char *probe_ntfs(struct volume_id *id, char *dev)
{
	return volume_id_probe_ntfs(id) ? NULL : "ntfs";
}

#ifdef RTCONFIG_HFS
static char *probe_hfs(struct volume_id *id, char *dev)
{
	unsigned char *buf;
	unsigned sig = 0;

	if (volume_id_probe_hfs_hfsplus(id) != 0)
		return NULL;

	buf = volume_id_get_buffer(id, 0x400, 0x200);
	if (buf == NULL)
		return NULL;

	if (buf[0] == 'B' && buf[1] == 'D')
		sig += 124;

	if (buf[sig] == 'H') {
		if (buf[sig + 1] == '+') /* ordinary HFS+ */
			return "hfs+j";
		if (buf[sig + 1] == 'X') /* case-sensitive HFS+ */
			return "hfs+jx";
	}

	return sig ? "hfs" : NULL;
}
#endif

#if defined(RTCONFIG_APFS) && !defined(RTCONFIG_TUXERA_APFS)
struct apfs_prange {
	unsigned long long pr_start_paddr;
	unsigned long long pr_block_count;
}  __attribute__((packed));

struct apfs_obj_phys {
						/*00*/ unsigned long long o_cksum;
						/* Fletcher checksum */
	unsigned long long o_oid;	/* Object-id */
						/*10*/ unsigned long long o_xid;
						/* Transaction ID */
	unsigned int o_type;	/* Object type */
	unsigned int o_subtype;	/* Object subtype */
}  __attribute__((packed));

/* Indexes into a container superblock's array of counters */
enum {
	APFS_NX_CNTR_OBJ_CKSUM_SET = 0,
	APFS_NX_CNTR_OBJ_CKSUM_FAIL = 1,

	APFS_NX_NUM_COUNTERS = 32
};
#define APFS_NX_EPH_INFO_COUNT			4

#define APFS_NX_MAX_FILE_SYSTEMS		100

/*
 * On-disk representation of the container superblock
 */
struct apfs_nx_superblock {
/*00*/ struct apfs_obj_phys nx_o;
/*20*/ unsigned int nx_magic;
	unsigned int nx_block_size;
	unsigned long long nx_block_count;

/*30*/ unsigned long long nx_features;
	unsigned long long nx_readonly_compatible_features;
	unsigned long long nx_incompatible_features;

/*48*/ char nx_uuid[16];

/*58*/ unsigned long long nx_next_oid;
	unsigned long long nx_next_xid;

/*68*/ unsigned int nx_xp_desc_blocks;
	unsigned int nx_xp_data_blocks;
/*70*/ unsigned long long nx_xp_desc_base;
	unsigned long long nx_xp_data_base;
	unsigned int nx_xp_desc_next;
	unsigned int nx_xp_data_next;
/*88*/ unsigned int nx_xp_desc_index;
	unsigned int nx_xp_desc_len;
	unsigned int nx_xp_data_index;
	unsigned int nx_xp_data_len;

/*98*/ unsigned long long nx_spaceman_oid;
	unsigned long long nx_omap_oid;
	unsigned long long nx_reaper_oid;

/*B0*/ unsigned int nx_test_type;

	unsigned int nx_max_file_systems;
/*B8*/ unsigned long long nx_fs_oid[APFS_NX_MAX_FILE_SYSTEMS];
/*3D8*/ unsigned long long nx_counters[APFS_NX_NUM_COUNTERS];
/*4D8*/ struct apfs_prange nx_blocked_out_prange;
	unsigned long long nx_evict_mapping_tree_oid;
/*4F0*/ unsigned long long nx_flags;
	unsigned long long nx_efi_jumpstart;
/*500*/ char nx_fusion_uuid[16];
	struct apfs_prange nx_keylocker;
/*520*/ unsigned long long nx_ephemeral_info[APFS_NX_EPH_INFO_COUNT];

/*540*/ unsigned long long nx_test_oid;

	unsigned long long nx_fusion_mt_oid;
/*550*/ unsigned long long nx_fusion_wbc_oid;
	struct apfs_prange nx_fusion_wbc;
}  __attribute__((packed));

struct apfs_wrapped_meta_crypto_state {
	unsigned short major_version;
	unsigned short minor_version;
	unsigned int cpflags;
	unsigned int persistent_class;
	unsigned int key_os_version;
	unsigned short key_revision;
	unsigned short unused;
}  __attribute__((packed));

#define APFS_MODIFIED_NAMELEN	      32

/*
 * Structure containing information about a program that modified the volume
 */
struct apfs_modified_by {
	char id[APFS_MODIFIED_NAMELEN];
	unsigned long long timestamp;
	unsigned long long last_xid;
}  __attribute__((packed));

#define APFS_MAX_HIST				8
#define APFS_VOLNAME_LEN			256

struct apfs_superblock {
/*00*/ struct apfs_obj_phys apfs_o;

/*20*/ unsigned int apfs_magic;
	unsigned int apfs_fs_index;

/*28*/ unsigned long long apfs_features;
	unsigned long long apfs_readonly_compatible_features;
	unsigned long long apfs_incompatible_features;

/*40*/ unsigned long long apfs_unmount_time;

	unsigned long long apfs_fs_reserve_block_count;
	unsigned long long apfs_fs_quota_block_count;
	unsigned long long apfs_fs_alloc_count;

/*60*/ struct apfs_wrapped_meta_crypto_state apfs_meta_crypto;

/*74*/ unsigned int apfs_root_tree_type;
	unsigned int apfs_extentref_tree_type;
	unsigned int apfs_snap_meta_tree_type;

/*80*/ unsigned long long apfs_omap_oid;
	unsigned long long apfs_root_tree_oid;
	unsigned long long apfs_extentref_tree_oid;
	unsigned long long apfs_snap_meta_tree_oid;

/*A0*/ unsigned long long apfs_revert_to_xid;
	unsigned long long apfs_revert_to_sblock_oid;

/*B0*/ unsigned long long apfs_next_obj_id;

/*B8*/ unsigned long long apfs_num_files;
	unsigned long long apfs_num_directories;
	unsigned long long apfs_num_symlinks;
	unsigned long long apfs_num_other_fsobjects;
	unsigned long long apfs_num_snapshots;

/*E0*/ unsigned long long apfs_total_blocks_alloced;
	unsigned long long apfs_total_blocks_freed;

/*F0*/ char apfs_vol_uuid[16];
/*100*/ unsigned long long apfs_last_mod_time;

	unsigned long long apfs_fs_flags;

/*110*/ struct apfs_modified_by apfs_formatted_by;
/*140*/ struct apfs_modified_by apfs_modified_by[APFS_MAX_HIST];

/*2C0*/ char apfs_volname[APFS_VOLNAME_LEN];
/*3C0*/ unsigned int apfs_next_doc_id;

	unsigned short apfs_role;
	unsigned short reserved;

/*3C8*/ unsigned long long apfs_root_to_xid;
	unsigned long long apfs_er_state_oid;
}  __attribute__((packed));

#define FIRST_VOL_BNO 20002
#define APPLE_NX_DEFAULT_BLOCKSIZE 4096
#define APFS_NX_MAGIC				0x4253584E
#endif

#ifdef RTCONFIG_APFS
static char *probe_apfs(struct volume_id *id, char *dev)
{
#ifdef RTCONFIG_TUXERA_APFS
	char cmd[32], buf[PATH_MAX];
	FILE *fp;

	memset(id->uuid, 0, sizeof(id->uuid));

	snprintf(cmd, sizeof(cmd), "/usr/sbin/apfsinfo %s", dev);
	if((fp = popen(cmd, "r")) != NULL){
		int first = 1;
		while(fgets(buf, sizeof(buf), fp) != NULL){
			if(!strncmp(buf, "UUID", 4) && !first){
				first = 0;

				sscanf(buf, "UUID: %s", id->uuid);
				break;
			}
			else if(!strncmp(buf, "Name", 4))
				sscanf(buf, "Name: %s %*s", id->label);
		}
		fclose(fp);
	}

	return *(id->label) || *(id->uuid) ? "apfs" : NULL;
#else
	FILE *fp;
	struct apfs_superblock *sb;
	struct apfs_nx_superblock *nxsb;

	memset(id->uuid, 0, sizeof(id->uuid));
	memset(id->label, 0, sizeof(id->label));
	if((fp = fopen(dev, "rb")) != NULL){
		nxsb = (struct apfs_nx_superblock *) malloc(sizeof(struct apfs_nx_superblock));
		fseek(fp, 0, SEEK_SET);
		fread(nxsb, 1, sizeof(struct apfs_nx_superblock), fp);
		if(le32toh(nxsb->nx_magic) == APFS_NX_MAGIC){
			strlcpy(id->uuid, nxsb->nx_uuid, strlen(nxsb->nx_uuid));
			sb = (struct apfs_superblock *) malloc(sizeof(struct apfs_superblock));
			fseek(fp, FIRST_VOL_BNO * le32toh(nxsb->nx_block_size), SEEK_SET);
			fread(sb, 1, sizeof(struct apfs_superblock), fp);
			if(le32toh(sb->apfs_magic) == 0x42535041){
				strlcpy(id->label, sb->apfs_volname, strlen(sb->apfs_volname));
				strlcpy(id->uuid, sb->apfs_vol_uuid, strlen(sb->apfs_vol_uuid));
			}
			free(sb);
		}
		free(nxsb);
		fclose(fp);
	}

	return *(id->label) || *(id->uuid) ? "apfs" : NULL;
#endif

	return NULL;
}
#endif

#ifdef RTCONFIG_USB_CDROM
static char *probe_udf(struct volume_id *id, char *dev)
{
	return volume_id_probe_udf(id) ? NULL : "udf";
}

static char *probe_iso(struct volume_id *id, char *dev)
{
	return volume_id_probe_iso9660(id) ? NULL : "iso9660";
}
#endif

/* Probe FS type and fill FS type, label and uuid, if required.
 * Return >0 if FS is detected or 0 if unknown.
 * Return -1 on error or no device found.
 */
int probe_fs(char *device, char **type, char *label, char *uuid)
{
	static const probefunc probe[] = {
		probe_mbr,
		probe_vfat,
	};
	static const probefunc probe_full[] = {
#ifdef RTCONFIG_USB_CDROM
		probe_udf,
		probe_iso,
#endif
		probe_swap,
		probe_ext,
		probe_ntfs,
#ifdef RTCONFIG_HFS
		probe_hfs,
#endif
	};
	struct volume_id id;
	char *fstype = NULL;
	int i;

	memset(&id, 0, sizeof(id));
	if (type) *type = NULL;
	if (label) *label = '\0';
	if (uuid) *uuid = '\0';

	if ((id.fd = open(device, O_RDONLY)) < 0) {
		_dprintf("%s: open %s failed: %s", __FUNCTION__, device, strerror(errno));
		return -1;
	}

	/* signature in the first block, only small buffer needed */
	for (i = 0; i < ARRAY_SIZE(probe); i++) {
		fstype = probe[i](&id, device);
		if (fstype || id.error)
			goto found;
	}

	/* fill buffer with maximum */
	volume_id_get_buffer(&id, 0, SB_BUFFER_SIZE);

	for (i = 0; i < ARRAY_SIZE(probe_full); i++) {
		fstype = probe_full[i](&id, device);
		if (fstype || id.error)
			goto found;
	}

found:
	volume_id_free_buffer(&id);
	close(id.fd);

#ifdef RTCONFIG_APFS
	if(fstype == NULL)
		fstype = probe_apfs(&id, device);
#endif

	if (fstype == NULL && id.error) {
		_dprintf("%s: probe %s failed: %s", __FUNCTION__, device, strerror(errno));
		return -1;
	}

	if (type)
		*type = fstype;
	if (label && *id.label != 0)
		strcpy(label, id.label);
	if (uuid && *id.uuid != 0)
		strcpy(uuid, id.uuid);

	return fstype ? 1 : 0;
}

/* Detect FS type.
 * Return FS type or "unknown" if not known/detected.
 * Return NULL on error or no device found.
 */
char *detect_fs_type(char *device)
{
	char *type;

	if (probe_fs(device, &type, NULL, NULL) < 0)
		return NULL;

	return type ? : "unknown";
}

/* Put the label in *label and uuid in *uuid.
 * Return >0 if there is a label/uuid or 0 if both are absent.
 * Return -1 on error or no device found.
 */
int find_label_or_uuid(char *device, char *label, char *uuid)
{
	if (probe_fs(device, NULL, label, uuid) < 0)
		return -1;

	return (label && *label) || (uuid && *uuid);
}
