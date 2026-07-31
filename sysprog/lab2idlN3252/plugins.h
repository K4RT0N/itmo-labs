#ifndef _PLUGINS_H
#define _PLUGINS_H

#include <dirent.h>
#include <getopt.h>

#include "plugin_api.h"

typedef struct {
    char *name;

    void *handle;
    int (*plugin_get_info)(struct plugin_info* ppi);
    int (*plugin_process_file)(const char *fname,
            struct option in_opts[],
            size_t in_opts_len);

    struct option *longopts;
    int longopts_len;
} plugin_t;

void load_plugins(DIR *plugdir, char *plugdir_path,
        plugin_t **plugins, int *plugins_len);
void unload_plugins(plugin_t **plugins, int plugins_len);
void get_longopts(struct option **longopts, int *longopts_len);

#endif
