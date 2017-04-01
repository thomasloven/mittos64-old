#include <ata.h>
#include <ports.h>
#include <debug.h>

ata_drive drives[4] = {
  {.bus=ATA_PRIMARY, .ms=ATA_MASTER},
  {.bus=ATA_PRIMARY, .ms=ATA_SLAVE},
  {.bus=ATA_SECONDARY, .ms=ATA_MASTER},
  {.bus=ATA_SECONDARY, .ms=ATA_SLAVE}
};


int ata_wait_status(int bus)
{
  inb(ATA_STATUS(bus));
  inb(ATA_STATUS(bus));
  inb(ATA_STATUS(bus));
  inb(ATA_STATUS(bus));
  return inb(ATA_STATUS(bus));
}

int ata_send_command(ata_cmd_t *cmd)
{
  outb(ATA_DEVICE(cmd->bus), cmd->device | (cmd->lba_sep[3] & 0xF));
  ata_wait_status(cmd->bus);

  outb(ATA_COUNT(cmd->bus), cmd->count);
  outb(ATA_LBAL(cmd->bus), cmd->lba_sep[0]);
  outb(ATA_LBAM(cmd->bus), cmd->lba_sep[1]);
  outb(ATA_LBAH(cmd->bus), cmd->lba_sep[2]);
  while(inb(ATA_STATUS(cmd->bus)) & ATA_BSY);
  outb(ATA_COMMAND(cmd->bus), cmd->command);

  if(!inb(ATA_STATUS(cmd->bus)))
    return 0;

  cmd->status = ata_wait_status(cmd->bus);
  while((cmd->status = inb(ATA_STATUS(cmd->bus))) & ATA_BSY);
  if(cmd->wait_status) while(!(cmd->status = inb(ATA_STATUS(cmd->bus)) & (cmd->wait_status | ATA_ERR)));
  cmd->error = inb(ATA_ERROR(cmd->bus));
  cmd->count = inb(ATA_COUNT(cmd->bus));
  cmd->device = inb(ATA_DEVICE(cmd->bus));
  cmd->lba_sep[0] = inb(ATA_LBAL(cmd->bus));
  cmd->lba_sep[1] = inb(ATA_LBAM(cmd->bus));
  cmd->lba_sep[2] = inb(ATA_LBAH(cmd->bus));
  cmd->lba_sep[3] = 0;

  return cmd->status;
}

void init_drive(ata_drive *drive)
{
  int bus = drive->bus;
  // Check if the controller exists
  // by writing a value to it and check
  // that the same one is returned
  int v1 = inb(ATA_LBAL(bus));
  outb(ATA_LBAL(bus), (~v1)&0xFF);
  int v2 = inb(ATA_LBAL(bus));
  if(v2 != ((~v1)&0xFF))
    return;

  // Check if the drive exists
  // by selecting the drive
  outb(ATA_DEVICE(bus), 0xA0 | drive->ms);
  if(!(ata_wait_status(bus) & ATA_RDY))
    return;

  outb(ATA_CONTROL(bus), ATA_SRST);
  outb(ATA_CONTROL(bus), 0);
  uint64_t lba = inb(ATA_LBAH(bus))<<16 | inb(ATA_LBAM(bus)) << 8 | inb(ATA_LBAL(bus));

  ata_cmd_t command = {
    .bus = bus,
    .device=0xA0 | drive->ms,
    .command = ATA_CMD_IDENTIFY,
    .wait_status = ATA_DRQ,
  };
  if(lba == ATAPI_LBA_MAGIC)
  {
    drive->atapi = 1;
    command.command = ATA_CMD_IDENTIFY_PACKET;
  }
  if(!ata_send_command(&command))
    return;

  // Read IDENTIFY information
  uint16_t *buf = (void *)&drive->id;
  for(int i=0; i<256; i++)
  {
    buf[i] = inw(ATA_DATA(drive->bus));
  }

  drive->exists = 1;

}

int ata_read_block(ata_drive *drive, uint64_t lba, void *buffer)
{
  int retries = 5;
  while(retries)
  {
    ata_cmd_t command = {
      .bus = drive->bus,
      .count = 1,
      .lba = lba & 0xFFFFFF,
      .device = 0xE0 | drive->ms | ((lba >> 24) & 0xF),
      .command = ATA_CMD_READ_SECTORS,
    };

    int status = ata_send_command(&command);
    if(status & (ATA_DF | ATA_ERR) || !(status & ATA_DRQ))
    {
      retries--;
      continue;
    }

    uint16_t *buf = buffer;
    for(int i=0; i<256; i++)
    {
      buf[i] = inw(ATA_DATA(drive->bus));
    }
    return 0;
  }
  debug_error("Reading disk failed\n");
  return -1;
}

int ata_write_block(ata_drive *drive, uint64_t lba, void *buffer)
{
  ata_cmd_t command = {
    .bus = drive->bus,
    .count = 1,
    .lba = lba & 0xFFFFFF,
    .device = 0xE0 | drive->ms | ((lba >> 24) & 0xF),
    .command = ATA_CMD_WRITE_SECTORS,
  };
  int status = ata_send_command(&command);
  if(status & ATA_ERR)
  {
    debug_error("An error occurred\n");
  }
  uint16_t *buf = buffer;
  for(int i=0; i<256; i++)
  {
    outw(ATA_DATA(drive->bus), buf[i]);
  }

  return 0;
}

void ata_init()
{
  init_drive(&drives[0]);
  init_drive(&drives[1]);
  init_drive(&drives[2]);
  init_drive(&drives[3]);
}
