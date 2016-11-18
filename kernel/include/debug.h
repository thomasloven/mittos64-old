#pragma once
#include <string.h>
#include <int.h>
#include <sync.h>

#ifndef NDEBUG
  #define debug(...) ({spin_lock(&debug_lock);debug_printf(__VA_ARGS__);spin_unlock(&debug_lock);})
#else
  #define debug(...) ((void)0)
#endif
#define debug_info(...) do{debug("[INFO] ");debug(__VA_ARGS__);}while(0)
#define debug_ok(...) do{debug("[OK] ");debug(__VA_ARGS__);}while(0)
#define debug_warning(...) do{debug("[WARNING] ");debug(__VA_ARGS__);}while(0)
#define debug_error(...) do{debug("[ERROR] ");debug(__VA_ARGS__);}while(0)

extern lock_t debug_lock;

void debug_init();
void debug_putch(char c);
void debug_putsn(char *s, size_t n);
void debug_puts(char *s);
void debug_printf(char *, ...);

char *_kernel_build_date;
char *_kernel_build_time;

char *_kernel_git_hash;
char *_kernel_git_date;
int kernel_git_dirty;
char *_kernel_git_message;
char *_kernel_git_branch;

void debug_build_time();
void debug_git_info();

void print_registers(registers_t *);
