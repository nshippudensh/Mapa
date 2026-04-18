#include <kv.h>
#include <string.h>


kv_t *kv_init(size_t capacity) {
    if(capacity == 0) 
        return NULL;

    kv_t *table = malloc(sizeof(kv_t));

    if(table == NULL)
        return NULL;

    table->capacity = capacity;
    table->count = 0;

    table->entries = calloc(sizeof(kv_entry_t), capacity);
    if(table->entries == NULL)
        return NULL;

    return table;
}

size_t hash(char *val, int capacity) {
    size_t hash = 0x13371337deadbeef;

    while(*val) {
        hash ^= *val;
        hash <<= 8;
        hash += *val;

        val++;
    }

    return hash % capacity;
}

/*  fn kv_delete
    params:
        -table: a pointer to the db
        -key: a pointer to the key value
        returns the index of the deletion
        -1 is no found
*/
int kv_delete(kv_t *table, char *key) {
    if(!table || !key)
        return -1;

    size_t idx = hash(key, table->capacity);

    for (int i = 0; i < table->capacity - 1; i++) {
        size_t real_index = (idx + i) % table->capacity;
        kv_entry_t *entry = &table->entries[real_index];

        if(entry->key == NULL) 
            return -1;
        
        if(entry->key && entry->key != (void *)TOMBSTONE && !strcmp(entry->key, key)) {
            free(entry->key);
            free(entry->value);
            table->count--;
            entry->key = (void *)TOMBSTONE;
            entry->value = NULL;

            return real_index;
        }
    }
    return -1;
}

/*  fn kv_get
    params:
        -table: a pointer to the db
        -key: a pointer to the key value
        returns the pointer to the key
        NULL is no found
*/
char *kv_get(kv_t *table, char *key) {
    if(!table || !key)
        return NULL;

    size_t idx = hash(key, table->capacity);

    for (int i = 0; i < table->capacity - 1; i++) {
        size_t real_index = (idx + i) % table->capacity;
        kv_entry_t *entry = &table->entries[real_index];

        if(entry->key == NULL) 
            return NULL;

        if(entry->key && entry->key != (void *)TOMBSTONE &&!strcmp(entry->key, key))
            return entry->value;
    }
    return NULL;
}

/*  fn kv_put
    params:
        -table: a pointer to the db
        -key: a pointer to the key value
        -value: a pointer of the value itself
        returns the index of the key, otherwise on
        error, returns -1, on not found return -2
*/
int kv_put(kv_t *table, char *key, char *value) {

    if(!table || !key || !value)
        return -1;
    
    size_t idx = hash(key, table->capacity);

    for (int i = 0; i < table->capacity - 1; i++) {
        size_t real_index = (idx + i) % table->capacity;
        kv_entry_t *entry = &table->entries[real_index];

        // the key is already set, updating
        if(entry->key && entry->key != (void *)TOMBSTONE && !strcmp(entry->key, key)) {
            char *newval = strdup(value);
            if(!newval) {
                //free(newval);   //sdfsdfsdfsd
                return -1;
            }    
            free(entry->value);
            entry->value = newval;
            return real_index;
        }

        // land in a slot that is "empty"
        // null or TOMBSTONE
        if(!entry->key || entry->key == (void *)TOMBSTONE) {
            char *newval = strdup(value);
            char *newkey = strdup(key);

            if(!newval || !newkey) {
                free(newval);
                free(newkey);
                return -1;
            }
            entry->key = newkey;
            entry->value = newval;
            table->count++;

            return real_index;
        }

    }

    // the db is occupied
    return -2;
}

/*  fn kv_free
    params:
        -table: a pointer to the db
        returns: 0 on success, -1 on failure
        
*/
int kv_free(kv_t *table) {
    if(!table)
        return -1;

    for (int i = 0; i < table->capacity - 1; i++) {
        
        kv_entry_t *entry = &table->entries[i];

        if(entry->key && entry->key != (void *)TOMBSTONE) {
            free(entry->key);
            free(entry->value);
            entry->key = NULL;
            entry->value = NULL;
            table->count--;
        }
    }

    free(table->entries);
    free(table);
    return 0;
}
