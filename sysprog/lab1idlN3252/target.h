#ifndef TARGET_H
#define TARGET_H

#include <stdbool.h>

typedef struct target_t {
    unsigned char *byte_array;
    int length;
} target_t;

bool init_target(target_t *target_struct, char *target_str);
void delete_target(target_t *target);

unsigned char *get_byte_array(target_t *target);
int get_length(target_t *target);

#endif
