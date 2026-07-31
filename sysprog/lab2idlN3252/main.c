#include <dirent.h>
#include <stdio.h>

#include "messages.h"
#include "plugins.h"
#include "codes.h"
#include "args.h"

int main(int argc, char ** argv)
{
    args_t args;

    char *plugdir_path;
    DIR *plugdir;

    plugin_t *plugins;
    int plugins_len;

    get_plugdir(argc, argv, &args);

    if (!args.valid_usage) {
        usage_message();
        return INVALID_USAGE_CODE;
    }
    
    plugdir_path = args.plugdir ? args.plugdir : ".";
    plugdir = opendir(plugdir_path);

    load_plugins(plugdir, plugdir_path, &plugins, &plugins_len);
    closedir(plugdir);
    unload_plugins(&plugins, plugins_len);

    return 0;
}
