#ifndef _MESSAGES_H
#define _MESSAGES_H

enum {
    file_open_error_code=1,
    usage_error_code=2,
    file_closure_error_code=3,
    file_reading_error_code=4,
    memory_allocation_error_code=5
};

void usage_error_message(void);

#endif