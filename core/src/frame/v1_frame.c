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

  Blob *frame = malloc(sizeof(Blob));
  frame->size = frame_size;

  unsigned char *frame_bytes = malloc(frame_size);
  frame->data = frame_bytes;

  memcpy(frame_bytes, header, V1_HEADER_SIZE);
  memcpy(&frame_bytes[V1_HEADER_SIZE], data_ptr->data, data_ptr->size);

  free(header);

  return frame;
}

Blob *decode_frame(const Blob *encoded_frame) {
  if (encoded_frame == NULL) {
    return NULL;
  }

  const V1_Header *header = decode_v1_header(encoded_frame->data);

  if (header == NULL) {
    return NULL;
  }

  Blob *frame = malloc(sizeof(Blob));

  unsigned char *frame_buffer = malloc(header->data_size);
  frame->data = frame_buffer;
  frame->size = header->data_size;

  memset(frame_buffer, 0, frame->size);

  memcpy(frame_buffer, &encoded_frame[V1_HEADER_SIZE],
         frame->size - V1_HEADER_SIZE);

  return frame;
}
