#include <syscall.h>
#include <stdint.h>
#include <stddef.h>
#include <debug.h>
#include <mem.h>
#include <string.h>
#include <process.h>
#include <vfs.h>

SYSCALL_DEF(open)
{
  SYSCALL_INIT(char *, path, int, flags, int, mode);
  process_t *p = get_current_process();
  for(int i = 0; i < PROC_NUMFP; i++)
  {
    if(!p->fp[i].file)
    {
      p->fp[i].file = fs_namef(path);
      p->fp[i].flags = flags;
      fs_open(p->fp[i].file, flags);
      p->fp[i].pos = 0;
      if(p->fp[i].file)
        return i;
      break;
    }
  }
  return -1;
}

SYSCALL_DEF(close)
{
  SYSCALL_INIT(int, fd);
  process_t *p = get_current_process();
  int retval = fs_close(p->fp[fd].file);
  fs_put(p->fp[fd].file);
  p->fp[fd].file = 0;
  return retval;
}

SYSCALL_DEF(isatty)
{
  SYSCALL_INIT(int, fd);
  process_t *p = get_current_process();
  if(p->fp[fd].file && ((p->fp[fd].file->type & FS_TTY) == FS_TTY))
    return 1;
  return 0;
}

SYSCALL_DEF(read)
{
  SYSCALL_INIT(int, fd, void *, buffer, size_t, nbyte);
  process_t *p = get_current_process();
  int64_t bytes = fs_read(p->fp[fd].file, buffer, nbyte, p->fp[fd].pos);
  p->fp[fd].pos += bytes;
  return bytes;
}

SYSCALL_DEF(write)
{
  SYSCALL_INIT(int, fd, void *, buffer, size_t, nbyte);

  process_t *p = get_current_process();
  int64_t bytes = fs_write(p->fp[fd].file, buffer, nbyte, p->fp[fd].pos);
  p->fp[fd].pos += bytes;
  return bytes;
}

SYSCALL_DEF(seek)
{
  SYSCALL_INIT(int, fd, long, offset, int, whence);
  process_t *p = get_current_process();
  if(!p->fp[fd].file)
    return -1;
  if(whence == 0)
    p->fp[fd].pos = offset;
  else if(whence == 1)
    p->fp[fd].pos += offset;

  return p->fp[fd].pos;
}

SYSCALL_DEF(pipe)
{
  SYSCALL_INIT(int *, fd);
  process_t *p = get_current_process();
  file_t *r, *w;
  pipe(&r, &w);

  fd[0] = -1;
  fd[1] = -1;

  int i;
  for(i = 0; i < PROC_NUMFP; i++)
  {
    if(!p->fp[i].file)
    {
      p->fp[i].file = fs_get(w);
      fs_open(w,0);
      fd[0] = i;
      break;
    }
  }
  if(i == PROC_NUMFP)
    goto pipe_fail;
  for(i = 0; i < PROC_NUMFP; i++)
  {
    if(!p->fp[i].file)
    {
      p->fp[i].file = fs_get(r);
      fs_open(r,0);
      fd[1] = i;
      break;
    }
  }
  if(i == PROC_NUMFP)
    goto pipe_fail;

  return 0;

pipe_fail:
  if(r)
    kfree(r);
  if(w)
    kfree(w);
  if(fd[0] != -1)
  {
    fs_put(p->fp[fd[0]].file);
    p->fp[fd[0]].file = 0;
  }
  if(fd[1] != -1)
  {
    fs_put(p->fp[fd[1]].file);
    p->fp[fd[1]].file = 0;
  }
  return -1;
}
