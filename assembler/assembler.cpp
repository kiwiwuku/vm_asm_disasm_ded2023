#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys\stat.h>

//cd documents\projects\vm_asm_disasm_ded2023\assembler
//TODO: typedef int elem_t

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
    char** words;
    int words_count;
};
struct Code_SC{
    int* code;
    int capacity;
    int length;
};

Errors get_commands(Command** arr, int* length);
Errors print_str(char* str);

Errors get_file_buf(Text* text);
Errors translate_to_code_sc(Command* commands, int commands_count, Text* text, Code_SC* codesc);
Errors write_in_file(char* compiled_path, char* compiled_path_bin, Code_SC* codesc);
Errors get_command_by_name(Command* commands, int commands_count, Command* command);

Errors print_code_sc(Code_SC* code_sc);
Errors print_commands(Command* commands, int commands_count);

Errors Text_ctor(Text* text);
Errors Text_dtor(Text* text);
Errors Text_verif(Text* text);
Errors Text_get_words(Text* text);
Errors Text_get_words_count(Text* text);

Errors Code_SC_ctor(Code_SC* codesc);
Errors Code_SC_dtor(Code_SC* codesc);
Errors Code_SC_verif(Code_SC* codesc);
Errors Code_SC_realloc(Code_SC* codesc);

int main()
{
    Errors err = OK;
    int commands_count = 0;
    Command* commands = (Command*)calloc(1, sizeof(Command));

    Text text = {};
    Text_ctor(&text);

    text.path = "../original_file.txt";

    if ((err = get_commands(&commands, &commands_count)))
    {
        printf("Error in get_commands! err = %d Sorry, Bye!\n", err);
        free(commands);
        return 0;
    }
    assert(commands_count);

    Code_SC code_sc = {};
    Code_SC_ctor(&code_sc);

    err = get_file_buf(&text);
    if (err)
    {
        printf("Error in get_file_buf! err = %d Sorry, Bye!\n", err);
        free(commands);
        return 0;
    }
    assert(text.buffer);

    err = Text_get_words_count(&text);
    err = Text_get_words(&text);
    if (err)
    {
        printf("Error in get_file_buf! err = %d Sorry, Bye!\n", err);
        free(commands);
        return 0;
    }

    err = translate_to_code_sc(commands, commands_count, &text, &code_sc);
    if (err)
    {
        printf("Error in translate_to_code_sc! err = %d Sorry, Bye!\n", err);
        free(commands);
        return 0;
    }

    char* compiled_path = "../compiled_file.txt";
    char* compiled_path_bin = "../compiled_file.bin";
    err = write_in_file(compiled_path, compiled_path_bin, &code_sc);
    if (err)
    {
        printf("Error in write_in_file! err = %d Sorry, Bye!\n", err);
        free(commands);
        return 0;
    }

    printf("write_in_file happened!\n");
    printf("Closing files and freeing memory...\n");
    free(commands);
    Code_SC_dtor(&code_sc);
    Text_dtor(&text);
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
    text->buffer = (char*)calloc(text->buffer_count, sizeof(char));
    assert(text->buffer);
    fread(text->buffer, sizeof(char), text->buffer_count, orig_file);

    return OK;
}

Errors translate_to_code_sc(Command* commands, int commands_count, Text* text, Code_SC* codesc)
{
    Text_verif(text);
    assert(commands);
    assert(commands_count > 0);
    assert(codesc);

    Command cur_command = {};

    int counter = 0;
    int received = sscanf(text->words[counter++], "%s", cur_command.name);

    printf("cur_command.name = %s\n", cur_command.name);

    int ip = 0;

    while (received > 0)
    {
        get_command_by_name(commands, commands_count, &cur_command);

        if (codesc->length + 1 + cur_command.args_count >= codesc->capacity)
            Code_SC_realloc(codesc);

        printf("start of while counter = %d cur_command.name = %s cur_command.args_count = %d cur_command.number = %d\n",
               counter, cur_command.name, cur_command.args_count, cur_command.number);

        if (counter + 1 + cur_command.args_count > text->words_count)
        {
            printf("ERROR! counter = %d cur_command.args_count = %d > text->words_count = %d\nBye!\n",
                   counter, cur_command.args_count, text->words_count);
            break;
        }
        printf("1 ip = %d codesc->code[ip] [%p] = %d codesc->length = %d\n", ip, codesc->code+ip, codesc->code[ip], codesc->length);

        codesc->code[ip++] = cur_command.number;
        codesc->length++;

        printf("2 ip = %d codesc->code[ip-1] [%p] = %d codesc->length = %d\n", ip, codesc->code+ip-1, codesc->code[ip-1], codesc->length);

        for (int i = 0; i < cur_command.args_count; i++)
        {
            printf("in for\n");
            int cur_numb = -1;
            received = sscanf(text->words[counter++], "%d", &cur_numb);
            if (!received)
            {
                printf("Error in sscanf. Sorry, Bye!\n");
                assert(0);
                return ERR;
            }
            codesc->code[ip++] = cur_numb;
            codesc->length++;
        }
        received = sscanf(text->words[counter++], "%s", cur_command.name);

        printf("end of while counter = %d cur_command.name = %s cur_command.args_count = %d cur_command.number = %d received = %d codesc->length = %d\n",
               counter, cur_command.name, cur_command.args_count, cur_command.number, received, codesc->length);
    }
    return OK;
}

Errors write_in_file(char* compiled_path, char* compiled_path_bin, Code_SC* codesc)
{
    assert(compiled_path);
    Code_SC_verif(codesc);

    FILE *compiled_file = fopen(compiled_path, "wb");
    if (!compiled_file)
    {
        printf("Compiled file is null\nBye!\n");
        return ERR;
    }

    int returned = -1;
    for (int i = 0; i < codesc->length; i++)
    {
        returned = fprintf(compiled_file, "%d ", codesc->code[i]);
        if (returned < 0)
        {
            printf("Error in writing in compiled_file. Sorry, Bye!\n");
            fclose(compiled_file);
            return ERR;
        }
    }
    printf("Written in compiled_file!\n");
    fclose(compiled_file);
    printf("compiled_file is closed!\n");

    FILE *compiled_file_bin = fopen(compiled_path_bin, "wb");
    if (!compiled_file_bin)
    {
        printf("Compiled file bin is null\nBye!\n");
        return ERR;
    }

    returned = fwrite(codesc->code, sizeof(int), codesc->length, compiled_file_bin);

    if (returned != codesc->length)
    {
        printf("Error in writing in compiled_file_bin. Sorry, Bye!\n");
        fclose(compiled_file_bin);
        return ERR;
    }

    printf("Written in compiled_file_bin!\n");
    fclose(compiled_file_bin);
    printf("compiled_file_bin is closed!\n");

    return OK;
}

Errors get_command_by_name(Command* commands, int commands_count, Command* command)
{
    assert(commands);
    assert(commands_count > 0);
    assert(command);
    assert(command->name);

    for (int i = 0; i < commands_count; i++)
    {
        if (!strcmp(command->name, commands[i].name))
        {
            command->name_length = commands[i].name_length;
            command->number = commands[i].number;
            command->args_count = commands[i].args_count;
        }
    }

    if (!(command->name_length))
    {
        printf("command->name_length = %d\n", command->name_length);
        printf("There is no name_length of command or no command was found. Sorry, Bye!\n");
        assert(0);
    }

    return OK;
}

Errors print_str(char* str)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++)
        printf("%c(%d)", str[i], str[i]);
    return OK;
}

Errors get_commands(Command** arr, int* length)
{
    assert(arr);
    assert(*arr);
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
    *arr = (Command*)realloc(*arr, (*length)*sizeof(Command));
    assert(*arr);
    for (int i = 0; i < *length; i++)
    {
        if (!fscanf(cmds_file, "\n%s", (*arr)[i].name))
        {
            printf("Error! Can't read command's name!\n");
            fclose(cmds_file);
            return ERR;
        }
        (*arr)[i].name_length = strlen((*arr)[i].name);
        if (!fscanf(cmds_file, "%d", &((*arr)[i].number)))
        {
            printf("Error! Can't read command's number!\n");
            fclose(cmds_file);
            return ERR;
        }
        if (!fscanf(cmds_file, "%d", &((*arr)[i].args_count)))
        {
            printf("Error! Can't read command's args count!\n");
            fclose(cmds_file);
            return ERR;
        }
    }

    for (int i = 0; i < *length; i++)
    {
        printf("commands[%d] = %s = %d args_count = %d name_length = %d\n",
               i, (*arr)[i].name, (*arr)[i].number, (*arr)[i].args_count, (*arr)[i].name_length);
    }

    fclose(cmds_file);
    return OK;
}

Errors Text_ctor(Text* text)
{
    text->path = NULL;
    text->buffer = NULL;
    text->buffer_count = 0;
    text->words = NULL;
    text->words_count = 0;

    return OK;
}

Errors Text_dtor(Text* text)
{
    free(text->buffer);
    free(text->words);
    text->path = NULL;
    text->buffer_count = -1;
    text->words_count = -1;

    return OK;
}

Errors Text_verif(Text *text)
{
    assert(text);
    assert(text->path);
    assert(text->buffer);
    assert(text->buffer_count >= 0);
    assert(text->words);
    assert(text->words_count >= 0);

    return OK;
}

Errors Text_get_words(Text* text)
{
    assert(text);
    assert(text->path);
    assert(text->buffer);
    assert(text->buffer_count >= 0);
    assert(text->words_count > 0);

    text->words = (char**)calloc(text->words_count, sizeof(char*));
    int counter = 0;
    text->words[counter] = text->buffer;
    counter++;
    printf("text->words_count = %d\n", text->words_count);
    for (int i = 0; i < text->buffer_count; i++)
    {
        if (text->buffer[i] == ' ' || text->buffer[i] == '\n')
        {
            text->words[counter] = text->buffer + i + 1;
            text->buffer[i] = '\0';

            counter++;
        }
    }
    for (int i = 0; i < text->words_count; i++)
    {
        printf("text->words[%d] [%p] = %s\n", i, text->words[i], text->words[i]);
    }

    return OK;
}

Errors Text_get_words_count(Text* text)
{
    assert(text);
    assert(text->path);
    assert(text->buffer);
    assert(text->buffer_count >= 0);

    int result = 0;

    for (int i = 0; i < text->buffer_count; i++)
    {
        if (text->buffer[i] == ' ' || text->buffer[i] == '\n')
            result++;
    }
    result++;
    text->words_count = result;

    return OK;
}

Errors Code_SC_ctor(Code_SC* codesc)
{
    codesc->length = 0;
    codesc->capacity = 1;
    codesc->code = (int*)realloc(codesc->code, codesc->capacity*sizeof(int));

    return OK;
}

Errors Code_SC_dtor(Code_SC* codesc)
{
    free(codesc->code);
    codesc->length = -1;
    codesc->capacity = -1;

    return OK;
}

Errors Code_SC_verif(Code_SC* codesc)
{
    assert(codesc);
    assert(codesc->code);
    assert(codesc->length >= 0);
    assert(codesc->capacity > 0);

    return OK;
}

Errors Code_SC_realloc(Code_SC* codesc)
{
    Code_SC_verif(codesc);

    printf("Code_SC_realloc happened!\n");

    int new_cap = (codesc->capacity)*2;

    codesc->code = (int*)realloc(codesc->code, new_cap*sizeof(int));
    assert(codesc);
    //memset(codesc->code + codesc->capacity, 0, new_cap-codesc->capacity);
    codesc->capacity = new_cap;

    return OK;
}

Errors print_code_sc(Code_SC* code_sc)
{
    for (int i = 0; i < code_sc->capacity; i++)
    {
        printf("code_sc->code[%d] [%p] = %d\n", i, code_sc->code + i, code_sc->code[i]);
    }

    return OK;
}

Errors print_commands(Command* commands, int commands_count)
{
    printf("Printing commands...\n");
    for (int i = 0; i < commands_count; i++)
    {
        printf("commands[%d] name = %s\n", i, commands[i].name);
    }

    return OK;
}
