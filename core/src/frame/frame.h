const int FRAME_BUFFER_SIZE = 0xffff;

struct Blob *encode_frame(const int flags, const struct Blob *data_ptr);

struct Blob *decode_frame(const struct Blob *encoded_frame);
