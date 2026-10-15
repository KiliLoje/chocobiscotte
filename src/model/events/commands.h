#include "../../../include/common.h"

typedef struct ff6_command_t {
  u8 opcode;
  u8 argc;

  union {
    struct ff6_command_t *actions;
    u8 *argv;
  };

  u32 address;
} ff6_command_t;
typedef ff6_command_t ff6_action_command_t;

typedef struct {
  ff6_command_t *commands;
  u32 size;
  u32 address;
} ff6_command_subroutine_t;

typedef struct {
  u8 *buf;
  u32 len;
  u32 index;
} ff6_command_parser_t;

ff6_command_t ff6_parse_command(void *parser_ctx);
u8 ff6_parse_action_queue(void *dest, void *parser_ctx, u8 expected_size);
ff6_action_command_t ff6_parse_action_command(void *parser_ctx);
ff6_command_subroutine_t ff6_parse_subroutine(void *parser_ctx);
