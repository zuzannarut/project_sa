typedef struct entry_s {
    void *key;
    void *value;
    struct entry_s *next;
} entry_s;

typedef struct mem_s {
    void * value;
    int len;
} mem_s;

typedef struct hash_table_s {
    entry_s **buckets;
    int size;
    int count;
    int (*hash_function)(void *key);
    int (*key_compare)(void *key1, void *key2);
} hash_table_s;

hash_table_s *create_table(int size, int (*hash_function)(void*, int), int (*key_compare)(void*, void*));

void *search(hash_table_s *table, void *key);

void insert(hash_table_s *table, void *key, void *value);

int string_hash_function(void *key, int len);

int string_key_compare(void *key1, void *key2);

int mem_key_compare(void *key1, void *key2);
