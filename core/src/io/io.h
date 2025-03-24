void setup_lockfile();

void setup_indexfile();

void setup_datafile();

void dump_id_lock();

int get_incremental_id();

int write_new_record(const struct Blob *record);

void write_index(int head_pos, int id);

int get_record_pos_by_id(int id);

struct Blob *read_record(int record_pos);
