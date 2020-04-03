//
// Created by bartek on 4/1/20.
//

#include "stdlib.h"
#include "string.h"
#include "helpers.h"


char * remove_req_from_read(const char * mes_recv)
{
    char *ptr = strchr(mes_recv, '\n') + 1;
    return ptr;
}