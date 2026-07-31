#include <stdlib.h>
#include <stdio.h>

#include "file_walk.h"
#include "messages.h"
#include "target.h"
#include "codes.h"
#include "debug.h"
#include "args.h"

int main(int argc, char **argv)
{
    char *target_str, *catalogue;
    target_t target;
    bool result;
    args_t args;

    if (debug()) {
        fprintf(stderr, "Program started.\n");
    }
    
    parse_args(argc, argv, &args);

    if (!is_correct(&args)) {
        /*
         * Invalid program usage. Exiting
         */
        if (debug()) {
            fprintf(stderr, "Invalid usage detected.\n");
        }
        usage_message();
        if (debug()) {
            fprintf(stderr, "Program terminated.\n");
        }
        exit(invalid_usage_code);
    }

    if (is_version(&args)) {
        /*
         * User wants version information. Sending and exiting.
         */
        version_message();
        if (debug()) {
            fprintf(stderr, "Version message was sent.\n");
            fprintf(stderr, "Program succesfully ended.\n");
        }
        exit(0);
    }

    if (is_help(&args)) {
        /*
         * User wants help information. Sending and exiting.
         */
        help_message();
        if (debug()) {
            fprintf(stderr, "Help message was sent.\n");
            fprintf(stderr, "Program succesfully ended.\n");
        }
        exit(0);
    }
    
    /*
     * Parameter initialization.
     */
    catalogue = get_catalogue(&args);
    if (debug()) {
        fprintf(stderr, "Catalogue was specified. Catalogue is %s.\n",
                catalogue);
    }
    target_str = get_target(&args);
    result = init_target(&target, target_str);
    if (debug()) {
        fprintf(stderr, "Target was specified. Target is %s.\n", target_str);
    }
    if (!result) {
        /*
         * Invalid target format.
         */
        if (debug()) {
            fprintf(stderr, "User specified invalid target.\n");
        }
        invalid_target_message();
        if (debug()) {
            fprintf(stderr, "Invalid target message was sent.\n");
            fprintf(stderr, "Program terminated.\n");
        }
        exit(invalid_target_code);
    }
    
    result = file_walk(catalogue, &target);
    if (!result) {
        /*
         * FTW couldn't walk catalogue
         */
        if (debug()) {
            fprintf(stderr,"Couldn't walk catalogue tree of %s for walking.\n",
                    catalogue);
        }
        printf("Couldn't open catalogue tree of %s.\n", catalogue);
    } else if (debug()) {
        /*
         * FTW could walk catalogue
         */
        fprintf(stderr, "Walking file tree was succesful.\n");
    }
    
    /*
     * Memory freeing.
     */
    delete_target(&target);
    
    if (debug()) {
        fprintf(stderr, "Memory was freed.\n");
        fprintf(stderr, "Program succesfully ended.\n");
    }

    exit(0);
}
