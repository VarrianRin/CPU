#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys\stat.h>


void  CodeWriter(const char* CommandCode, int size) {

    assert(CommandCode != NULL);

    FILE* Commands = fopen("CommandCode.mx", "wb");
    assert(Commands != NULL);

    for(int i = 0; i < size; i++)
        fputc(CommandCode[i], Commands);

    fclose(Commands);

}

int   GetFileSize(const char* FileName) {

    assert(FileName != NULL);

    struct _stat buff = {};
    int StatReturn = _stat(FileName, &buff);
    assert(StatReturn == 0);

    return buff.st_size;
}

char* WinReader(const char* FileName, int SizeFile) {

    assert(FileName != NULL);

    FILE* TextFile = fopen(FileName, "rb");
    assert(TextFile != NULL);

    char* text  = (char*) calloc(SizeFile + 1, sizeof(text[0]));

    assert(text != NULL);

    int ReadReturn = fread(text, sizeof(text[0]), SizeFile, TextFile);
    assert (ReadReturn == SizeFile);

    fclose(TextFile);

    text[SizeFile] = '\0';

    return text;
}

char* InputReader(const char* FileName, int* SizeFile) {

    assert(FileName != NULL);

    *SizeFile = GetFileSize(FileName);

    char* input = WinReader(FileName, *SizeFile);
    assert (input != NULL);

    return input;
}
