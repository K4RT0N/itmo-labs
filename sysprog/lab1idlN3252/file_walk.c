#include <string.h>
#include <stdio.h>
#include <ftw.h>

#include "file_walk.h"
#include "debug.h"

static int fn(const char *fpath, const struct stat *sb, int);

static unsigned char *byte_array;
static int length;

/*
 * Walks file tree starting from char *catalogue and looks for target
 */
bool file_walk(char *catalogue, target_t *target)
{
    if (debug()) {
        fprintf(stderr, "file_walk function started.\n");
    }
    int result;

    byte_array = target->byte_array;
    length = target->length;
    result = ftw(catalogue, fn, 64);


    if (result == 0) {
        if (debug()) {
            fprintf(stderr, "file_walk function ended succesfully.\n");
        }
        return true;
    }
    if (debug()) {
        fprintf(stderr, "file_walk function ended with error.\n");
    }
    return false;
}

/*
 * Scans file for target.
 */
static int fn(const char *fpath, const struct stat *sb, int)
{
    FILE *f;
    int ch;
    int i;
    
    if ((sb->st_mode & S_IFMT) != S_IFREG) {
        return 0;
    }
    
    f = fopen(fpath, "r");
    if (f == NULL) {
        perror(fpath);
        return 0;
    }
    
    i = 0;
    while ((ch = getc(f)) != EOF) {
        if (ch == byte_array[i]) {
            i++;
            if (i == length) {
                printf("%s\n", fpath);
                fclose(f);
                return 0;
            }
        } else {
            i = 0;
        }
    }

    fclose(f);

    return 0;
}
