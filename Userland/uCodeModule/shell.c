#include <syscalls.h>
#include <color.h>
#include <timer.h>
#include <lib.h>
#include <tron.h>
#include <tests.h>
#include <loop.h>
#include <cat.h>

#define COMMAND_CHAR "$> "
#define CURSOR "|"
#define BACKSPACE '\b'
#define MAX_KBD_BUF 55

#define COMMAND_NOT_FOUND -1

#define SHELL_NAME "Shell"

#define HELP_COMMAND "help"
#define CLEAR_COMMAND "clear"
#define EXIT_COMMAND "exit"
#define TRON_COMMAND "tron"
#define DIVIDE_BY_ZERO "div-zero"
#define INVALID_OP "invalid-op"
#define DATE_COMMAND "date"
#define TIME_COMMAND "time"
#define DATE_TIME_COMMAND "datetime"
#define INC_FONT_SIZE_COMMAND "inc-font"
#define DEC_FONT_SIZE_COMMAND "dec-font"
#define INFOREG_COMMAND "inforeg"
#define PRINTMEM_COMMAND "printmem"
#define TEST_PROCESSES_COMMAND "test-processes"
#define TEST_MM_COMMAND "test-mm"
#define TEST_SYNC_COMMAND "test-sync"
#define LOOP_COMMAND "loop"
#define KILL_COMMAND "kill"
#define BLOCK_COMMAND "block"
#define UNBLOCK_COMMAND "unblock"
#define NICE_COMMAND "nice"
#define CAT_COMMAND "cat"

#define MAX_TERMINAL_CHARS 124 // 124 = (1024/8) - 4 (number of characters that fit in one line minus the command prompt and cursor characters)
#define HELP_MESSAGE "HELP:\n\
The following is a list of the different commands the shell can interpret and a short description of what they do:\n\
\
help              - Displays the available commands the shell can interpret and a short description of them\n\
clear             - Clears screen allowing more text to be written\n\
exit              - Exit the shell returning to kernel space. This command will shut down the program\n\
tron              - Launches tron game\n\
div-zero          - Calls function that showcases the Divide By Zero Exception\n\
invalid-op        - Calls function that showcases the Invalid Opcode Exception\n\
date              - Displays date\n\
time              - Displays time\n\
datetime          - Displays date and time\n\
inc-font          - Increases font size\n\
dec-font          - Decreases font size\n\
inforeg           - Displays the contents of all the registers at a given time.\n\
                    To save registers press and release the CTRL key.\n\
                    If the command is called before pressing CTRL at least once,\n\
                    the registers will appear as if they have the value 0\n\
printmem           - Receives a parameter in hexadecimal. Displays the next 32 bytes after the given memory direction given\n"

#define INCREASE 1
#define DECREASE -1

#define REGISTER_NUM 17
#define REGISTER_NAMES                                                                                                        \
    {                                                                                                                         \
        "RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15" \
    }

#define PRINT_BYTES 32

#define COMMAND_NOT_FOUND_MESSAGE "Command not found"
#define INCREASE_FONT_FAIL "Maximum font size reached"
#define DECREASE_FONT_FAIL "Minimum font size reached"

#define NEWLINE "\n"

void shell(int argc, char **argv);
void bufferRead(char **buf);
void readNumber(char **buf);
int readBuffer(char *buf, int fd_read, int fd_write);
int pipedBuffer(char *buf);
void printLine(char *str);
void helpCommand(void);
void printNewline(void);
void testInvalidOpException();
void testDivideByZeroException();

void printInforeg();
void printErrorMessage(char *program, char *errorMessage);

int increaseFontSize();
int decreaseFontSize();

extern void invalidOpcode();
extern void divideZero();

void shell(int argc, char **argv)
{
    int out = 1;

    while (out)
    {
        char str[MAX_TERMINAL_CHARS] = {0};
        char *string = str;
        bufferRead(&string);
        printf("\b");
        printNewline();
        out = pipedBuffer(string);
        if (out == -1)
            out = readBuffer(string, 0, 1);
    }
}

void bufferRead(char **buf)
{
    int c = 1;
    int i = 0;
    (*buf)[i] = 0;
    printString(COMMAND_CHAR, GREEN);
    printf(CURSOR);
    while (c != 0 && i < MAX_TERMINAL_CHARS - 1)
    {
        c = getChar();
        if (c == BACKSPACE)
        {
            if (i > 0)
            {
                (*buf)[--i] = 0;
                printf("\b");
                printf("\b");
                printf(CURSOR);
            }
        }
        else if (c >= ' ')
        {
            (*buf)[i++] = (char)c;
            (*buf)[i] = 0;
            printf("\b");
            printf(*buf + i - 1);
            printf(CURSOR);
        }
    }
}

void printmem(char *buf)
{
    int i = 0;
    while (buf[i] != 0 && buf[i] == ' ')
        i++;
    if (buf[i] == 0)
    {
        printErrorMessage(PRINTMEM_COMMAND, "No argument received");
        printNewline();
        return;
    }
    if (buf[i] == '0')
        i++;
    else
    {
        printErrorMessage(PRINTMEM_COMMAND, "Argument must be a hexadecimal value");
        printNewline();
        return;
    }
    if (buf[i] == 'x')
        i++;
    else
    {
        printErrorMessage(PRINTMEM_COMMAND, "Argument must be a hexadecimal value");
        printNewline();
        return;
    }
    if (buf[i] == 0)
    {
        printErrorMessage(PRINTMEM_COMMAND, "Argument must be a hexadecimal value");
        printNewline();
        return;
    }
    long long accum = 0;
    for (; buf[i] != 0; i++)
    {
        if (buf[i] >= 'a' && buf[i] <= 'f')
            accum = 16 * accum + buf[i] - 'a' + 10;
        else if (buf[i] >= '0' && buf[i] <= '9')
            accum = 16 * accum + buf[i] - '0';
        else
        {
            printErrorMessage(PRINTMEM_COMMAND, "Argument must be a hexadecimal value");
            printNewline();
            return;
        }
    }
    long long *pointer = (long long *)accum;
    if (0xFFFFFFFF - accum <= 32)
    {
        printErrorMessage(PRINTMEM_COMMAND, "Input number is too big, limit is 0xFFFFFFDE");
        printNewline();
        return;
    }
    for (long long j = 0; j < PRINT_BYTES && accum + j + 2 < 0xFFFFFFFFFFFFFFFF; j++)
    {
        printBase((int)j, 10);
        printf("d\n");
        printBase(pointer[j], 2);
        printf("b\n");
    }
}

int readBuffer(char *buf, int fd_read, int fd_write)
{
    int l;
    if (!strcmp(buf, ""))
        ;
    else if (!strncmp(buf, PRINTMEM_COMMAND, l = strlen(PRINTMEM_COMMAND)))
    {
        if (buf[l] != ' ' && buf[l] != 0)
        {
            printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return COMMAND_NOT_FOUND;
        }
        printmem(buf + l);
    }
    else if (!strcmp(buf, HELP_COMMAND))
        helpCommand();
    else if (!strcmp(buf, CLEAR_COMMAND))
        clear();
    else if (!strcmp(buf, TRON_COMMAND))
    {
        clear();

        // Lower font size
        int count = 0;
        for (; decreaseFontSize(); count++)
            ;

        mainTron(); // Call tron game

        // Reset font size to previous value
        for (int i = 0; i < count; i++)
            increaseFontSize();
        clear();
    }
    else if (!strcmp(buf, DATE_COMMAND))
    {
        char str[MAX_TERMINAL_CHARS] = {0};
        char *string = str;
        getDateFormat(string);
        printf("%s\n", string);
    }
    else if (!strcmp(buf, TIME_COMMAND))
    {
        char str[MAX_TERMINAL_CHARS] = {0};
        char *string = str;
        getTimeFormat(string);
        printf("%s\n", string);
    }
    else if (!strcmp(buf, DATE_TIME_COMMAND))
    {
        char str[MAX_TERMINAL_CHARS] = {0};
        char *string = str;
        getDateAndTime(string);
        printf("%s\n", string);
    }
    else if (!strcmp(buf, INC_FONT_SIZE_COMMAND))
    {
        int check = increaseFontSize();
        if (!check)
        {
            printErrorMessage(buf, INCREASE_FONT_FAIL);
            printNewline();
        }
        else
            clear();
    }
    else if (!strcmp(buf, DEC_FONT_SIZE_COMMAND))
    {
        int check = decreaseFontSize();
        if (!check)
        {
            printErrorMessage(buf, DECREASE_FONT_FAIL);
            printNewline();
        }
        else
            clear();
    }
    else if (!strcmp(buf, INFOREG_COMMAND))
        printInforeg();
    else if (!strcmp(buf, DIVIDE_BY_ZERO))
    {
        testDivideByZeroException();
        return 0;
    }
    else if (!strcmp(buf, INVALID_OP))
    {
        testInvalidOpException();
        return 0;
    }
    else if (!strcmp(buf, EXIT_COMMAND))
    {
        clear();
        return 0;
    }
    else if (!strcmp(buf, TEST_PROCESSES_COMMAND))
    {
        char *argv[] = {"2", 0};

        int ret_pid = exec("test_processes", &test_processes, argv, fd_read, fd_write, 0);

        return ret_pid;
        // test_processes(1,argv);
    }
    else if (!strcmp(buf, TEST_MM_COMMAND))
    {
        char *argv[] = {"100000000000", 0};
        int ret_pid = exec("test_mm", &test_mm, argv, fd_read, fd_write, 1);
        return ret_pid;

        // test_processes(1,argv);
    }
    else if (!strcmp(buf, TEST_SYNC_COMMAND))
    {
        char *argv[] = {"20", "5", 0};
        int ret_pid = exec("test_sync", &test_sync, argv, fd_read, fd_write, 1);
        return ret_pid;
    }
    else if (!strcmp(buf, LOOP_COMMAND))
    {
        char *argv[] = {"Hola", "Como Estas", NULL};
        int ret_pid = exec("loop", &loop, argv, fd_read, fd_write, 1);
        return ret_pid;
    }
    else if (!strcmp(buf, KILL_COMMAND))
    {
        // TODO: Ojo que no vuelve a la shell si la matamos

        int pid;
        printf("Enter PID:  ");
        char * pidString = (char *) malloc(10);
        readNumber(&pidString);
        pid = atoi(pidString);

        if (kill(pid) == pid)
            printf("\nPID %d killed successfully\n", pid);
        else
            printf("\nFailed to kill PID %d\n", pid);
    }
    else if (!strcmp(buf, BLOCK_COMMAND))
    {
        int pid;
        printf("Enter PID:  ");
        char * pidString = (char *) malloc(10);
        readNumber(&pidString);
        pid = atoi(pidString);

        if (block(pid) == pid)
            printf("\nPID %d blocked successfully\n", pid);
        else
            printf("\nFailed to block PID %d\n", pid);
    }
    else if (!strcmp(buf, UNBLOCK_COMMAND))
    {
        int pid;
        printf("Enter PID:  ");
        char * pidString = (char *) malloc(10);
        readNumber(&pidString);
        pid = atoi(pidString);

        if (unblock(pid) == pid)
            printf("\nPID %d unblocked successfully\n", pid);
        else
            printf("\nFailed to unblock PID %d\n", pid);
    }
    else if (!strcmp(buf, NICE_COMMAND))
    {
        int pid, priority;

        printf("Enter PID:  ");
        char * pidString = (char *) malloc(10);
        readNumber(&pidString);
        pid = atoi(pidString);
        
        char * priorityString = (char *) malloc(10);
        do
        {
            printf("\nPriorities are: 0 (low), 1 (medium) or 2 (high)");
            printf("\nEnter priority number:  ");
            readNumber(&priorityString);
            priority = atoi(priorityString);
        } while (priority < 0 || priority > 2);

        printf("\nSelected PID: %d\n", pid);
        printf("Selected priority: %d\n", priority);

        int new_pid = change_priority(pid, priority);
        printf("New PID: %d\n", new_pid);

        if (new_pid == pid)
            printf("\nPID %d had its priority modified successfully\n", pid);
        else
            printf("\nFailed to modify priority of PID %d\n", pid);
    }
    /*
    else if (!strcmp(buf, CAT_COMMAND))
    {
        char *argv[] = {"2", 0};
        
        //sem_open("cat_sem", 1);
        //sem_wait("cat_sem");
        exec("cat", &cat, argv, fd_read, fd_write, 1);
        //sem_post("cat_sem");
        //sem_close("cat_sem");
    }*/
    else
    {
        printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
        printNewline();
        return COMMAND_NOT_FOUND;
    }
    return 1;
}

void printErrorMessage(char *program, char *errorMessage)
{
    printString(SHELL_NAME, GREEN);
    printf(" : %s : ", program);
    printError(errorMessage);
}

void helpCommand()
{
    printf(HELP_MESSAGE);
    printNewline();
}

void printNewline()
{
    printString(NEWLINE, WHITE);
}

void testDivideByZeroException()
{
    divideZero();
}

void testInvalidOpException()
{
    invalidOpcode();
}

void printInforeg()
{
    long array[REGISTER_NUM] = {0};
    long *arr = (long *)&array;
    sys_inforeg(arr);
    char *registerNames[] = REGISTER_NAMES;
    for (int i = 0; i < REGISTER_NUM; i++)
    {
        printf("%s : ", registerNames[i]);
        printBase(arr[i], 2);
        printf("b\n");
    }
}

int increaseFontSize()
{
    return sys_changeFontSize(INCREASE);
}

int decreaseFontSize()
{
    return sys_changeFontSize(DECREASE);
}

int pipedBuffer(char *buf)
{
    if (!strchr(buf, '|') != 0)
        return -1;

    char *left;
    // int lengthLeft;
    char *right;
    // int lengthRight;

    int totalLength = strlen(buf);

    // Find the position of '|'
    int delimiterPos = -1;
    for (int i = 0; i < totalLength; i++)
    {
        if (buf[i] == '|')
        {
            delimiterPos = i;
            break;
        }
    }

    // If delimiter '|' not found, return NULL for both substrings
    if (delimiterPos == -1)
    {
        left = NULL;
        // lengthLeft = 0;
        right = NULL;
        // lengthRight = 0;
        return -1;
    }

    // Allocate memory for left and copy characters before '|'
    left = malloc((delimiterPos + 1) * sizeof(char));
    strncpy(left, buf, delimiterPos);
    (left)[delimiterPos] = '\0';
    // lengthLeft = delimiterPos;

    // Allocate memory for right and copy characters after '|'
    right = malloc((totalLength - delimiterPos) * sizeof(char));
    strncpy(right, buf + delimiterPos + 1, totalLength - delimiterPos);
    (right)[totalLength - delimiterPos - 1] = '\0';
    // lengthRight = totalLength - delimiterPos - 1;

    // printf("Substring 1: %s\n", left);
    // printf("Substring 1 length: %d\n", lengthLeft);
    // printf("Substring 2: %s\n", right);
    // printf("Substring 2 length: %d\n", lengthRight);

    int fd = pipe_open("pipes");

    long leftPid = readBuffer(left, 0, fd);
    if (leftPid == -1)
        return 2;

    long rightPid = readBuffer(right, fd, 1);
    if (rightPid == -1)
    {
        sys_kill(leftPid);
        return 3;
    }

    return 1;
}

void readNumber(char **buf)
{
    int c = 1, i = 0;
    while (c != 0 && i < MAX_TERMINAL_CHARS - 1)
    {
        c = getChar();
        if (c == BACKSPACE)
        {
            if (i > 0)
            {
                (*buf)[--i] = 0;
                printf("\b");
                printf("\b");
                printf(CURSOR);
            }
        }
        else if (c >= ' ')
        {
            (*buf)[i++] = (char)c;
            (*buf)[i] = 0;
            printf("\b");
            printf(*buf + i - 1);
            printf(CURSOR);
        }
    }
    printf("\b");
}