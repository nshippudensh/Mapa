#include <stdio.h>
#include <kv.h>

int main() {
    kv_t *db = kv_init(16);

    printf("%p\n", db);
    printf("%lld\n", db->capacity);

    kv_put(db, "foo", "far");
    kv_put(db, "boo", "bar");
    kv_put(db, "doo", "dar");
    kv_put(db, "doo", "dsfhakljsdhfkljadsh");

    for(int i = 0; i < db->capacity; i++) {
        if(db->entries[i].key) {
            printf("[%d]: %s %s\n",i, db->entries[i].key, db->entries[i].value);
        }
    }

    char *val1 = kv_get(db, "foo");
    char *val2 = kv_get(db, "doo");
    char *val3 = kv_get(db, "fooo");

    printf("val1 = %s\n", val1);
    printf("val2 = %s\n", val2);
    printf("val3 = %s\n", val3);

    kv_delete(db, "foo");
    val1 = NULL;
    val1 = kv_get(db, "foo");

    printf("val1 = %s\n", val1);
    printf("val2 = %s\n", val2);
    printf("val3 = %s\n", val3);

    kv_free(db);
    db = NULL;

}