#define DEF_CMD(name, number, n_param, code) CMD_##name = number,

enum command_t {

    #include "Commands.h"

    };

#undef  DEF_CMD
