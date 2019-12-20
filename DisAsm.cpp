#include "Headers\stack.h"
#include "Headers\Enum.h"
#include "Headers\Functions.h"

//FUNCTIONS

bool DisAsmWriter(char* text, int TextSize, FILE* Commands) {

    assert(text != NULL);
    assert(Commands != NULL);

    for(int pc = 0; pc < TextSize; pc++) {

        Elem_t value = 0;

        #define DEF_CMD(name, num, n_param, code) case CMD_##name:\
                                                    fprintf(Commands, #name);\
                                                    for(int count = 0; count < n_param; count++) {\
                                                        fprintf(Commands, " "ELEM_T, *(Elem_t*)(text + pc + 1));\
                                                        pc += sizeof(Elem_t);\
                                                    }\
                                                    if (n_param == -1) {\
                                                        fprintf(Commands, " %d", *(int*)(text + pc + 1));\
                                                        pc += sizeof(int);\
                                                    }\
                                                    break;
        switch (text[pc]) {

        #include "Headers\Commands.h"

        }

        fprintf(Commands, "\n");

        #undef  DEF_CMD(name, num, n_param, code)
    }

    return 0;
}

int  main() {

    int SizeFile = 0;

    char* text = InputReader("CommandCode.mx", &SizeFile);
    assert (text != NULL);

    FILE* Commands = fopen("CommandText.mx", "wb");
    assert(Commands != NULL);

    DisAsmWriter(text, SizeFile, Commands);

    fclose(Commands);
    return 0;
}
