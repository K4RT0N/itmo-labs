#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "target.h"
#include "codes.h"
#include "debug.h"

static bool validate_target(char *target_str);

/*
 * Parses target_string and saves it into target_struct
 */
bool init_target(target_t *target_struct, char *target_str)
{
    unsigned char *byte_array;
    unsigned char byte_value;
    char *endptr;
    char byte[3];
    int length;

    if (debug()) {
        fprintf(stderr, "init_target function started.\n");
    }

    if (!validate_target(target_str)) {
        return false;
    }
    
    length = strlen(target_str) / 2 - 1;
    byte_array = malloc(sizeof(char) * length);
    if (byte_array == NULL) {
        perror("malloc");
        exit(allocation_error_code);
    }
    
    byte[2] = '\0';
    for (int i = 0; i < length; i++) {
        byte[0] = target_str[i*2 + 2];
        byte[1] = target_str[i*2 + 3];
        byte_value = (unsigned char) strtol(byte, &endptr, 16);
        if (*endptr != '\0') {
            free(byte_array);
            if (debug()) {
                fprintf(stderr, "init_target ended with error.\n");
            }
            return false;
        }
        byte_array[i] = byte_value;
    }

    target_struct->byte_array = byte_array;
    target_struct->length = length;

    if (debug()) {
        fprintf(stderr, "init_target ended succesfully.\n");
    }

    return true;
}

void delete_target(target_t *target)
{
    free(target->byte_array);
}

/*
 * Validates target.
 */
static bool validate_target(char *target_str)
{
    int target_length;

    target_length = strlen(target_str);

    if (target_length % 2 || target_length <= 2 ||
        target_str[0] != '0' || target_str[1] != 'x') {
        return false;
    }

    for (int i = 2; i < (int) strlen(target_str); i++) {
        if (!isxdigit(target_str[i])) {
            return false;
        }
    }

    return true;
}

unsigned char *get_byte_array(target_t *target)
{
    return target->byte_array;
}

int get_length(target_t *target)
{
    return target->length;
}
