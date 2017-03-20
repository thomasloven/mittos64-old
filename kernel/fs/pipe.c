#include <vfs.h>
#include <stdint.h>
#include <sync.h>
#include <mem.h>
#include <debug.h>
#include <scheduler.h>

#define PIPE_SIZE 0x1000

struct pipe_data
{
  uint8_t *buffer;
  lock_t lock;
  uint64_t size;
  uint64_t readers;
  uint64_t writers;
  uint64_t read_pos;
  uint64_t write_pos;
  file_t *reader;
  file_t *writer;
};

int pipe_open(file_t *file, uint64_t flags)
{
  (void) flags;
  struct pipe_data *d = file->data;
  spin_lock(&d->lock);
  if(file == d->reader)
    d->readers ++;
  else
    d->writers ++;
  spin_unlock(&d->lock);
  return 0;
}

int pipe_close(file_t *file)
{
  debug("Closing pipe\n");
  struct pipe_data *d = file->data;
  spin_lock(&d->lock);
  if(file == d->reader)
    d->readers --;
  else
    d->writers --;
  if((!d->readers) && !(d->writers))
  {
    debug("Freeing pipe\n");
    kfree(d);
    kfree(file);
  } else {
    spin_unlock(&d->lock);
  }
  return 0;
}

size_t pipe_read(file_t *file, void *buffer, size_t nbyte, size_t offset)
{
  (void) offset;
  struct pipe_data *d = file->data;
  uint8_t *buf = buffer;

  spin_lock(&d->lock);
  while(d->read_pos >= d->write_pos)
  {
    spin_unlock(&d->lock);
    schedule();
    spin_lock(&d->lock);
  }

  uint64_t bytes = d->write_pos - d->read_pos;
  if(bytes > nbyte) bytes = nbyte;

  uint64_t buf_pos = d->read_pos % d->size;
  uint64_t n = d->size - buf_pos;
  if(n > bytes) n = bytes;

  memcpy(buf, &d->buffer[buf_pos], n);
  if(bytes-n)
    memcpy(&buf[n], d->buffer, bytes-n);

  d->read_pos += bytes;

  spin_unlock(&d->lock);
  return bytes;
}

size_t pipe_write(file_t *file, void *buffer, size_t nbyte, size_t offset)
{
  (void) offset;
  struct pipe_data *d = file->data;
  uint8_t *buf = buffer;
  uint64_t written = 0;

  spin_lock(&d->lock);
  while(nbyte)
  {
    while(d->write_pos >= d->read_pos + d->size)
    {
      spin_unlock(&d->lock);
      schedule();
      spin_lock(&d->lock);
    }

    uint64_t bytes = d->size - (d->write_pos - d->read_pos);
    if(bytes > nbyte) bytes = nbyte;

    uint64_t buf_pos = d->write_pos % d->size;
    uint64_t n = d->size - buf_pos;
    if(n > bytes) n = bytes;

    memcpy(&d->buffer[buf_pos], &buf[written], n);
    if(bytes-n)
      memcpy(d->buffer, &buf[written + n], bytes-n);

    nbyte -= bytes;
    d->write_pos += bytes;
    written += bytes;
  }

  spin_unlock(&d->lock);
  return written;
}

fs_driver_t pipe_writer = {
  .open = pipe_open,
  .close = pipe_close,
  .write = pipe_write,
};
fs_driver_t pipe_reader = {
  .open = pipe_open,
  .close = pipe_close,
  .read = pipe_read,
};

int pipe(file_t **r, file_t **w)
{
  *r = kcalloc(1, sizeof(file_t));
  *w = kcalloc(1, sizeof(file_t));
  struct pipe_data *data = (*r)->data = (*w)->data = kcalloc(1, sizeof(struct pipe_data));
  (*r)->driver = &pipe_reader;
  (*w)->driver = &pipe_writer;

  data->buffer = kcalloc(1, PIPE_SIZE);
  data->size = PIPE_SIZE;
  spin_unlock(&data->lock);
  data->reader = *r;
  data->writer = *w;

  return 0;
}
