#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int (*hash_function)(void *key, int len);
    int (*key_compare)(void *key1, void *key2);
} hash_table_s;

hash_table_s *create_table(int size, int (*hash_function)(void*, int), int (*key_compare)(void*, void*)) {
    hash_table_s *table = malloc(sizeof(hash_table_s));
    table->size = size;
    table->count = 0;
    table->buckets = calloc(table->size, sizeof(entry_s*));
    table->hash_function = hash_function;
    table->key_compare = key_compare;
    return table;
}

void resize_table(hash_table_s *table) {
    int new_size = table->size * 2;
    entry_s ** new_buckets = (entry_s **)calloc(new_size, sizeof(entry_s*));
    int new_bucket_index;
    entry_s * entry;
    entry_s * next_entry;
    int i;
    
    for (i = 0; i < table->size; i++) {
        entry = table->buckets[i];
        while (entry != NULL) {
            new_bucket_index = table->hash_function(entry->key, strlen(entry->key)) % new_size;
            next_entry = entry->next;
            entry->next = new_buckets[new_bucket_index];
            new_buckets[new_bucket_index] = entry;
            entry = next_entry;
        }
    }

    free(table->buckets);
    table->buckets = new_buckets;
    table->size = new_size;
}

void insert(hash_table_s *table, void *key, void *value) {
    // Resize logic here > 0.75
    if (table->count > table->size * 0.75) {
	printf("Resizing\n");
        resize_table(table);
    }


   
    int hash = table->hash_function(key, strlen(key)) % table->size;
    if (hash < 0)
	hash = -hash;

    int bucket_index = hash;//table->hash_function(key) % table->size;
    entry_s *entry = table->buckets[bucket_index];
    entry_s *new_entry;
    
    while (entry != NULL) {
        if (table->key_compare(entry->key, key) == 0) {
            // Key exists, update value
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    // Insert new entry
    new_entry = malloc(sizeof(entry));
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = table->buckets[bucket_index];
    table->buckets[bucket_index] = new_entry;
    table->count++;
}

void *search(hash_table_s *table, void *key) {
    
    int hash = table->hash_function(key, strlen(key)) % table->size;
    if (hash < 0)
	hash = -hash;
		  
    //printf("Hash: %d\n", hash);
    int bucket_index = hash;//table->hash_function(key) % table->size;
    entry_s *entry = table->buckets[bucket_index];

    while (entry != NULL) {
        if (table->key_compare(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    // Not found
    return NULL;
}



int string_hash_function(void *key, int len) {
    char *str = (char *)key;
    unsigned long hash = 5612;
    int c;
    int count = 0;

    while ((count<len))
    {
	c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	count++;
    }

    if (hash < 0)
	hash = -hash;
		  
    
    return hash;
}

int string_key_compare(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2);
}


int mem_key_compare(void *key1, void *key2) {
    return memcmp(((mem_s *)key1)->value, ((mem_s *)key1)->value, ((mem_s *)key1)->len);
}



// Add more functions as needed, e.g., delete, destroy_table, etc.

/* int main() { */

/*     hash_table *table = create_table(10, string_hash_function, string_key_compare); */

    

/*     return 0; */
/* } */
