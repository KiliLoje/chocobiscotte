#include "../include/common.h"
#include "model/events/commands.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *rom_path = "ffvi.sfc";

  FILE *romptr = fopen(rom_path, "r");
  if (!romptr) {
    printf("couldn't find file\n");
    return 1;
  }
  fseek(romptr, 0, SEEK_END);
  u32 buf_size = ftell(romptr);
  fseek(romptr, 0, SEEK_SET);

  u8 *buf = (u8*)calloc(buf_size, sizeof(u8));
  if (buf) fread(buf, sizeof(u8), buf_size, romptr);
  else return 1;

  ff6_command_parser_t parser = {
    .buf = buf,
    .index = 0xA2381,
    .len = buf_size,
  };

  u32 count = 30;
  for (int i = 0; i < count; i ++) {
    ff6_command_t cmd = ff6_parse_command(&parser);
    printf("%x: %02x  ", cmd.address, cmd.opcode);
    if (cmd.opcode <= 0x35) {
      for (int i = 0; i < cmd.argc; i++) {
        printf("\n");
        printf("    %x: %02x  ", cmd.actions[i].address, cmd.actions[i].opcode);
        for (int j = 0; j < cmd.actions[i].argc; j++) {
          printf("%02x ", cmd.actions[i].argv[j]);
        }
      }
    } else {
      for (int i = 0; i < cmd.argc; i++)
        printf("%02x ", cmd.argv[i]);
    }
    printf("\n");
  }

  return EXIT_SUCCESS;
}
