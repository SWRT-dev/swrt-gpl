/** \file xmlfs.c
 *  \brief source for mount.fuse.xml
 *         
 * This is the source file for fuse xml file system
 * \author blunderer <blunderer@blunderer.org>
 * \date 15 Apr 2009
 *
 * Copyright (C) 2009 blunderer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * The author added a static linking exception, see License.txt.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef XMLFS_C
#define XMLFS_C

#define FUSE_USE_VERSION 26
 
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
 
#include <roxml.h>
 

#define MAX_ENTRIES	512
#define LOG_FILE	"/var/log/xmlfs.log"
#define NODE_CONTENT	"content.data"

node_t ** opened_files = NULL;
FILE *debug_f = NULL;

#define DEBUG(...) { if(debug_f) { fprintf(debug_f,"%s::%s:%d ",__FILE__,__func__,__LINE__);fprintf(debug_f,__VA_ARGS__); fprintf(debug_f,"\n"); fflush(debug_f); } }
 
static int xmlfs_getattr(const char *path, struct stat *stbuf)
{
	DEBUG("here")
	int nb;
	int fsize = 1;
	int content = 0;
	node_t *n = NULL;
	node_t *root = fuse_get_context()->private_data;
	char newpath[512];
	char *ptr;

	strcpy(newpath, path);
	if(ptr = strstr(newpath, NODE_CONTENT))	{
		*(ptr-1) = 0;
		content = 1;
	}

	DEBUG("trying '%s'",newpath);
	node_t **ans = roxml_xpath(root, newpath, &nb);

	if(ans) {
		DEBUG("got ans")
		n = ans[0];
		if((roxml_get_type(n)==ROXML_ATTR_NODE)||(content))	{
			char * txt = roxml_get_content(n, NULL, 0, NULL);
			fsize = strlen(txt);
			roxml_release(txt);
			stbuf->st_mode = S_IFREG;
			DEBUG("file stat : %d",fsize)
		} else {
			fsize = 1;
			stbuf->st_mode = S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
		}
		
		stbuf->st_ino = 0;
		stbuf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH /*| S_IWUSR | S_IWGRP | S_IWOTH*/;
		stbuf->st_nlink = 0;
		stbuf->st_uid = 0;
		stbuf->st_gid = 0;
		stbuf->st_rdev = 0;
		stbuf->st_size = fsize;
		stbuf->st_blksize = 1;
		stbuf->st_blocks = fsize;
		stbuf->st_atime = 0;
		stbuf->st_mtime = 0;
		stbuf->st_ctime = 0;

		DEBUG("will release")
		roxml_release(ans);
		DEBUG("released")
		return 0;
	}
	return -ENOENT;
}
 
static int xmlfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	DEBUG("here")
	DEBUG("trying '%s'",path);
	int nb;
	node_t *root = fuse_get_context()->private_data;
	node_t **ans = roxml_xpath(root, (char*)path, &nb);

	if(ans)	{
		int i;
		node_t *n;

		n = ans[0];
		roxml_release(ans);

		nb = roxml_get_chld_nb(n);
		DEBUG("%d dirs", nb)
		for(i = 0; i < nb; i++)	{
			int idx;
			char fname[512] = "";
			char rname[512] = "";
			node_t *tmp = roxml_get_chld(n, NULL, i);
			char *name = roxml_get_name(tmp, NULL, 0);
			DEBUG("get position of %s", name)
			idx = roxml_get_node_position(tmp);
			if(name == NULL) {
				switch(roxml_get_type(tmp)) {
					case ROXML_PI_NODE:
						strcpy(rname,"processing-instruction()");
					break;
					case ROXML_CMT_NODE:
						strcpy(rname,"comment()");
					break;
				}
				name = rname;
			}
			if(idx > 1)	{
				DEBUG("here")
				sprintf(fname,"%s[%d]",name,idx);
				filler(buf, fname, NULL, 0);
			} else	{
				DEBUG("here")
				filler(buf, name, NULL, 0);
			}
			roxml_release(name);
			DEBUG("here")
		}
		nb = roxml_get_attr_nb(n);
		DEBUG("%d files", nb)
		for(i = 0; i < nb; i++)	{
			char fname[512] = "@";
			char *name = roxml_get_name(roxml_get_attr(n, NULL, i), NULL, 0);
			strcat(fname, name);
			filler(buf, fname, NULL, 0);
			roxml_release(name);
		}
		nb = roxml_get_txt_nb(n);
		if(nb > 0)	{
			DEBUG("content file")
			char fname[512];
			sprintf(fname, "%s", NODE_CONTENT);
			filler(buf, fname, NULL, 0);
		}
		if((roxml_get_type(n) == ROXML_PI_NODE)||(roxml_get_type(n) == ROXML_CMT_NODE)) {
			DEBUG("specnode file")
			char fname[512];
			sprintf(fname, "%s", NODE_CONTENT);
			filler(buf, fname, NULL, 0);
		}

		return 0;
	}
	return -ENOENT;
}
 
static int xmlfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	DEBUG("here %s", path)

	int nb = 0;
	char *dirname = strdup(path);
	char *filename = dirname;
	char *ptr = dirname;
	node_t *root = fuse_get_context()->private_data;

	while(ptr = strstr("/", dirname)) {
		filename = ptr;
	}
	filename[0] = '\0';
	filename++;

	DEBUG("file '%s' in '%s'", filename, dirname);
	node_t **parent = roxml_xpath(root, strlen(dirname)?dirname:"/", &nb);

	DEBUG("%d results", nb);
	if(nb > 0) {
		DEBUG("parent is : '%s'\n",roxml_get_name(parent[0], NULL, 0));
		roxml_add_node(parent[0], 0, ROXML_ELM_NODE, filename, NULL);
		roxml_release(RELEASE_LAST);
	} else {
		DEBUG("zero results for %s", dirname)
		return -ENOENT;
	}

	free(dirname);
	return 0;
}
 
static int xmlfs_open(const char *path, struct fuse_file_info *fi)
{
	DEBUG("here")
	int nb;
	node_t *root = fuse_get_context()->private_data;
	char newpath[512];
	char *ptr;

	strcpy(newpath, path);
	if(ptr = strstr(newpath, NODE_CONTENT))	{
		*(ptr-1) = 0;
	}
	node_t **ans = roxml_xpath(root, (char*)newpath, &nb);

	if(ans)	{
		int i = 0;
		node_t *n = ans[0];
		roxml_release(ans);
		while((i < MAX_ENTRIES)&&(opened_files[i]))	{ i++; } 
		if(i < MAX_ENTRIES)	{
			fi->fh = i;
			opened_files[i] = n;

			return 0;
		}
		return -ENOENT;
	}
	return -ENOENT;
}

static int xmlfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	DEBUG("here")
	int bsize;
	int bytes;
	node_t *n;
	char *content;
	if(size == 0) { return 0; }

	n = opened_files[fi->fh];

	if (!n) { return -ENOENT; }

	content = roxml_get_content(n, NULL, 0, NULL);
	bsize = strlen(content);

	if((size + offset) >= bsize)	{
		bytes = bsize - offset;
	} else {
		bytes = size;
	}
	memcpy(buf, content+offset, bytes); 

	roxml_release(content);

	return bytes;
}

static int xmlfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	DEBUG("here")
	return -ENOENT;
}

static int xmlfs_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
	DEBUG("here")
	return -ENOENT;
}

static int xmlfs_release(const char *path, struct fuse_file_info *fi)
{
	DEBUG("here")
	if(fi->fh < 0) { return -ENOENT; }
	if(fi->fh > MAX_ENTRIES) { return -ENOENT; }

	DEBUG("closing file %d",fi->fh)
	opened_files[fi->fh] = NULL;
	return 0;
}

void xmlfs_cleanup(void *data)
{
	DEBUG("here")
	DEBUG("freeing file table")
	free(opened_files);
}

int xmlfs_statfs(const char *path, struct statvfs *stats)
{
	DEBUG("here")
	stats->f_bsize = 0;
	stats->f_frsize = 0;
	stats->f_blocks = 0;
	stats->f_bfree = 0;
	stats->f_bavail = 0;
	stats->f_namemax = 512;
	stats->f_files = 1000000000;
	stats->f_ffree = 1000000000;
	return 0;
}

int xmlfs_truncate(const char *path, off_t size)
{
	DEBUG("here")
	return -ENOENT;
}

int xmlfs_ftruncate(const char *path, off_t size, struct fuse_file_info *fi)
{
	DEBUG("here")
	return -ENOENT;
}

int xmlfs_unlink(const char *path)
{
	DEBUG("here")
	return -ENOENT;
}

int xmlfs_rename(const char *from, const char *to)
{
	DEBUG("here")
	return -ENOENT;
}

int xmlfs_mkdir(const char *dir, mode_t ignored)
{
	DEBUG("here")
	return -ENOENT;
}

void *xmlfs_init(struct fuse_conn_info *conn)
{
	DEBUG("here")
	node_t *n = NULL;
	char *src = NULL;
	struct fuse_context *ctx = fuse_get_context();

	opened_files = (node_t**)malloc(sizeof(node_t*)*MAX_ENTRIES);

	src = (char*)ctx->private_data;
	DEBUG("mounting %s",src)

	n = roxml_load_doc(src);
	DEBUG("loaded doc to %p mount init ok", n);

	return n;
}

static struct fuse_operations xmlfs_oper = {
	.getattr = xmlfs_getattr,
	.statfs = xmlfs_statfs,
	.readdir = xmlfs_readdir,
	.mkdir = xmlfs_mkdir,
	.rmdir = xmlfs_unlink,
	.create = xmlfs_create,
	.open = xmlfs_open,
	.read = xmlfs_read,
	.write = xmlfs_write,
	.truncate = xmlfs_truncate,
	.ftruncate = xmlfs_ftruncate,
	.unlink = xmlfs_unlink,
	.rename = xmlfs_rename,
	.fsync = xmlfs_fsync,
	.release = xmlfs_release,
	.init = xmlfs_init,
	.destroy = xmlfs_cleanup
};

static int xmlfs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
	DEBUG("here")
	char *data_arg = (char*)data;
	static int num = 0;

	switch(key) {
		case FUSE_OPT_KEY_OPT:
			if(strcmp(arg, "--debug") == 0) {
				debug_f = fopen(LOG_FILE,"w");
				fprintf(stderr,"debug mode started\n");
				return 0;
			} else if(strcmp(arg, "-oallow-other") == 0) {
				return 0;
			}
			break;
		case FUSE_OPT_KEY_NONOPT:
			num++;
			if(num == 1)	{
				strcpy(data_arg, arg);
				return 0;
			}	
			break;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	DEBUG("here")
	char mount_src[512] = "";
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	if (argc < 2) {
		return -1;
	}

	if(fuse_opt_parse(&args, mount_src, NULL, xmlfs_opt_proc) == -1) {
		return -1;
	}
	fuse_opt_add_arg(&args, "-oallow_other");

	if(strlen(mount_src) == 0)	{
		return -1;
	} else if(mount_src[0] != '/')	{
		char tmp[512];
		strcpy(tmp, mount_src);
		getcwd(mount_src, 512);
		strcat(mount_src, "/");
		strcat(mount_src, tmp);
	}

	return fuse_main(args.argc, args.argv, &xmlfs_oper, mount_src);
}

#endif /* XMLFS_C */

