#include <stdlib.h>
#include <stdio.h>
#include "commands.h"

static const u8 commands_argument_counts[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, /* 0x00-0x35: action commands: variable argc and not parsed as commands */
  1, 1, 2, 0, 0, 0, 0, 4, 1, 1, 2, 2, 1, 1, 2, 2,
  0, 1, 0, 2, 0,
  0, /* 0x4A: unused */
  2, 2, 2, 0, 0, 1, 3, 1, 3, 0, 1, 1, 1, 1, 1, 1,
  0, /* 0x5B: unused */
  0, 2, 2, 2, 2, 3, 1, 1,
  0, 0, 0, 0, 0, 0, /* 0x64-0x69: unused */
  5, 5, 5,
  0, 0, 0, /* 0x6D-0x6F: unused */
  2, 2, 2,
  0, 0, /* 0x73-0x74: variable argc */
  0,
  0, /* 0x76: unused */
  1, 1, 2, 4, 0, 1,
  0, /* 0x7D: unused */
  2, 2, 1,
  0, /* 0x81: unused */
  0,
  0, /* 0x83: unused */
  2, 2, 1, 1, 3, 3, 3, 2, 2, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 3, 0, 1, 1, 0,
  0, 0, /* 0x9E-0x9F: unused */
  5, 1,
  0, 0, 0, 0, /* 0xA2-0xA5: unused */
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 4, 1, 1,
  0, /* 0xB6: variable argc */
  4, 1, 1, 1, 1, 2, 3,
  0, /* 0xBE: variable argc */
  0, 5, 7, 9, 11, 13, 15, 17, 19, 5, 7, 9, 11, 13, 15, 17,
  19, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, /* 0xDF: unused */
  0, 0, 0, 0, 0,
  0, 0, 0, /* 0xE5-0xE7: unused */
  3, 3, 3, 3,
  0, 0, 0, /* 0xEC-0xEE: unused */
  2, 1, 2, 1, 1, 1, 3, 3, 0,
  0, /* 0xF8: unusued */
  1, 0, 0,
  0, /* 0xFC: unused */
  0, 0, 0
};

static u8 getitem(ff6_command_parser_t *ctx)
{
  if (ctx && ctx->buf && ctx->index < ctx->len) return ctx->buf[ctx->index++];
  else {
    if (!ctx || !ctx->buf) {
      printf("NULL parsing context\n");
    }
    if (ctx->index >= ctx->len) {
      printf("index %d out of bound\n", ctx->index);
    }
    exit(1);
  }
}

static u8 checkitem(ff6_command_parser_t *ctx, u16 step)
{
  if (ctx && ctx->buf && ctx->index + step < ctx->len) return ctx->buf[ctx->index + step];
  else {
    if (!ctx || !ctx->buf) {
      printf("NULL parsing context\n");
    }
    if (ctx->index + step >= ctx->len) {
      printf("index %d out of bound\n", ctx->index);
    }
    exit(1);
  }
}

/* unchecked getitem, do not use unless the check has already been passed */
#define _getitem(ctx) (ctx)->buf[(ctx)->index++]

static u8 *getitems(ff6_command_parser_t *ctx, u16 size)
{
  if (ctx->index + size >= ctx->len) return NULL;
  if (ctx && ctx->buf && ctx->index < ctx->len) {

    u8 *output = malloc(sizeof(u8) * size);
    if (!output) return NULL;

    for(int i = 0; i < size; i++) output[i] = _getitem(ctx);
    return output;

  } else {
    if (!ctx || !ctx->buf) {
      printf("NULL parsing context\n");
    }
    if (ctx->index >= ctx->len) {
      printf("index %d out of bound\n", ctx->index);
    }
    exit(1);
  }
}

static u8 step(ff6_command_parser_t *ctx, u16 step)
{
  if (ctx->index + step >= ctx->len) {
    printf("index %d out of bound\n", ctx->index + step);
    return 1;
  }
  else {
    ctx->index += step;
    return 0;
  }
}

ff6_command_t ff6_parse_command(void *parser_ctx)
{
  ff6_command_parser_t *ctx = (ff6_command_parser_t *)parser_ctx;

  ff6_command_t command = {
    .address = ctx->index,
    .opcode = getitem(ctx)
  };

  if (command.opcode < 0x35) {
    u8 size = getitem(ctx);
    command.argc = ff6_parse_action_queue(command.actions, ctx, size);
  } else {
    switch (command.opcode) {
      case 0x73:
      case 0x74:
        /* number of argument is determined by the size of the chunk
         * that is specified by the third and fourth arguments */
        command.argc = 2 + checkitem(ctx, 3) * checkitem(ctx, 4);
        break;
      case 0xB6:
        /* TODO: implement this
         * this is temporary */
        command.argc = 6;
        break;
      case 0xBE:
        /* first arg represents the number of 12-bit addresses that follows it */
        command.argc = 1 + 3 * checkitem(ctx, 1);
      default:
        command.argc = commands_argument_counts[command.opcode];
        break;
    }
    command.argv = getitems(ctx, command.argc);
  }
  return command;
}

/* returns the number of actions parsed, and put the actual list of parsed commands into the dest parameter */
u8 ff6_parse_action_queue(void *dest, void *parser_ctx, u8 expected_size)
{
  ff6_command_parser_t *ctx = (ff6_command_parser_t *)parser_ctx;

  ff6_action_command_t *actions = calloc(0x7F, sizeof(ff6_action_command_t));

  if (!actions) {
    printf("failed to calloc\n");
    exit(1);
  }

  u8 count, size = 0;
  do {
    actions[count] = ff6_parse_action_command(ctx);
    size += 1 + actions[count].argc;
    count++;
  } while(size <= expected_size && actions[count].opcode != 0xFF);
  actions = realloc(actions, sizeof(ff6_action_command_t) * count);
  if (!actions) {
    printf("failed to realloc\n");
    exit(1);
  }

  return count;
}

ff6_action_command_t ff6_parse_action_command(void *parser_ctx)
{
  ff6_command_parser_t *ctx = (ff6_command_parser_t *)parser_ctx;

  ff6_action_command_t command = {
    .address = ctx->index,
    .opcode = getitem(ctx),
  };

  switch (command.opcode) {
    case 0xC8:
    case 0xC9:
    case 0xE0:
    case 0xE1:
    case 0xE2:
    case 0xE3:
    case 0xE4:
    case 0xE5:
    case 0xE6:
    case 0xF1:
    case 0xFB:
    case 0xFC:
    case 0xFD:
      command.argc = 1;
      break;
    case 0xD5:
      command.argc = 2;
      break;
    case 0xF9:
      command.argc = 3;
      break;
    default:
      command.argc = 0;
      break;
  }
  command.argv = getitems(ctx, command.argc);
  return command;
}
