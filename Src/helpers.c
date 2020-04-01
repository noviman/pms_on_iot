//
// Created by bartek on 4/1/20.
//

#include "stdlib.h"
#include "string.h"
#include "helpers.h"



// When Using this function, to avoid memory leak, free the memory after usage of this fnc!
char * add_newline_to_message(const char * message)
{
    char *string = malloc( (strlen(message) + 4) * sizeof(char));
    sprintf(string, "%s\r\n", message);
    return string;
}

char * remove_req_from_read(const char * mes_recv)
{
    char *ptr = strchr(mes_recv, '\n') + 1;
    return ptr;
}