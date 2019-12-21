
    DEF_CMD(END      ,-1, 0, ({StackKill(&stack);

                               while(VarId  > 0)
                                   FreeVar(&VarList[--VarId]);

                               printf("\nPress anything to exit\n");
                               getch();
                               return 0;}) )

    //операции со стеком и регистрами
    DEF_CMD(PUSH     , 1, 1, ({pc++;                   // 0 < номер <= 9 операции со стеком

                               if (*(text + pc) == '[' && *(text + pc + 1) == '@') {
                                   char smth[SMTHSIZE] = {};
                                   int n_bytes = 0;
                                   sscanf(text + pc, "[@%[^]]]%n", smth, &n_bytes);
                                   pc += n_bytes - 1; //printf("\n'%s'\n", smth);

                                   if ( !strcmp(smth, "ax") )
                                       StackPush(&stack, ax);

                                   else if ( !strcmp(smth, "bx") )
                                       StackPush(&stack, bx);

                                   else if ( !strcmp(smth, "cx") )
                                       StackPush(&stack, cx);

                                   else if ( !strcmp(smth, "dx") )
                                       StackPush(&stack, dx);

                                   else {
                                       bx = StackPop(&bxStack);
                                       StackPush(&bxStack, bx);
                                       StackPush(&stack, VarList[GetG(smth, VarList, ax, bx, cx, dx)].value); //printf("%d", GetE(smth, VarList, ax, bx, cx, dx));
                                   }
                               }
                               else {
                                   StackPush(&stack, *(Elem_t*)(text + pc));
                                   pc += sizeof(Elem_t) - 1;
                               } }) )

    DEF_CMD(POP      , 2, 0, ({if (*(text + pc + 1) == '[' && *(text + pc + 2) == '@') {
                                   char smth[SMTHSIZE] = {};
                                   int n_bytes = 0;
                                   sscanf(text + ++pc, " [@%[^]]]%n", smth, &n_bytes);
                                   pc += n_bytes - 1; //printf("\n'%s'\n", smth);

                                   if ( !strcmp(smth, "ax") )
                                       ax = StackPop(&stack);

                                   else if ( !strcmp(smth, "bx") )
                                       bx = StackPop(&stack);

                                   else if ( !strcmp(smth, "cx") )
                                       cx = StackPop(&stack);

                                   else if ( !strcmp(smth, "dx") )
                                       dx = StackPop(&stack);

                                   else {
                                       bx = StackPop(&bxStack);
                                       StackPush(&bxStack, bx);
                                       VarList[GetG(smth, VarList, ax, bx, cx, dx)].value = StackPop(&stack); // printf("'%d'", GetG(smth, VarList, ax, bx, cx, dx));
                                   }
                               }
                               else
                                   StackPop(&stack);  }) )

    DEF_CMD(OUT      , 3, 0, ({printf(ELEM_T"\n", StackPop(&stack));}) )

    DEF_CMD(MOUT     , 4, 0, ({Elem_t out = StackPop(&stack); //OUT that doesn't pop
                               printf(ELEM_T"\n", out);
                               StackPush(&stack, out);}) )

    DEF_CMD(STACKDUMP, 5, 0, ({StackDump(&stack,  __FILE__, __LINE__, __PRETTY_FUNCTION__);}) )

    DEF_CMD(GETSIZE  , 6, 0, ({StackPush(&stack, stack.size);}) )

    DEF_CMD(IN       , 7, 0, ({Elem_t in;
                               scanf(ELEM_T, &in);
                               StackPush(&stack, in);}) )

    DEF_CMD(DEC      , 8 ,0, ({pc++;
                               char name[NAMESIZE] = {};
                               int n_bytes = 0;
                               sscanf(text + pc, "[@%[^]]]%n", name, &n_bytes);
                               pc += n_bytes - 1;
                               VarList[VarId].name = strdup(name);
                               VarList[VarId].value = 0;
                               VarList[VarId++].global = global; }))

    //арифметика

    DEF_CMD(ADD      ,10, 0, ({StackPush(&stack, StackPop(&stack) + StackPop(&stack));}) )  // 9 < номер <= 19 арифметика

    DEF_CMD(SUB      ,11, 0, ({Elem_t number = StackPop(&stack);
                               StackPush(&stack, StackPop(&stack) - number);}) )

    DEF_CMD(MUL      ,12, 0, ({StackPush(&stack, StackPop(&stack) * StackPop(&stack));}) )

    DEF_CMD(DIV      ,13, 0, ({Elem_t number = StackPop(&stack);
                               StackPush(&stack, StackPop(&stack) / number);}) )

    DEF_CMD(SQRT     ,14, 0, ({StackPush(&stack, sqrt(StackPop(&stack)));}) )

    //тригониметрия

    DEF_CMD(COS      ,22, 0, ({StackPush(&stack, cos(StackPop(&stack)));}) )  // 19 < номер <= 29 тригониметрия

    //jumps  (have n_param = -1)

    DEF_CMD(JMP      ,30,-1, ({pc = *(int*)(text + pc + 1) - 1;}) )

    DEF_CMD(JA       ,31,-1, ({Elem_t first_n  = StackPop(&stack);
                               Elem_t second_n = StackPop(&stack);
                               pc = (second_n > first_n) ? *(int*)(text + pc + 1) - 1 : pc + sizeof(int) - 1;
                               StackPush(&stack, second_n);
                               StackPush(&stack, first_n);}) )

    DEF_CMD(JAE      ,32,-1, ({Elem_t first_n  = StackPop(&stack);
                               Elem_t second_n = StackPop(&stack);
                               pc = (second_n >= first_n) ? *(int*)(text + pc + 1) - 1 : pc + sizeof(int) - 1;
                               StackPush(&stack, second_n);
                               StackPush(&stack, first_n);}) )

    DEF_CMD(JB       ,33,-1, ({Elem_t first_n  = StackPop(&stack);
                               Elem_t second_n = StackPop(&stack);
                               pc = (second_n < first_n) ? *(int*)(text + pc + 1) - 1 : pc + sizeof(int) - 1;
                               StackPush(&stack, second_n);
                               StackPush(&stack, first_n);}) )

    DEF_CMD(JBE      ,34,-1, ({Elem_t first_n  = StackPop(&stack);
                               Elem_t second_n = StackPop(&stack);
                               pc = (second_n <= first_n) ? *(int*)(text + pc + 1) - 1 : pc + sizeof(int) - 1;
                               StackPush(&stack, second_n);
                               StackPush(&stack, first_n);}) )

    DEF_CMD(JE       ,35,-1, ({Elem_t first_n  = StackPop(&stack);
                               Elem_t second_n = StackPop(&stack);
                               //printf("'%d', '%d'", second_n, first_n);
                               pc = (second_n == first_n) ? *(int*)(text + pc + 1) - 1 : pc + sizeof(int) - 1;
                               StackPush(&stack, second_n);
                               StackPush(&stack, first_n);}) )

    DEF_CMD(JNE      ,36,-1, ({Elem_t first_n  = StackPop(&stack);
                               Elem_t second_n = StackPop(&stack);
                               pc = (second_n != first_n) ? *(int*)(text + pc + 1) - 1 : pc + sizeof(int) - 1;
                               StackPush(&stack, second_n);
                               StackPush(&stack, first_n);}) )

    DEF_CMD(CALL     ,37,-1, ({if (*(text + pc + 5) == '[' && *(text + pc + 6) == '@') {

                                   char smth[SMTHSIZE] = {};
                                   int n_bytes = 0;
                                   sscanf(text + ++pc + 4, " [@%[^]]]%n", smth, &n_bytes);
                                   StackPush(&ReturnStack, pc + 4 + n_bytes - 1);
                                   pc = *(int*)(text + pc) - 1 + CallParam(smth, VarList, &bxStack, &cxStack, text, pc, 1);
                               }
                               else {
                                   CallParam(nullptr, VarList, &bxStack, &cxStack, text, pc, 0);
                                   StackPush(&ReturnStack, pc + 4);  //printf("%d", pc + 4);
                                   pc = *(int*)(text + pc + 1) - 1;
                               } }) )

    DEF_CMD(RET      ,38, 0, ({pc = StackPop(&ReturnStack);
                               if (*(text + pc - 4) != CMD_CALL)
                                  CallRet(VarList, &bxStack, &cxStack);}) )

    //остальное

    DEF_CMD(VARS     ,40, 0, ({printf("\n------------------------");
                               for(int n = 0; n < VarId; n++)
                                   printf("\n%d, \"%s\"[%d] = %d", VarList[n].global, VarList[n].name, n, VarList[n].value);
                               printf("\nglobal = %d\nVarId = %d\n------------------------\n", global, VarId); }))

    DEF_CMD(REGDUMP ,100, 0, ({printf("\nREGISTER DUMP:\n"
                                         "ax = %d\n"
                                         "bx = %d\n"
                                         "cx = %d\n"
                                         "dx = %d\n", ax, bx, cx, dx);}) )

