#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h> // read(), write(), close()

const unsigned int STDIN_READ_BUFFER_SIZE = 128;
const unsigned int CMD_WORD_SIZE = 32;
const char QUIT_CMD_SEQ[] = {"__CMD@>QUIT"};

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

int main(int argc, char **arv) {
  unsigned char stdin_read_buffer[STDIN_READ_BUFFER_SIZE];
  while (true) {
    fread(stdin_read_buffer, STDIN_READ_BUFFER_SIZE, 1, stdin);
    // printf("after%s\n\n", stdin_read_buffer);
    fwrite(stdin_read_buffer, STDIN_READ_BUFFER_SIZE, 1, stdout);

    int res = cmd_cmp(QUIT_CMD_SEQ, (char *)stdin_read_buffer);
    printf("\n\n[DEBUG]:\tres %d\n\n", res);
    if (res) {
      return 123;
    }
  }
}
