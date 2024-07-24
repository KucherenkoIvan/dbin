#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../frame/frame.h"
#include "../utils/utils.h"
#include "io.h"

// use global descriptors for R & W operations
// do not close/reopen them, just set to end of stream and fseek when needed
// when done synchronously should not cause any problems
FILE *LOCKFILE_READ_PTR = NULL;
FILE *LOCKFILE_WRITE_PTR = NULL;

FILE *DATA_READ_PTR = NULL;
FILE *DATA_WRITE_PTR = NULL;

FILE *IDX_READ_PTR = NULL;
FILE *IDX_WRITE_PTR = NULL;

const int LOCK_SIZE = 8;
unsigned long ID_LOCK = 0;

void setup_lockfile() {
  if (LOCKFILE_WRITE_PTR == NULL) {
    LOCKFILE_WRITE_PTR = fopen("lockfile", "ab");
  }

  bool lockfile_exists = file_exists("lockfile");

  if (!lockfile_exists) {
    fwrite("", 0, 1, LOCKFILE_WRITE_PTR);
  }

  if (LOCKFILE_READ_PTR == NULL) {
    LOCKFILE_READ_PTR = fopen("lockfile", "rb");
  }

  fseek(LOCKFILE_READ_PTR, 0, SEEK_END);
  int size = ftell(LOCKFILE_READ_PTR);

  if (size != 0) {
    unsigned char lock_buf[LOCK_SIZE];
    fread(lock_buf, LOCK_SIZE, 1, LOCKFILE_READ_PTR);

    ID_LOCK = 0;
    for (int i = 0; i < LOCK_SIZE; i++) {
      ID_LOCK += lock_buf[i] << (8 * (LOCK_SIZE - i - 1));
    }
  }
}

void dump_id_lock() {
  unsigned char buf[LOCK_SIZE];
  int mask = 0x00ff;

  for (int i = 0; i < LOCK_SIZE; i++) {
    buf[i] = (ID_LOCK >> (8 * (LOCK_SIZE - i - 1))) & mask;
  }

  fwrite(buf, LOCK_SIZE, 1, LOCKFILE_WRITE_PTR);
}

int get_incremental_id() {
  ID_LOCK++;
  dump_id_lock();
  return ID_LOCK;
}

int write_new_record(const Blob *data) {
  if (data == NULL) {
    return -1;
  }

  int flags = 0x00;

  Blob *encoded_frame = encode_frame(flags, data);

  if (encoded_frame == NULL) {
    return -1;
  }

  if (DATA_WRITE_PTR == NULL) {
    create_file_if_not_exists("data.dbin");
    DATA_WRITE_PTR = fopen("data.dbin", "ab");
  }

  int head_pos = ftell(DATA_WRITE_PTR);

  fwrite(encoded_frame->data, encoded_frame->size, 1, DATA_WRITE_PTR);

  return head_pos;
}

void write_index(int head_pos, int id) {
  if (id <= 0)
    id = get_incremental_id();

  int size = sizeof(int);
  unsigned char write_buffer[size * 2];

  int i = 0;

  // byte by byte write head_pos to first half of buffer
  for (; i < size; i++) {
    write_buffer[i] = get_nth_byte(head_pos, i);
  }

  // byte by byte write record id to second half of buffer
  for (; i < size * 2; i++) {
    write_buffer[i] = get_nth_byte(id, i);
  }

  create_file_if_not_exists("index.idx");

  if (IDX_WRITE_PTR == NULL) {
    IDX_WRITE_PTR = fopen("index.idx", "ab");
  }

  fwrite(write_buffer, size * 2, 1, IDX_WRITE_PTR);
}
