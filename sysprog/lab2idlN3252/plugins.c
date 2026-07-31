#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "plugin_api.h"
#include "plugins.h"
#include "debug.h"

static char *get_filepath(char *dirpath, char *filename);
static char *get_plugname(char *filename);
static bool is_plugname(char *filename);

/* Loads plugins from plugdir and saves their data in plugins variable*/
void load_plugins(DIR *plugdir, char *plugdir_path,
        plugin_t **plugins, int *plugins_len)
{
    void *handle;
    char *plugname;
    int (*plugin_get_info)(struct plugin_info* ppi);
    int (*plugin_process_file)(const char *fname,
            struct option in_opts[],
            size_t in_opts_len);

    struct dirent *dirent;
    char *filename;
    char *filepath;

    plugin_t *plugins_array;
    int plugins_counter;

    void *ptr;

    debug("Plugin loading started\n");

    /* Reading the content of directory */
    plugins_array = NULL;
    plugins_counter = 0;
    while ((dirent = readdir(plugdir)) != NULL) {
        filename = dirent->d_name;
        if (!is_plugname(filename)) {
            /* The name is not of a plugin */
            continue;
        }
        debug("%s was identified as a plugin name\n", filename);
        /* Getting filepath */
        filepath = get_filepath(plugdir_path, filename);
        if (!filepath) {
            /* An error occured while allocating memory for filepath */
            debug("An error occured while trying to get a path to %s\n",
                  filename);
            continue;
        }
        /* Loading plugin */
        handle = dlopen(filepath, RTLD_NOW);
        free(filepath);
        if (!handle) {
            /* An error occured whlie loading plugin */
            debug("An error occured while trying to load %s as a plugin\n",
                  filename);
            continue;
        }
        /* Getting plugin name */
        plugname = get_plugname(filename);
        if (!plugname) {
            /* An error occurew while allocating memory for plugin name */
            debug("An error occured while trying to get a plugin name\n");
            dlclose(handle);
            continue;
        }
        /* Loading function from plugin */
        plugin_get_info = dlsym(handle, "plugin_get_info");
        if (!plugin_get_info) {
            /* An error occured while trying to load a function from plugin*/
            debug("Coulgn't load plugin_get_info from %s\n", plugname);
            dlclose(handle);
            free(plugname);
            continue;
        }
        /* Loading function from plugin */
        plugin_process_file = dlsym(handle, "plugin_process_file");
        if (!plugin_process_file) {
            /* An error occured while trying to load a function from plugin*/
            debug("Coulgn't load plugin_process_file from %s\n", plugname);
            dlclose(handle);
            free(plugname);
            continue;
        }
        /* Saving loading data */
        ptr = realloc(plugins_array, sizeof(plugin_t) * (plugins_counter + 1));
        if (!ptr) {
            /* An error occured whlie trying to reallocate memory for plugins */
            debug("An error occured while trying to allocate "
                  "new plugin\n");
            dlclose(handle);
            free(plugname);
            continue;
        }
        debug("%s plugin was successfully loaded\n", plugname);
        plugins_array = ptr;
        plugins_array[plugins_counter].name = plugname;
        plugins_array[plugins_counter].handle = handle;
        plugins_array[plugins_counter].plugin_get_info = plugin_get_info;
        plugins_array[plugins_counter].plugin_process_file =
            plugin_process_file;
        plugins_counter++;
    }

    *plugins = plugins_array;
    *plugins_len = plugins_counter;

    debug("Plugin loading ended\n");
}

/* This function unloads plugins */
void unload_plugins(plugin_t **plugins, int plugins_len)
{
    plugin_t *plugins_array;

    debug("Plugin unloading started\n");

    plugins_array = *plugins;

    for (int i = 0; i < plugins_len; i++) {
        dlclose(plugins_array[i].handle);
        free(plugins_array[i].name);
    }

    free(*plugins);
    *plugins = NULL;

    debug("Plugin unloading ended\n");
}

/* This function saves long options for plugisn */
void get_longopts(struct option **longopts, int *longopts_len)
{
    struct option *longopts_array;
    int longopts_counter;

    longopts_array = NULL;
    longopts_counter = 0;

    
}

/* Gives calling function a filepath */
static char *get_filepath(char *dirpath, char *filename)
{
    char *filepath;

    filepath = malloc(sizeof(char) * (strlen(dirpath) + strlen(filename) + 2));
    if (!filepath) {
        return NULL;
    }
    filepath[0] = '\0';
    strcat(filepath, dirpath);
    strcat(filepath, "/");
    strcat(filepath, filename);

    return filepath;
}

/* Gives calling function a plugin name */
static char *get_plugname(char *filename)
{
    char *plugname;

    plugname = malloc(sizeof(char) * (strlen(filename) - 2));
    if (!plugname) {
        return NULL;
    }
    memcpy(plugname, filename, sizeof(char) * (strlen(filename) - 3));
    plugname[strlen(filename) - 3] = '\0';

    return plugname;
}

/* Tells calling function if a string is a plugin name*/
static bool is_plugname(char *filename)
{
    if (strlen(filename) < 3) {
        return false;
    }
    return strcmp(filename + strlen(filename) - 3, ".so") == 0;
}
