#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h> // read(), write(), close()

#define STDIN_READ_BUFFER_SIZE 128
#define CMD_WORD_SIZE 32

static const unsigned char CMD_PREFIX[] = "__CMD@>";

unsigned char stdin_read_buffer[STDIN_READ_BUFFER_SIZE] = {};
char cmd_read_buffer[CMD_WORD_SIZE];

enum CMD {
  CMD_UNKNOWN = 0,
  CMD_QUIT = 1,
  CMD_ADD = 2,
};

int cmd_cmp(const char *a, const char *b) {
  int l_a = strnlen(a, CMD_WORD_SIZE);
  int l_b = strnlen(b, CMD_WORD_SIZE);
  int len = l_a < l_b ? l_a : l_b;

  for (int i = 0; i < len; i++) {
    if (a[i] != b[i]) {
      return 0;
    }
  }
  return 1;
}

void _build_cmp_buff(char *inp, const char *cmd) {
  strncpy(inp, "", CMD_WORD_SIZE);
  strncpy(inp, (char *)CMD_PREFIX, CMD_WORD_SIZE);
  strncat(inp, cmd, CMD_WORD_SIZE);
}

enum CMD get_cmd(const char *cmd_buff) {
  char cmp_buff[32];

  _build_cmp_buff(cmp_buff, "QUIT");
  if (cmd_cmp(cmd_buff, cmp_buff)) {
    return CMD_QUIT;
  }

  _build_cmp_buff(cmp_buff, "ADD");
  if (cmd_cmp(cmd_buff, cmp_buff)) {
    return CMD_ADD;
  }

  return CMD_UNKNOWN;
}

void clean_input_buffers() {
  strncpy(cmd_read_buffer, "", CMD_WORD_SIZE);
  strncpy((char *)stdin_read_buffer, "", STDIN_READ_BUFFER_SIZE);
}

void add_data() {
  fread(stdin_read_buffer, STDIN_READ_BUFFER_SIZE, 1, stdin);
  fwrite(stdin_read_buffer, STDIN_READ_BUFFER_SIZE, 1, stdout);
}

int main(int argc, char **arv) {
  while (true) {
    clean_input_buffers();

    fread(cmd_read_buffer, CMD_WORD_SIZE, 1, stdin);

    enum CMD input_cmd = get_cmd((char *)cmd_read_buffer);

    switch (input_cmd) {
    case CMD_QUIT:
      return (unsigned short int)0;

    case CMD_ADD:
      add_data();
      break;

    case CMD_UNKNOWN:
    default:
      return (unsigned short int)1;
    }
  }
}
