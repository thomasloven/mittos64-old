#include <vfs.h>
#include <debug.h>

size_t debug_write(file_t *file, void *buffer, size_t nbyte, size_t offset)
{
  (void)file;
  (void)offset;
  debug_putsn(buffer, nbyte);
  return nbyte;
}

fs_driver_t debug_fs = {
  .write = debug_write,
};

file_t debug_file = {
  .refs = 1,
  .type = FS_TTY,
  .driver = &debug_fs,
};
