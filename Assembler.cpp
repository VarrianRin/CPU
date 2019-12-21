#include "Headers\stack.h"
#include "Headers\Enum.h"
#include "Headers\Functions.h"

const int MAXCODESIZE  = 1000,
          MAXLABELS    = 1000,
          MAXCOMSIZE   = 20,
          MAXLABELNAME = 20;

//FUNCTIONS

struct Label {

    char labelname[MAXLABELNAME];
    int  jmpvalue;
};

int    strcmp_noreg(char* line1, char* line2) {

    assert(line1 != NULL);
    assert(line2 != NULL);

    for(int i = 0; line1[i] != '\0'; i++)
        line1[i] = toupper((int)line1[i]);

    return strcmp(line1, line2);
}

void   TestWriter(char* code, int size) {

    assert(code != NULL);

    for(int i = 0; i < size; i++)
        printf("%d, ", code[i]);
    printf("\n");
}

void   ParamCase(char* CommandCode, const char* text, int* pc, int* i, int pass) {

    int     n_bytes   = 0;

    if (*(text+*i + 1) == '[') {

        char smth[MAXCOMSIZE] = {};
        *i += 2;
        *(char*)(CommandCode + *pc) = '[';
        *pc += sizeof(char);

        sscanf(text + *i, "%s%n", smth, &n_bytes);
        *(char*)(CommandCode + *pc) = '@';
        *pc += 1;

        for (int n = 0; smth[n]; n++, *pc += 1)
            *(char*)(CommandCode + *pc) = smth[n];

        *i += n_bytes;
    }
}

void   JumpCase(char* CommandCode, const char* text, Label* labels, int* pc, int* i, int labelamount, int pass) {

    assert(CommandCode != NULL);
    assert(text != NULL);
    assert(labels != NULL);
    assert(pc != NULL);
    assert(i != NULL);

    int  n_bytes = 0;
    char JumpLabel[MAXLABELNAME] = {};

    if (pass == 1) {
        *(int*)(CommandCode + *pc) = -1;
        *pc += sizeof(int);
        sscanf(text + *i, " %*s%n", &n_bytes);
        *i += n_bytes;
    }
    else {
        sscanf(text + *i, "%s%n", JumpLabel, &n_bytes);

        for(int lc = 0; lc < labelamount; lc++) {

            if ( !strcmp(JumpLabel, labels[lc].labelname) ) {
                *(int*)(CommandCode + *pc) = labels[lc].jmpvalue;
                break;
            }
        }
        if (*(int*)(CommandCode + *pc) == -1)
            *(int*)(CommandCode + *pc) = atoi(JumpLabel);

        *pc += sizeof(int);
        *i += n_bytes;
    }
}

void   CommandCases(const char* command, char* CommandCode, const char* text, Label* labels, int* pc, int* i, int labelamount, int pass) {

    assert(command != NULL);
    assert(CommandCode != NULL);
    assert(text != NULL);
    assert(labels != NULL);
    assert(pc != NULL);
    assert(i != NULL);

    Elem_t  value     = 0;
    bool    mistake   = 1;
    int     n_bytes   = 0;

    #define DEF_CMD(name, number, n_param, code)                                    \
                                                                                    \
        if ( !strcmp(command, #name) ) {                                            \
            mistake = 0;                                                            \
            CommandCode[(*pc)++] = CMD_##name;                                      \
        if (*(text+*i + 1) == '[')                                                  \
            ParamCase(CommandCode, text, pc, i, pass);                              \
         else {                                                                     \
            for(int count = 0; count < n_param; count++) {                          \
                                                                                    \
                sscanf(text + *i, ELEM_T"%n", &value, &n_bytes);                    \
                *(Elem_t*)(CommandCode + *pc) = value;                              \
                (*pc) += sizeof(Elem_t);                                            \
                *i += n_bytes + 1;                                                  \
            }                                                                       \
        }                                                                           \
        if (n_param == -1) {                                                        \
            JumpCase(CommandCode, text, labels, pc, i, labelamount, pass);          \
            if (*(text+*i + 1) == '[')                                              \
                ParamCase(CommandCode, text, pc, i, pass);                          \
            else if (number == CMD_CALL)                                            \
                CommandCode[(*pc)++] = 0;                                           \
        }                                                                           \
        }

    #include "Headers\Commands.h"

    #undef  DEF_CMD(name, number, n_param, code)

    if (mistake && pass == 2)
        printf("ERROR: UNKNOWN COMMAND  -  %s \n"
               "PROGRAM OFFSET: %d\n", command, *pc);
}

char*  AsmWriter(const char* text, int* pc) {

    assert(text != NULL);

    Label    labels[MAXLABELS]        = {};
    char     command[MAXCOMSIZE]      = {};
    char     CommandCode[MAXCODESIZE] = {};
    int      n_bytes                  = 0;
    int      labelamount              = 0;

    for(int pass = 1; pass < 3; pass++) {

        int n_label = 0;
        *pc = 0;

        for(int i = 0; sscanf(text + i, "%s%n", command, &n_bytes) != EOF; ) {

            i += n_bytes;

            //if comment
            if (*command == ';') {
                sscanf(text + i, "%*[^\n]%n", &n_bytes);
                i += n_bytes;
                continue;
            }
            //if label
            if (command[strlen(command) - 1] == ':') {

                sscanf(text + i - n_bytes, " %[^:]", labels[n_label].labelname);
                labels[n_label++].jmpvalue = *pc;
                if (*(text + i + 1) == '[')
                    ParamCase(CommandCode, text, pc, &i, pass);
                continue;
            }

            CommandCases(command, CommandCode, text, labels, pc, &i, labelamount, pass);
        }

        labelamount = n_label;
        //TestWriter(CommandCode, *pc);
    }

    return CommandCode;
}

int    main() {

    int SizeFile = 0;
    char* text = InputReader("F:\\proga\\StackCPU\\input.txt", &SizeFile);
    assert (text != NULL);

    int pc = 0;

    char* CommandCode = AsmWriter(text, &pc);
    assert (CommandCode != NULL);

    TestWriter(CommandCode, pc);

    CodeWriter(CommandCode, pc);

    return 0;
}

    /*#include "Headers\stack.h"
#include "Headers\Enum.h"
#include "Headers\Functions.h"

const int MAXCODESIZE  = 1000,
          MAXLABELS    = 1000,
          MAXCOMSIZE   = 20,
          MAXLABELNAME = 20;

//FUNCTIONS

struct Label {

    char labelname[MAXLABELNAME];
    int  jmpvalue;
};

int    strcmp_noreg(char* line1, char* line2) {

    assert(line1 != NULL);
    assert(line2 != NULL);

    for(int i = 0; line1[i] != '\0'; i++)
        line1[i] = toupper((int)line1[i]);

    return strcmp(line1, line2);
}

void   JumpCase(char* CommandCode, const char* text, Label* labels, int* pc, int* i, int labelamount, int pass) {

    assert(CommandCode != NULL);
    assert(text != NULL);
    assert(labels != NULL);
    assert(pc != NULL);
    assert(i != NULL);

    int  n_bytes = 0;
    char JumpLabel[MAXLABELNAME] = {};

    if (pass == 1) {
        *(int*)(CommandCode + *pc) = -1;
        *pc += sizeof(int);
        sscanf(text + *i, "%*[^\n]%n", &n_bytes);
        *i += n_bytes;
    }
    else {
        sscanf(text + *i, "%s%n", JumpLabel, &n_bytes);

        for(int lc = 0; lc < labelamount; lc++) {

            if ( !strcmp(JumpLabel, labels[lc].labelname) ) {
                *(int*)(CommandCode + *pc) = labels[lc].jmpvalue;
                break;
            }
        }
        if (*(int*)(CommandCode + *pc) == -1)
            *(int*)(CommandCode + *pc) = atoi(JumpLabel);

        *pc += sizeof(int);
        *i += n_bytes;
    }
}



void   CommandCases(const char* command, char* CommandCode, const char* text, Label* labels, int* pc, int* i, int labelamount, int pass) {

    assert(command != NULL);
    assert(CommandCode != NULL);
    assert(text != NULL);
    assert(labels != NULL);
    assert(pc != NULL);
    assert(i != NULL);

    Elem_t  value     = 0;
    bool    mistake   = 1;
    int     n_bytes   = 0;

    #define DEF_CMD(name, number, n_param, code)                                    \
                                                                                    \
        if ( !strcmp(command, #name) ) {                                            \
            mistake = 0;                                                            \
            CommandCode[(*pc)++] = CMD_##name;                                      \
                                                                                    \
        ParamCase(CommandCode, text, pc, i);                                        \
        if (*(text+*i + 1) != '[') {                                                \
            for(int count = 0; count < n_param; count++) {                          \
                                                                                    \
                sscanf(text + *i, ELEM_T"%n", &value, &n_bytes);                    \
                *(Elem_t*)(CommandCode + *pc) = value;                              \
                (*pc) += sizeof(Elem_t);                                            \
                *i += n_bytes + 1;                                                  \
            }                                                                       \
        }                                                                           \
        }                                                                           \
        if (n_param == -1) {                                                        \
            JumpCase(CommandCode, text, labels, pc, i, labelamount, pass);          \
            ParamCase(CommandCode, text, pc, i);                                    \
        }


    #include "Headers\Commands.h"

    #undef  DEF_CMD(name, number, n_param, code)

    if (mistake && pass == 2)
        printf("ERROR: UNKNOWN COMMAND  -  %s \n"
               "PROGRAM OFFSET: %d\n", command, *pc);
}

char*  AsmWriter(const char* text, int* pc) {

    assert(text != NULL);

    Label    labels[MAXLABELS]        = {};
    char     command[MAXCOMSIZE]      = {};
    char     CommandCode[MAXCODESIZE] = {};
    int      n_bytes                  = 0;
    int      labelamount              = 0;

    for(int pass = 1; pass < 3; pass++) {

        int n_label = 0;
        *pc = 0;

        for(int i = 0; sscanf(text + i, "%s%n", command, &n_bytes) != EOF; ) {

            i += n_bytes;

            //if comment
            if (*command == ';') {
                sscanf(text + i, "%*[^\n]%n", &n_bytes);
                i += n_bytes;
                continue;
            }
            //if label
            if (command[strlen(command) - 1] == ':') {
                sscanf(text + i - n_bytes, " %[^:]", labels[n_label].labelname);
                labels[n_label++].jmpvalue = *pc;
                ParamCase(CommandCode, text, pc, &i);
                continue;
            }

            CommandCases(command, CommandCode, text, labels, pc, &i, labelamount, pass);
        }

        labelamount = n_label;
    }

    return CommandCode;
}

void   TestWriter(char* code, int size) {

    assert(code != NULL);

    for(int i = 0; i < size; i++)
        printf("%d, ", code[i]);
    printf("\n");
}

int    main() {

    int SizeFile = 0;
    char* text = InputReader("F:\\proga\\StackCPU\\input.txt", &SizeFile);
    assert (text != NULL);

    int pc = 0;

    char* CommandCode = AsmWriter(text, &pc);
    assert (CommandCode != NULL);

    TestWriter(CommandCode, pc);

    CodeWriter(CommandCode, pc);

    return 0;
}
                  */
