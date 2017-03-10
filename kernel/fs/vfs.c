#include <vfs.h>
#include <stdint.h>
#include <stddef.h>
#include <mem.h>
#include <string.h>
#include <debug.h>

file_t *fs_get(file_t *file)
{
  if(!file)
    file = kcalloc(1, sizeof(file_t));
  file->refs += 1;
  return file;
}
file_t *fs_put(file_t *file)
{
  if(!file)
    return file;
  file->refs -= 1;
  if(!file->refs)
  {
    kfree(file);
    file = 0;
  }
  return file;
}

int fs_open(file_t *file, uint64_t flags)
{
  if(file && file->driver && file->driver->open)
    return file->driver->open(file, flags);
  return -1;
}

int fs_close(file_t *file)
{
  if(file && file->driver && file->driver->close)
    return file->driver->close(file);
  return -1;
}

size_t fs_read(file_t *file, void *buffer, size_t nbyte, size_t offset)
{
  if(file && file->driver && file->driver->read)
    return file->driver->read(file, buffer, nbyte, offset);
  return -1;
}

size_t fs_write(file_t *file, void *buffer, size_t nbyte, size_t offset)
{
  if(file && file->driver && file->driver->write)
    return file->driver->write(file, buffer, nbyte, offset);
  return -1;
}

int fs_readdir(file_t *dir, dirent_t *entry, uint64_t offset)
{
  if(dir && dir->driver && dir->driver->readdir)
    return dir->driver->readdir(dir, entry, offset);
  return 0;
}

file_t *fs_finddir(file_t *dir, char *name)
{
  dirent_t entry;
  int offset = 0;
  while(1)
  {
    int retval = fs_readdir(dir, &entry, offset++);
    if(!retval)
      break;
    if(strcmp((&entry)->name, name))
    {
      fs_put(entry.file);
      continue;
    }
    return entry.file;
  }
  return 0;
}


void fs_mount(file_t *root, const char *path)
{
  struct mountpoint *mp = kcalloc(1, sizeof(struct mountpoint) + strlen(path) + 1);
  mp->root = fs_get(root);
  mp->path_len = strlen(path);
  memcpy(mp->path, path, mp->path_len);
  mp->path[mp->path_len] = '\0';
  LIST_APPEND(mountpoints, mp, mountpoints);
}

void fs_umount(const char *path)
{
  LIST_FOREACH(mountpoints, struct mountpoint, mp, mountpoints)
  {
    if(!strcmp(mp->path, path))
    {
      LIST_REMOVE(mountpoints, mp, mountpoints);
      fs_put(mp->root);
      kfree(mp);
      return;
    }
  }
}

struct mountpoint *fs_closest_mp(const char *path)
{
  uint64_t best_len = 0;
  struct mountpoint *best = 0;
  LIST_FOREACH(mountpoints, struct mountpoint, mp, mountpoints)
  {
    uint64_t len = mp->path_len;
    if((!strncmp(mp->path, path, len-1)) && (len > best_len))
    {
      best = mp;
      best_len = len;
    }
  }
  return best;
}


char *pathtok(char **p)
{
  char sep = '/';

  if(!**p) return 0;

  while(**p && **p == sep) (*p)++;
  char *ret = *p;
  while(**p && **p != sep) (*p)++;
  if(**p) **p = '\0', (*p)++;
  return ret;
}

file_t *fs_namef(const char *path)
{
  char *name;
  char *p = strdup(path);
  char *q = p;
  struct mountpoint *mp = fs_closest_mp(path);
  file_t *root = mp->root;
  p += mp->path_len;
  file_t *dir = fs_get(root);

  while((name = pathtok(&p)))
  {
    file_t *next = fs_finddir(dir, name);
    if(!next)
    {
      dir =  0;
      goto end;
    }
    fs_put(dir);
    dir = next;
  }
end:
  kfree(q);
  return dir;
}
