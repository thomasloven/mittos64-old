#pragma once
#include <stdint.h>

#define ATA_PRIMARY 0x1F0
#define ATA_SECONDARY 0x170
#define ATA_MASTER 0x00
#define ATA_SLAVE  0x10

// Input to controller
#define ATA_FEATURE(x) (x + 1)
#define ATA_COUNT(x)   (x + 2)
#define ATA_LBAL(x)    (x + 3)
#define ATA_LBAM(x)    (x + 4)
#define ATA_LBAH(x)    (x + 5)
#define ATA_DEVICE(x)  (x + 6)
#define ATA_COMMAND(x) (x + 7)
#define ATA_CONTROL(x) (x + 0x206)

// Output from controller
#define ATA_DATA(x) (x + 0)
#define ATA_ERROR(x)   (x + 1)
// COUNT
// LBAL
// LBAM
// LBAH
// DEVICE
#define ATA_STATUS(x)  (x + 7)

#define ATA_ERR (1<<0)
#define ATA_DRQ (1<<3)
#define ATA_SRV (1<<4)
#define ATA_DF  (1<<5)
#define ATA_RDY (1<<6)
#define ATA_BSY (1<<7)

#define ATA_nIEN (1<<1)
#define ATA_SRST (1<<2)

#define ATA_CMD_IDENTIFY 0xEC
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_WRITE_SECTORS 0x30
#define ATA_CMD_PACKET 0xA0

#define ATAPI_LBA_MAGIC 0xEB1401

typedef struct ata_drive
{
  int exists;
  int bus;
  int ms;
  int atapi;
  struct {
    uint16_t config;
    uint16_t unused1[9];
    char serial[20];
    uint16_t unused2[3];
    char firmware[8];
    char model[40];
    uint8_t unused3;
    uint8_t sectors_per_interrupt;
    uint16_t unused4;
    uint16_t capabilities[2];
    uint16_t unused5[2];
    uint16_t validity;
    uint16_t unused6[3];
    uint32_t capacity_sectors;
    uint16_t sectors_per_command;
    uint32_t capacity_lba28;
    uint16_t unused7[38];
    uint64_t capcity_lba48;
    uint16_t unused8[152];
  }__attribute__((packed)) id;
} ata_drive;

ata_drive drives[4];

typedef struct ata_cmd_st
{
  uint16_t bus;
  union {
    uint8_t feature;
    uint8_t error;
  };
  uint8_t count;
  union {
    uint64_t lba;
    uint8_t lba_sep[4];
  };
  uint8_t device;
  union {
    uint8_t command;
    uint8_t status;
  };
  uint8_t wait_status;
} ata_cmd_t;

int ata_read_block(struct ata_drive *drive, uint64_t lba, void *buffer);
int ata_write_block(struct ata_drive *drive, uint64_t lba, void *buffer);

void ata_init();
