void setup_lockfile();

void dump_id_lock();

int get_incremental_id();

int write_new_record(const struct Blob *record);

void write_index(int head_pos, int id);
