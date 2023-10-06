#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enum Errors{
    OK  = 0,
    ERR = 1
};
struct Command{
    int name_length;
    char* name;
    int number;
    int args_count;
};

Errors decompile(Command* commands, int commands_length, FILE* compiled_file, FILE* decompiled_file);
Errors slice(char* str, char* new_str, int offset);
Errors get_commands(Command* arr, int* length);
Errors print_str(char* str);

int main()
{
    Command commands[11] = {};
    int commands_length = 0;
    get_commands(commands, &commands_length);
    printf("commands_length = %d\n", commands_length);
    assert(commands_length);

    char* compiled_path = "compiled_file.txt";
    FILE *compiled_file = fopen(compiled_path, "rb");
    if (!compiled_file)
    {
        printf("Compiled file is null\nBye!\n");
        return 0;
    }

    char* decompiled_path = "decompiled_file.txt";
    FILE *decompiled_file = fopen(decompiled_path, "wb");
    if (!decompiled_file)
    {
        printf("Decompiled file is null\nBye!\n");
        return 0;
    }

    Errors err = OK;
    if (!(err = decompile(commands, commands_length, compiled_file, decompiled_file)))
    {
        printf("Error in decompiling! Sorry, Bye!\n");
        return 0;
    }

    printf("Closing files...\n");
    fclose(compiled_file);
    fclose(decompiled_file);
    printf("Files are closed. Bye!\n");
    return 0;
}

Errors decompile(Command* commands, int commands_count, FILE* compiled_file, FILE* decompiled_file)
{
    assert(commands);
    assert(commands_count);
    assert(compiled_file);
    assert(decompiled_file);

    const int max_symbols_in_line = 50;
    const int max_symbols_in_command = 10;
    const int max_lines_in_file = 100;

    char cur_str[max_symbols_in_line] = {}; //текущая строка

    if (!fgets(cur_str, max_symbols_in_line, compiled_file))
    {
        printf("No lines! Bye!\n");
        return ERR;
    }

    int cur_cmd = -1; //текущая команда
    int received = 0;

    for (int line = 0; line < max_lines_in_file; line++)
    {
        printf("line = %d cur_str = \n", line);
        print_str(cur_str);

        received = sscanf(cur_str, "%d", &cur_cmd); //считываем номер команды
        if (!received)
            return ERR;

        printf("line = %d cur_cmd = %d\n", line, cur_cmd);

        for (int j = 0; j < commands_count; j++) //ищем соответствие среди названий команд
        {
            printf("Seeking for this command... j = %d\n", j);
            if (cur_cmd == commands[j].number) //нашли используемую команду
            {
                printf("Disassembler is replacing \"%d\" with \"%s\"\n", commands[j].number, commands[j].name);

                //char new_cmd[10] = {}; //хранит строковое представление номера команды
                /*if (commands[j].args_count == 0)
                {
                    sprintf(new_cmd, "%d\n", commands[j].number);
                    fwrite(new_cmd, sizeof(char), strlen(new_cmd), compiled_file); //записываем в файл номер команды

                    break;
                }*/

                //sprintf(new_cmd, "%d", commands[j].number);

                fwrite(commands[j].name, sizeof(char), commands[j].name_length, decompiled_file); //записываем в файл название команды

                char new_cur_str[max_symbols_in_line] = {};
                int number_length = 1; //нужно написать функцию которая это ищет
                slice(cur_str, new_cur_str, number_length);

                fwrite(new_cur_str, sizeof(char), strlen(new_cur_str), decompiled_file); //записываем в файл оставшуюся часть строки

                printf("new_cur_str = \n");
                print_str(new_cur_str);

                break;
            }
        }
        char* reterned  = fgets(cur_str, max_symbols_in_line, compiled_file);
        printf("reterned = %s\n", reterned);
        if (!reterned)
            return OK;
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

Errors slice(char* str, char* new_str, int offset)
{
    assert(str);
    assert(new_str);

    const int len = (const int)strlen(str);
    new_str[0] = ' ';
    int i = 1;
    printf("slice() ...\n");
    printf("len = %d\n", len);
    printf("offset = %d\n", offset);
    printf("new_str[0] = %c(%d)\n", new_str[0], new_str[0]);
    printf("new_str[1] = %c(%d)\n", new_str[1], new_str[1]);
    printf("new_str[2] = %c(%d)\n", new_str[2], new_str[2]);
    printf("str[0] = %c(%d)\n", str[0], str[0]);
    printf("str[1] = %c(%d)\n", str[1], str[1]);
    printf("str[2] = %c(%d)\n", str[2], str[2]);

    while ((i+offset) < len)
    {
        new_str[i] = str[i+offset];
        printf("i = %d, new_str[i] = %c(%d)\n", i, new_str[i], new_str[i]);
        i++;
    }
    printf("sliced!\n");
    return OK;
}

Errors get_commands(Command* arr, int* length)
{
    assert(arr);
    assert(length);
    Command result[11]= {{2, "in", 1, 0},
                        {4, "push", 2, 1},
                        {3, "out", 3, 0},
                        {3, "hlt", 0, 0},
                        {3, "add", 4, 0},
                        {3, "sub", 5, 0},
                        {4, "mult", 6, 0},
                        {3, "div", 7, 0},
                        {4, "sqrt", 8, 0},
                        {3, "sin", 9, 0},
                        {3, "cos", 10, 0}};
    *length = 11;
    for (int i = 0; i < *length; i++)
    {
        arr[i] = result[i];
        printf("commands[%d] = %s = %d\n", i, arr[i].name, arr[i].number);
    }
    *length = 11;

    return OK;
}
