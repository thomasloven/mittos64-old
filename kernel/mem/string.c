#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <mem.h>

// Standard function required by gcc
// Just the naíve implementations for now

void *memcpy(void *dest, const void *src, size_t n)
{
  char *dp = dest;
  const char *sp = src;
  while(n--) *dp++ = *sp++;
  return dest;
}

void *memset(void *s, int c, size_t n)
{
  unsigned char *p = s;
  while(n--) *p++ = (unsigned char)c;
  return s;
}

void *memmove(void *dest, const void *src, size_t n)
{
  // Since our memcpy implementation copies one void * at a time, this is safe
  memcpy(dest, src, n);
  return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  const unsigned char *p1 = s1, *p2 = s2;
  for(; n--; p1++, p2++)
  {
    if (*p1 != *p2)
      return *p1 - *p2;
  }
  return 0;
}

size_t strlen(const char *s)
{
  size_t len = 0;
  while(*s++) len++;
  return len;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  while(n && *s1 && (*s1==*s2))
    s1++, s2++, n--;
  return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

int strcmp(const char *s1, const char *s2)
{
  while(*s1 && (*s1==*s2))
    s1++, s2++;
  return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

char *strdup(const char *s)
{
  char *ret = kmalloc(strlen(s)+1);
  memcpy(ret, s, strlen(s)+1);
  return ret;
}
