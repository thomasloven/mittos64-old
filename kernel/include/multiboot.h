#pragma once
#define MBOOT1_MAGIC 0x1BADB002
#define MBOOT1_MAGIC2 0x2BADB002
#define MBOOT1_FLAG_PAGE_ALIGN 0x01
#define MBOOT1_FLAG_MEM_INFO 0x02
#define MBOOT1_HEADER_FLAGS (MBOOT1_FLAG_PAGE_ALIGN | MBOOT1_FLAG_MEM_INFO)
#define MBOOT1_HEADER_CHECKSUM -(MBOOT1_HEADER_FLAGS + MBOOT1_MAGIC)

#define MBOOT2_MAGIC 0xE85250D6
#define MBOOT2_MAGIC2 0x36D76289
#define MBOOT2_ARCH 0
#define MBOOT2_LENGTH (MultiBootHeaderEnd - MultiBootHeader)
#define MBOOT2_CHECKSUM -(MBOOT2_MAGIC + MBOOT2_ARCH + MBOOT2_LENGTH)

#ifndef __ASSEMBLER__
#include <stdint.h>

// MULTIBOOT 1
  typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t symbol_num;
    uint32_t symbol_size;
    uint32_t symbol_addr;
    uint32_t symbol_shndx;
    uint32_t mmap_len;
    uint32_t mmap_addr;
    uint32_t drives_len;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t bootloader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
  } mboot1_info;

// MULTIBOOT 2

  typedef struct {
    uint32_t total_size;
    uint32_t reserved;
  } mboot2_tags_head;

  typedef struct {
    uint32_t type;
    uint32_t size;
    char data[];
  } mboot2_tag_basic;


  #define MBOOT2_CMDLINE 1
  #define MBOOT2_BOOTLOADER 2

  // Multiboot tags are padded to a multiple of 8 bytes
  #define next_tag(tag) ((void *)((((uintptr_t)tag) + tag->size + 7) & ~0x7))

  extern char *mboot_tags_type[];
  struct mboot_data_st{
    int version;
    void *data;
    char *commandline;
    char *bootloader;
  };
  extern struct mboot_data_st mboot_data;

  void multiboot_init(uint64_t magic, void *data);

#endif
