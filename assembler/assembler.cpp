#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


//cd documents\projects\vm_asm_disasm_ded2023\assembler

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

Errors compile(Command* commands, int commands_count, FILE* orig_file, FILE* compiled_file);
Errors get_commands(Command* arr, int* length);
Errors print_str(char* str);
Errors slice(char* str, char* new_str, int offset);

int main()
{
    Command commands[11] = {};
    int commands_count = 0;
    get_commands(commands, &commands_count);
    printf("commands_count = %d\n", commands_count);
    assert(commands_count);

    char* orig_path = "original_file.txt";
    FILE *orig_file = fopen(orig_path, "rb");
    if (!orig_file)
    {
        printf("Original file is null\nBye!\n");
        return 0;
    }

    char* compiled_path = "compiled_file.txt";
    FILE *compiled_file = fopen(compiled_path, "wb");
    if (!compiled_file)
    {
        printf("Compiled file is null\nBye!\n");
        return 0;
    }

    Errors err = OK;
    if (!(err = compile(commands, commands_count, orig_file, compiled_file)))
    {
        printf("Error in compiling! Sorry, Bye!\n");
        return 0;
    }

    printf("Closing files...\n");
    fclose(orig_file);
    fclose(compiled_file);
    printf("Files are closed. Bye!\n");
    return 0;
}

Errors compile(Command* commands, int commands_count, FILE* orig_file, FILE* compiled_file)
{
    assert(commands);
    assert(commands_count);
    assert(orig_file);
    assert(compiled_file);

    const int max_symbols_in_line = 50;
    const int max_symbols_in_command = 10;
    const int max_lines_in_file = 100;

    char cur_str[max_symbols_in_line] = {}; //������� ������

    if (!fgets(cur_str, max_symbols_in_line, orig_file))
    {
        printf("No lines! Bye!\n");
        return ERR;
    }

    char cur_cmd[max_symbols_in_command] = {}; //������� �������
    int received = 0;

    for (int line = 0; line < max_lines_in_file; line++)
    {
        printf("line = %d cur_str = \n", line);
        print_str(cur_str);

        received = sscanf(cur_str, "%s", cur_cmd); //��������� ������ ����� ������
        if (!received)
            return ERR;
        printf("line = %d cur_cmd = %s\n", line, cur_cmd);

        for (int j = 0; j < commands_count; j++) //���� ������������ ����� �������� ������
        {
            printf("Seeking for this command... j = %d\n", j);
            if (!strcmp(cur_cmd, commands[j].name)) //����� ������������ �������
            {
                printf("Assembler is replacing \"%s\" with \"%d\"\n", commands[j].name, commands[j].number);

                char new_cmd[10] = {}; //������ ��������� ������������� ������ �������
                if (commands[j].args_count == 0)
                {
                    sprintf(new_cmd, "%d\n", commands[j].number);
                    fwrite(new_cmd, sizeof(char), strlen(new_cmd), compiled_file); //���������� � ���� ����� �������

                    break;
                }
                sprintf(new_cmd, "%d", commands[j].number);

                fwrite(new_cmd, sizeof(char), strlen(new_cmd), compiled_file); //���������� � ���� ����� �������

                char new_cur_str[max_symbols_in_line] = {};
                slice(cur_str, new_cur_str, commands[j].name_length);

                fwrite(new_cur_str, sizeof(char), strlen(new_cur_str), compiled_file); //���������� � ���� ���������� ����� ������

                printf("new_cur_str = \n");
                print_str(new_cur_str);

                break;
            }
        }

        if (!fgets(cur_str, max_symbols_in_line, orig_file))
            break;
    }
    printf("compile() happened!");
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
    printf("sliced!");
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
