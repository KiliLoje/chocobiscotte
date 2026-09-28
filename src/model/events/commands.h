#include "../../../include/common.h"

typedef struct {
  u8 opcode;
  u8 argc;
  u8 *argv;
  u32 address;
} ff6_command_t;

typedef ff6_command_t ff6_action_command_t;

typedef struct {
  u8 opcode;
  u8 size; /* in bytes */
  u8 argc;
  ff6_action_command_t *argv;
  u32 address;
} ff6_action_queue_t;

typedef struct {
  ff6_command_t *commands;
  u32 size;
  u16 address;
} ff6_command_subroutine_t;

typedef struct {
  u8 *buf;
  u32 len;
  u32 index;
} ff6_command_parser_t;

ff6_command_t ff6_parse_command(void *parser_ctx);
ff6_action_queue_t ff6_parse_action_queue(void *parser_ctx);
ff6_action_command_t ff6_parse_action_command(void *parser_ctx);
ff6_command_subroutine_t ff6_parse_subroutine(void *parser_ctx);
