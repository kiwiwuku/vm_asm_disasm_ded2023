#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys\stat.h>

//cd documents\projects\vm_asm_disasm_ded2023\assembler

enum Errors{
    OK  = 0,
    ERR = 1
};
struct Command{
    int name_length;
    char name[10];
    int number;
    int args_count;
};
struct Text{
    char* path;
    char* buffer;
    int buffer_count;
};

Errors get_commands(Command* arr, int* length);
Errors print_str(char* str);
Errors slice(char* str, char* new_str, int offset);

Errors get_file_buf(Text* text);
Errors translate_to_cs_code(Command* commands, int commands_count, char* buf, int* code_cs, int code_cs_length);
Errors write_in_file(char* compiled_path, int* code_cs, int code_cs_length);

Errors Text_ctor(Text* text);
Errors Text_dtor(Text* text);
Errors Text_verif(Text* text);


int main()
{
    Errors err = OK;
    int commands_count = 0;
    Command* commands = (Command*)calloc(1, sizeof(Command));

    Text text = {};
    Text_ctor(&text);

    text.path = "original_file.txt";

    if (err = get_commands(commands, &commands_count))
    {
        printf("Error in get_commands! err = %d Sorry, Bye!\n", err);
        free(commands);
        return 0;
    }
    assert(commands_count);

    int code_cs[100] = {};
    int code_cs_length = 100;

    err = get_file_buf(&text);
    if (err)
    {
        printf("Error in get_file_buf! err = %d Sorry, Bye!\n", err);
        free(commands);
        return 0;
    }
    assert(text.buffer);

    //err = translate_to_cs_code(commands, commands_count, buf, code_cs, code_cs_length);
    if (err)
    {
        printf("Error in translate_to_cs_code! err = %d Sorry, Bye!\n", err);
        free(commands);
        return 0;
    }

    char* compiled_path = "../compiled_file.txt";
    FILE *compiled_file = fopen(compiled_path, "wb");
    if (!compiled_file)
    {
        printf("Compiled file is null\nBye!\n");
        free(commands);
        return 0;
    }


    printf("Closing files and freeing memory...\n");
    fclose(compiled_file);
    free(commands);
    printf("Files are closed and free happened. Bye!\n");
    return 0;
}

Errors get_file_buf(Text* text)
{
    printf("get_file_buf...\n");
    assert(text->path);

    FILE *orig_file = fopen(text->path, "rb");
    if (!orig_file)
    {
        printf("Original file is null\nBye!\n");
        return ERR;
    }

    struct stat st;
    stat(text->path, &st);

    text->buffer_count = st.st_size + 1;

    if (text->buffer_count <= 1)
        return ERR;
    printf("1 buf = %p\n", text->buffer);
    text->buffer = (char*)calloc(text->buffer_count, sizeof(char));
    printf("2 buf = %p\n", text->buffer);
    assert(text->buffer);

    fread(text->buffer, sizeof(char), text->buffer_count, orig_file);
    printf("2 buf = %p\n", text->buffer);

    return OK;
}

Errors translate_to_cs_code(Command* commands, int commands_count, char* buf, int* code_cs, int code_cs_length)
{
    assert(commands);
    assert(commands_count > 0);
    assert(buf);
    assert(code_cs);

    /*for (int i = 0; i < code_cs_length)
    {
        fscanf()
    }*/
    return OK;
}

Errors print_str(char* str)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++)
        printf("%c(%d)", str[i], str[i]);
    return OK;
}

Errors slice(char* str, char* new_str, int offset)
{
    assert(str);
    assert(new_str);

    const int len = (const int)strlen(str);
    new_str[0] = ' ';
    int i = 1;
    printf("slice() ...\n");

    while ((i+offset) < len)
    {
        new_str[i] = str[i+offset];
        printf("i = %d, new_str[i] = %c(%d)\n", i, new_str[i], new_str[i]);
        i++;
    }
    printf("sliced!");
    return OK;
}

Errors get_commands(Command* arr, int* length)
{
    assert(arr);
    assert(length);
    printf("get_commands...\n");
    char* cmds_path = "../commands.txt";
    FILE *cmds_file = fopen(cmds_path, "rb");
    if (!cmds_file)
    {
        printf("Commands file is null\nBye!\n");
        return ERR;
    }
    fscanf(cmds_file, "%d", length); //из первой строчки считываем кол-во команд
    arr = (Command*)realloc(arr, (*length)*sizeof(Command));
    assert(arr);
    for (int i = 0; i < *length; i++)
    {
        if (!fscanf(cmds_file, "\n%s", arr[i].name))
        {
            printf("Error! Can't read command's name!\n");
            fclose(cmds_file);
            return ERR;
        }
        arr[i].name_length = strlen(arr[i].name);
        if (!fscanf(cmds_file, "%d", &(arr[i].number)))
        {
            printf("Error! Can't read command's number!\n");
            fclose(cmds_file);
            return ERR;
        }
        if (!fscanf(cmds_file, "%d", &(arr[i].args_count)))
        {
            printf("Error! Can't read command's args count!\n");
            fclose(cmds_file);
            return ERR;
        }
    }

    for (int i = 0; i < *length; i++)
    {
        printf("commands[%d] = %s = %d args_count = %d name_length = %d\n",
               i, arr[i].name, arr[i].number, arr[i].args_count, arr[i].name_length);
    }

    fclose(cmds_file);
    return OK;
}

Errors Text_ctor(Text* text)
{
    text->path = NULL;
    text->buffer = NULL;
    text->buffer_count = 0;

    return OK;
}

Errors Text_dtor(Text* text)
{
    free(text->buffer);
    text->path = NULL;
    text->buffer_count = -1;

    return OK;
}

Errors Text_verif(Text *text)
{
    assert(text);
    assert(text->path);
    assert(text->buffer);
    assert(text->buffer_count >= 0);

    return OK;
}
