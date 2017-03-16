#include <vfs.h>
#include <ata.h>
#include <string.h>
#include <debug.h>
#include <mem.h>

size_t ata_read(file_t *file, void *buffer, size_t nbyte, size_t offset)
{
  ata_drive *drv = file->data;
  if(!drv->exists)
    return 0;

  uint64_t blocksize = 512;
  char blk_buf[blocksize];
  char *out_buf = buffer;

  size_t count = 0;
  while(count < nbyte)
  {
    uint64_t block = offset/blocksize;
    uint64_t bloff = offset%blocksize;
    uint64_t len = (nbyte-count > blocksize-bloff)?blocksize-bloff:nbyte-count;

    if(len == blocksize)
    {
      ata_read_block(drv, block, &out_buf[count]);
    } else {
      ata_read_block(drv, block, blk_buf);
      memcpy(&out_buf[count], &blk_buf[bloff], len);
    }

    count += len;
    offset += len;
  }
  return count;
}

size_t ata_write(file_t *file, void *buffer, size_t nbyte, size_t offset)
{
  ata_drive *drv = file->data;
  if(!drv->exists)
    return 0;

  uint64_t blocksize = 512;
  char blk_buf[blocksize];
  char *in_buf = buffer;

  size_t count = 0;
  while(count < nbyte)
  {
    uint64_t block = offset/blocksize;
    uint64_t bloff = offset%blocksize;
    uint64_t len = (nbyte-count > blocksize-bloff)?blocksize-bloff:nbyte-count;

    if(len == blocksize)
    {
      ata_write_block(drv, block, &in_buf[count]);
    } else {
      ata_read_block(drv, block, blk_buf);
      memcpy(&blk_buf[bloff], &in_buf[count], len);
      ata_write_block(drv, block, blk_buf);
    }

    count += len;
    offset += len;
  }
  return count;
}

fs_driver_t ata_fs = {
  .read = ata_read,
  .write = ata_write,
};

void ata_fs_init()
{
  ata_init();
  file_t *files = kcalloc(4, sizeof(file_t));
  char path[] = "/dev/sdX";
  for(int i=0; i<4; i++)
  {
    files[i].type = FS_FILE;
    files[i].driver = &ata_fs;
    files[i].data = &drives[i];
    path[strlen(path)-1] = 'a' + i;
    fs_mount(&files[i], path);
  }
}
