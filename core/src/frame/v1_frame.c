#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../header/header.h"
#include "../utils/utils.h"
#include "./frame.h"

Blob *encode_frame(const int flags, const Blob *data_ptr) {
  if (data_ptr == NULL) {
    return NULL;
  }
  const V1_Header header_args = {.flags = flags, .data_size = data_ptr->size};
  unsigned char *header = encode_v1_header(&header_args);

  if (header == NULL) {
    return NULL;
  }

  int frame_size = V1_HEADER_SIZE + data_ptr->size;

  Blob *frame = malloc(sizeof(struct Blob));
  frame->size = frame_size;

  unsigned char *frame_bytes = malloc(frame_size);

  memcpy(frame_bytes, header, V1_HEADER_SIZE);
  memcpy(&frame_bytes[V1_HEADER_SIZE], data_ptr->data, data_ptr->size);

  free(header);
  free(frame_bytes);

  return frame;
}
