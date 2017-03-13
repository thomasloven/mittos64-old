#pragma once
#include <stdint.h>
#include <stddef.h>
#include <list.h>

typedef struct vfs_node_st * INODE;

typedef struct file_st
{
  uint64_t refs;
  uint64_t type;
  struct fs_driver_st *driver;
  void *data;
} file_t;

typedef struct dirent_st
{
  char name[256];
  file_t *file;
} dirent_t;

#define FS_FILE 0x1
#define FS_DIR  0x2
#define FS_PIPE 0x3
#define FS_TTY  0x4

typedef struct fs_driver_st
{
  int (*open)(file_t *file, uint64_t flags);
  int (*close)(file_t *file);
  size_t (*read)(file_t *file, void *buffer, size_t nbyte, size_t offset);
  size_t (*write)(file_t *file, void *buffer, size_t nbyte, size_t offset);
  int (*readdir)(file_t *dir, dirent_t *entry, uint64_t offset);
} fs_driver_t;

int fs_open(file_t *file, uint64_t flags);
int fs_close(file_t *file);
size_t fs_read(file_t *file, void *buffer, size_t nbyte, size_t offset);
size_t fs_write(file_t *file, void *buffer, size_t nbyte, size_t offset);
int fs_readdir(file_t *dir, dirent_t *entry, uint64_t offset);


struct mountpoint
{
 LIST(struct mountpoint, mountpoints);
 file_t *root;
 uint64_t path_len;
 char path[];
};

file_t *fs_get(file_t *file);
file_t *fs_put(file_t *file);

LIST(struct mountpoint, mountpoints);

void fs_mount(file_t *root, const char *path);
void fs_umount(const char *path);

file_t *fs_namef(const char *path);

file_t debug_file;
