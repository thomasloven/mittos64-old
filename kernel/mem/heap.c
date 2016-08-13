#include <mem.h>
#include <stdint.h>
#include <stddef.h>
#include <debug.h>
#include <string.h>

typedef struct chunk_st
{
  struct chunk_st *p, *n;
  uint8_t flags;
  size_t size;
  uint8_t data[];
} chunk_t;
#define CHUNK_FREE 0x1

uintptr_t heap_max = KERNEL_HEAP_S;
uintptr_t brk = KERNEL_HEAP_S;

chunk_t *heap = 0;

void *ksbrk(size_t size)
{
  // This can only be used to increase the size of the kernel heap
  // for now...
  while(brk < heap_max + size)
  {
    vmm_set_page(0, brk, (uintptr_t)pmm_alloc(), PAGE_PRESENT | PAGE_WRITE);
    brk += PAGE_SIZE;
  }
  heap_max += size;
  return (void *)heap_max;
}

void merge_chunk(chunk_t *c)
{
  // Join free chunks together if they are next to each other

  if(c->n && c->n->flags & CHUNK_FREE) // Merge with next
  {
    // Update size
    c->size += c->n->size + sizeof(chunk_t);
    // Update list
    c->n = c->n->n;
    if(c->n)
      c->n->p = c;
  }
  if(c->p && c->p->flags & CHUNK_FREE) // Merge with previous
  {
    // Update size
    c->p->size += c->size + sizeof(chunk_t);
    // Update list
    c->p->n = c->n;
    if(c->n)
      c->n->p = c->p;
  }
}

void split_chunk(chunk_t *c, size_t size)
{
  // Split a chunk into one of given size and one of with the rest

  if(c->size <= size + sizeof(chunk_t))
    return;

  // Split off as much as isn't needed
  chunk_t *c2 = incptr(c->data, size);
  c2->size = c->size - size - sizeof(chunk_t);
  c2->flags = CHUNK_FREE;

  // Insert into list
  c2->n = c->n;
  if(c2->n) c2->n->p = c2;
  c->n = c2;
  c2->p = c;

  // Change size of first chunk
  c->size = size;
}

void kfree(void *a)
{
  if((uintptr_t)a < KERNEL_HEAP_S || (uintptr_t)a > heap_max)
  {
    debug_error("PANIC: kfree - Tried to free %x\n", a);
    for(;;);
  }

  // Find chunk header and mark as free
  chunk_t *c = incptr(a, -sizeof(chunk_t));
  c->flags = CHUNK_FREE;
  merge_chunk(c);
}

void *kmalloc(size_t size)
{
  chunk_t *c = heap, *p = 0;
  while(c)
  {
    // Linear search of list for free chunk

    if((c->flags & CHUNK_FREE) && c->size >= size)
    {
      // A large enough free chunk has been found
      // adjust and return
      split_chunk(c, size);
      c->flags = 0;
      return (void *)c->data;
    }
    p = c;
    c = c->n;
  }

  // No free chunk of right size found
  // increase heap size
  if(p)
  {
    c = incptr(p, p->size + sizeof(chunk_t));
  } else {
    c = heap = (chunk_t *)heap_max;
  }

  void *end = ksbrk(size + sizeof(chunk_t));
  // Add a new chunk
  c->size = (uintptr_t)end - (uintptr_t)c->data;
  c->flags = CHUNK_FREE;
  c->n = c->p = 0;
  c->p = p;
  if(p) c->n = p->n;
  if(p) p->n = c;
  // And try again...
  return kmalloc(size);
}

void *kcalloc(size_t count, size_t size)
{
  void *ptr = kmalloc(count*size);
  memset(ptr, 0, count*size);
  return ptr;
}

void *krealloc(void *ptr, size_t size)
{
  void *new = kmalloc(size);
  if(ptr)
  {
    memcpy(new, ptr, size);
    kfree(ptr);
  }
  return new;
}

void heap_print()
{
  // Print all heap chunks

  chunk_t *c = heap;
  debug_info("Heap map:\n");
  while(c)
  {
    debug("s:%x f:%x\n", c->size, c->flags);
    c = c->n;
  }
}
