#include "argparse.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    KIND_SWITCH_SHORT,
    KIND_SWITCH_LONG,
    KIND_ARGUMENT,
} KindOfParam;

void dtypeCaster(CliDataType dtype, void* pvalue, char* data) {
    //
    // Casts and assign to the value ptr
    //
    switch (dtype) {
        case STRING:
            *(char**)pvalue = data;
            break;
        case INT:
            *((int*)pvalue) = atoi(data);
            break;
        default:
            break;
    }
}

int parse(int argc, char **argv, CliElement *cliElements, size_t cliElementsLen) {
    CliSwitch *switch_selected = NULL;

    for (size_t i = 1; i < argc; i++) {
        KindOfParam pkind;
        char *arg     = argv[i];
        size_t argLen = strlen(arg);

        //
        // Check if switch is selected and gets the data
        //

        if (switch_selected) {
            dtypeCaster(switch_selected->dtype, switch_selected->value, arg);
            switch_selected = NULL;
            continue;
        }

        //
        // Check if is a switch or argument
        //

        if (argLen >= 3 && arg[0] == '-' && arg[1] == '-') {
            pkind       = KIND_SWITCH_LONG;
        } else if (argLen >= 2 && arg[0] == '-') {
            pkind       = KIND_SWITCH_SHORT;
        } else {
            pkind = KIND_ARGUMENT;
        }

        //
        // Search the element and find the match
        // --help -> CliSwitch
        // ANYARG -> CliArgument
        //

        bool param_is_switch = pkind == KIND_SWITCH_LONG || pkind == KIND_SWITCH_SHORT;

        for (int ei = 0; ei < cliElementsLen; ei++) {
            // Needs to be a ptr for global edit
            CliElement *pElement = &cliElements[ei];
             
            if (param_is_switch && pElement->type == SWITCH) {
                CliSwitch *pSwitch = &pElement->data.cswitch;

                if (pkind == KIND_SWITCH_LONG
                && strcmp(pSwitch->plong, arg + 2) == 0) {
                    switch_selected = pSwitch;
                    break;
                }
                else if (pkind == KIND_SWITCH_SHORT
                && pSwitch->pshort == *(arg + 1)) {
                    switch_selected = pSwitch;
                    break;
                }
            } else if (pkind == KIND_ARGUMENT && pElement->type == ARGUMENT) {
                // Needs to be a ptr for global edit
                CliArgument *pArg = &pElement->data.arg;

                //
                // Checks if the value is already set
                // IF not means the argument needs to be setted
                // NOTE: Can be improved the tracking of already setted arguments
                //
                if (!pArg->is_set) {
                    dtypeCaster(pArg->dtype, pArg->value, arg);
                    pArg->is_set = 1;
                    break;
                }
            }
        }

        if (param_is_switch) {
            //
            // Check if switch exists
            //

            if (!switch_selected) {
                return -1;
            }

            //
            // Set the value to TRUE if the type is ENABLE
            //

            if (switch_selected->dtype == ENABLE) {
                *((bool*)switch_selected->value) = 1;
                switch_selected = NULL;
            }
        }
    }

    return 0;
}
