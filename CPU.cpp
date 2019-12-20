#include <math.h>
#include <conio.h>
#include "Headers\stack.h"
#include "Headers\Enum.h"
#include "Headers\Functions.h"

const int RAMSIZE  = 1000,
          SMTHSIZE = 50, //size of string in [] for PUSH/POP
          N_VAR    = 30,
          NAMESIZE = 15;

struct Variable {

    char*  name;
    bool   global;
    Elem_t value;
};

const char* str = "";

int global = 0;
int VarId = 0;

int VarID(Variable* VarList, const char* name, int bx) {

    for(int i = bx; VarList[i].name; i++)
        if ( !strcmp(VarList[i].name, name) )
            return i;

    for(int i = 0; i < bx; i++)
        if ( !strcmp(VarList[i].name, name) )
            return i;

    return POISON;
}

int CallParam(const char* smth, Variable* VarList, stack_t* bxstack, stack_t* cxstack, char* text, int pc) {///нельзя передавать числа в качестве параметров
////bx смещение для первой переменной функции, cx для текущей при вызове
    if (!smth || !VarList) return POISON;

    char vars[N_VAR][NAMESIZE] = {};
    char params[N_VAR][NAMESIZE] = {};
    int offset = 0, pcoffset = 0;
    StackPush(cxstack, VarId);

    for(int i = 0; i < N_VAR && sscanf(smth, "%[^,],%n", vars[i], &offset) != -1; i++, smth += offset);

    pc = *(int*)(text + pc + 1) - 1;
    sscanf(text + pc, " [@%[^]]]%n", smth, &pcoffset);

    for(int i = 0; i < N_VAR && sscanf(smth, "%[^,],%n", params[i], &offset) != -1; i++, smth += offset);

    int i = 0;
    int bx = StackPop(bxstack);
    StackPush(bxstack, bx);

    for( ; vars[i]; i++) {

        VarList[VarId].name = strdup(params[i]);
        VarList[VarId].global = global + 1;
        VarList[VarId++].value = VarList[VarID(VarList, vars[i], bx)].value;
    }

    StackPush(bxstack, VarId - i);
    global++;

    return pcoffset;
}

void FreeVar(Variable* var) {

    assert(var);

    free(var->name);
    var->value = 0;
    var->global = 0;
}

int CallRet(Variable* VarList, stack_t* bxstack, stack_t* cxstack) {

    for(int i = StackPop(bxstack); i < VarId; i++)
        FreeVar(&VarList[i]);

    VarId = StackPop(cxstack);
    global--;

    return VarId;
}

int GetId(Variable* VarList, int ax, int bx, int cx, int dx) {

    char name[NAMESIZE] = {};

    for(int i = 0; isalpha(*str); i++, str++)
        name[i] = *str;

    if ( !strcmp(name, "ax"))
        return ax;

    if ( !strcmp(name, "bx"))
        return bx;

    if ( !strcmp(name, "cx"))
        return cx;

    if ( !strcmp(name, "dx"))
        return dx;

    return  VarID(VarList, name, bx);
}

int GetN() {

    int val = 0;

    while (isdigit(*str)) {

        val = val * 10 + *str - '0';
        str++;
    }
    return val;
}

int GetE(Variable* VarList, int ax, int bx, int cx, int dx);

int GetP(Variable* VarList, int ax, int bx, int cx, int dx) {

    if (*str == '(') {

        str++;
        int val = GetE(VarList, ax, bx, cx, dx);

        assert(*str == ')');
        str++;
        return val;
    }
    else if (isalpha(*str))
        return GetId(VarList, ax, bx, cx, dx);
    else
        return GetN();
}

int GetT(Variable* VarList, int ax, int bx, int cx, int dx) {

    int val = GetP(VarList, ax, bx, cx, dx);

    while (*str == '*' || *str == '/') {

        char oper = *str;
        str++;

        if (oper == '*')
            val *= GetP(VarList, ax, bx, cx, dx);
        else
            val /= GetP(VarList, ax, bx, cx, dx);
        }

    return val;
}

int GetE(Variable* VarList, int ax, int bx, int cx, int dx) {

    int val = GetT(VarList, ax, bx, cx, dx);

    while (*str == '+' || *str == '-') {

        char oper = *str;
        str++;

        if(oper == '+')
            val += GetT(VarList, ax, bx, cx, dx);
        else
            val -= GetT(VarList, ax, bx, cx, dx);
        }

    return val;
}

int GetG(const char* data, Variable* VarList, int ax, int bx, int cx, int dx) {

    str = data;
    int val = GetE(VarList, ax, bx, cx, dx);
    assert(*str == '\0');

    str++;

    return val;
}

int main() {

    stack_t stack = {};
    StackInit(&stack);

    stack_t ReturnStack = {};
    StackInit(&ReturnStack);

    stack_t bxStack = {};
    StackInit(&bxStack);
    StackPush(&bxStack, 0);

    stack_t cxStack = {};
    StackInit(&cxStack);
    StackPush(&bxStack, 0);

    Elem_t RAM[RAMSIZE] = {};

    int SizeFile = 0;
    char *text = InputReader("CommandCode.mx", &SizeFile);
    assert (text != NULL);

    Variable VarList[N_VAR] = {};                        //implementer(text, &stack, &ReturnStack);

    Elem_t ax = 0, bx = 0, cx = 0, dx = 0;

    for(int pc = 0; ; pc++) {
        //printf("'%d', ", text[pc]);
        #define DEF_CMD(name, num, n_param, code) case CMD_##name:\
                                                             code;\
                                                             break;
        switch (text[pc]) {

        #include "Headers\Commands.h"

        }

        #undef  DEF_CMD
    }

    return 1;
}

