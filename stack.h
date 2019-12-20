#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

//хэш
//сторонний стэк

//#define Elem_tDouble

#ifdef Elem_tDouble
    typedef double Elem_t;
    #define ELEM_T "%lg"

#else
    typedef int Elem_t;
    #define ELEM_T "%d"
#endif


const Elem_t  POISON        = -1073676287;
const int     CAN_V         = 0xBEDA,
              INCREASECOEFF = 2,
              INITCAPACITY  = 4,
              N_ERRORS      = 5,
              OK            = 0,
              CAN_ERROR     = 1,
              SIZE_ERROR    = 2,
              DATA_ERROR    = 3,
              HASH_ERROR    = 4,
              CAPACITY_ERROR= 5,
              ERRCODE_ERROR = 6,
              NULL_POINT    = 7;

#define  CHECKSTACK(stk);  if(StackOk(stk)) StackDump(stk, __FILE__, __LINE__, __PRETTY_FUNCTION__);
#define  CANNARY1          *(int*)stk->data
#define  CANNARY2          *(int*)(stk->data + sizeof(stk->cannary0) + stk->capacity * sizeof(Elem_t))
#define  VALUES(i)         (*(Elem_t*)(stk->data + sizeof(stk->cannary0) + (i) * sizeof(Elem_t)) )

struct   stack_t {

    int         cannary0;
    int         size;
    int         errcode;
    long int    hash;
    int         capacity;
    char*       data;
};

long int GavHash(stack_t* stk) {

    assert(stk != NULL);

    const int code = 0xBEDAB0DA;
    long int hash = 0;

    hash += (long int)stk->cannary0 * 1 * code + stk->size * 2 * code + stk->errcode * 3 * code;
    hash += (long int)CANNARY1 * 4 * code + stk->capacity * 5 * code + (long int)CANNARY2 * (stk->capacity + 5);

    for(int i = 0; i < stk->capacity; i++)
        hash += (long int) VALUES(i) * (i + 6) * code;

    return hash;
}

int      StackOk(stack_t* stk) {

    if (!stk)
        return NULL_POINT;

    if (stk->cannary0 != CAN_V || CANNARY1 != CAN_V || CANNARY2 != CAN_V)
        stk->errcode |= (1 << (CAN_ERROR - 1));

    if (stk->capacity <= 0)
        stk->errcode |= (1 << (CAPACITY_ERROR - 1));

    if (stk->size < 0 || stk->size > stk->capacity)
        stk->errcode |= (1 << (SIZE_ERROR - 1));

    if ( !((stk->errcode >> (SIZE_ERROR - 1)) & 1) ) {
        for(int i = stk->size; i < stk->capacity; i++) {
            if (VALUES(i) != POISON) {
                stk->errcode |= (1 << (DATA_ERROR - 1));
                break;
            }
        }
    }

    if (stk->hash != GavHash(stk))
        stk->errcode |= (1 << (HASH_ERROR - 1));

    return stk->errcode;
}

bool     ErrorPrint(int errcode) {

    if (errcode < 0 || errcode > (1 << N_ERRORS) - 1)//one bit for each error
        printf("(ERRCODE_ERROR)");
    else {

        if ((errcode >> (CAN_ERROR  - 1)) & 1)
            printf("(CANARY_ERROR) ");

        if ((errcode >> (SIZE_ERROR - 1)) & 1)
            printf("(SIZE_ERROR) ");

        if ((errcode >> (DATA_ERROR - 1)) & 1)
            printf("(DATA_ERROR) ");

        if ((errcode >> (HASH_ERROR - 1)) & 1)
            printf("(HASH_ERROR) ");

        if ((errcode >> (CAPACITY_ERROR - 1)) & 1)
            printf("(CAPACITY_ERROR) ");
        }

    return 0;
}

bool     DataStkPrint(const stack_t* stk) {

    if (stk->size >= 0 && stk->size <= stk->capacity) {

        for(int i = 0; i < stk->size; i++)
            printf("\t\t *[%d] = "ELEM_T"\n", i + 1, VALUES(i));

        for(int i = stk->size; i < stk->capacity; i++) {

            if (VALUES(i) == POISON)
                printf("\t\t  [%d] = "ELEM_T" (POISON?) \n", i + 1, VALUES(i));
            else
                printf("\t\t  [%d] = "ELEM_T"\n", i + 1, VALUES(i));
        }
    }

    else {

        for(int i = 0; i < stk->capacity; i++)
            printf("\t\t  [%d] = "ELEM_T"\n", i + 1, VALUES(i));
    }

    return 0;
}

bool     StackDump(const stack_t* stk, const char* file, int line, const char* function) {

    if (!stk) {
        printf("\n ERROR: NULL POINTER TO STACK! \n");
        return 1;
    }

    printf("\n STACK DUMP: File: %s, line: %d, function: %s \n", file, line, function);

    printf("\n stack_t [%p] ", stk);
    (stk->errcode == 0) ? printf("(ok)\n") : printf("(ERROR)\n");

    printf("    {\n"
           "\t cannary0 = %d\n"
           "\t size = %d\n"
           "\t errcode = %d ", stk->cannary0, stk->size, stk->errcode);

    ErrorPrint(stk->errcode);

    printf("\n\t hash = %ld\n"
           "\t cannary1 = %d\n"
           "\t data[%d] [%p]\n"
           "\t    {\n", stk->hash, CANNARY1, stk->capacity, stk->data + sizeof(stk->cannary0));

    DataStkPrint(stk);

    printf("\t    }\n"
           "\t cannary2 = %d\n"
           "    }\n\n", CANNARY2);

    //abort();
    return 0;
}

bool     StackInit(stack_t* stk) {

    assert(stk != NULL);

    stk->capacity = INITCAPACITY;
    stk->size = 0;
    stk->cannary0 = CAN_V;
    stk->errcode = OK;

    stk->data = (char*)calloc(stk->capacity * sizeof(Elem_t) + 2 * sizeof(stk->cannary0), 1);
    assert(stk->data != NULL);

    CANNARY1 = CAN_V;
    CANNARY2 = CAN_V;

    for(int i = 0; i < stk->capacity; i++)
         VALUES(i) = POISON;

    stk->hash = GavHash(stk);

    CHECKSTACK(stk);
    return 0;
}

bool     IncreaseData(stack_t* stk) {

    CHECKSTACK(stk);

    char* reservedata = (char*) realloc(stk->data, sizeof(Elem_t) * (stk->capacity *= INCREASECOEFF) + 2 * sizeof(stk->cannary0));
    stk->data = (reservedata != NULL) ? reservedata : stk->data;

    if (reservedata == NULL) {
        stk->capacity /= INCREASECOEFF;
        printf("Not able to increase data");
        return 1;
    }

    for(int i = stk->size; i < stk->capacity; i++)
        VALUES(i) = POISON;

    CANNARY2 = CAN_V;
    stk->hash = GavHash(stk);

    CHECKSTACK(stk);
    return 0;
}

bool     StackPush(stack_t* stk, Elem_t elem) {

    CHECKSTACK(stk);

    if (stk->size == stk->capacity && IncreaseData(stk) == 1) {
        return 1;
    }

    VALUES(stk->size++) = elem;

    stk->hash = GavHash(stk);

    CHECKSTACK(stk);
    return 0;
}

bool     DecreaseData(stack_t* stk) {

    CHECKSTACK(stk);

    char* reservedata = (char*) realloc(stk->data, sizeof(Elem_t) * (stk->capacity /= INCREASECOEFF) + 2 * sizeof(stk->cannary0));
    stk->data = (reservedata != NULL) ? reservedata : stk->data;

    if (reservedata == NULL) {
        stk->capacity *= INCREASECOEFF;
        return 1;
    }

    CANNARY2 = CAN_V;
    stk->hash = GavHash(stk);

    CHECKSTACK(stk);
    return 0;

}

Elem_t   StackPop(stack_t* stk) {

    CHECKSTACK(stk);

    if (stk->size < 1) {
        printf("No more data");
        return 0;
    }

    Elem_t value = VALUES(--stk->size);
    VALUES(stk->size) = POISON;
    stk->hash = GavHash(stk);

    if (stk->size == stk->capacity / 2 - 4)
        DecreaseData(stk);

    CHECKSTACK(stk);
    return value;
}

bool     StackKill(stack_t* stk) {

    CHECKSTACK(stk);

    free(stk->data);
    stk->cannary0 = 0;
    stk->size = 0;
    stk->errcode = OK;
    stk->hash = 0;
    stk->data = NULL;

    return 0;
}
