#ifndef ARGPARSE_H
#define ARGPARSE_H
#include <stddef.h>

#define NEW_CLI_SWITCH(DTYPE, PSHORT, PLONG, VALUE) {       \
    .type  = SWITCH,                                        \
    .data.cswitch = {                                       \
        .dtype = DTYPE,                                     \
        .pshort = PSHORT,                                   \
        .plong = PLONG,                                     \
        .value = VALUE,                                     \
    }                                                       \
}

#define NEW_CLI_ARG(DTYPE, VALUE) {                         \
    .type  = ARGUMENT,                                      \
    .data.arg = {                                           \
        .dtype  = DTYPE,                                    \
        .value  = VALUE,                                    \
        .is_set = 0,                                        \
    }                                                       \
}

typedef enum {
    STRING,
    ENABLE,
    INT
} CliDataType;

typedef enum {
    SWITCH,
    ARGUMENT
} CliType;

typedef struct {
    CliDataType dtype;
    void        *value;
    bool        is_set;
} CliArgument;

typedef struct {
    CliDataType     dtype;
    char            pshort;
    char            *plong;
    void            *value;
} CliSwitch;

typedef struct {
    CliType     type;
    union {
        CliArgument arg;
        CliSwitch   cswitch;
    } data; 
} CliElement;

int parse(int argc, char **argv, CliElement *cliElements, size_t cliElementsLen);

#endif
