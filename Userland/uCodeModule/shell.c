// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <timer.h>
#include "include/lib.h"
#include <tron.h>
#include <tests.h>
#include <loop.h>
#include <cat.h>
#include "include/filter.h"
#include <wc.h>
#include <phylo.h>

#define COMMAND_NOT_FOUND -1

#define MAX_WORDS 100
#define MAX_WORD_LENGTH 50

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
#define TEST_PRIO_COMMAND "test-prio"
#define PS_COMMAND "ps"
#define LOOP_COMMAND "loop"
#define KILL_COMMAND "kill"
#define BLOCK_COMMAND "block"
#define UNBLOCK_COMMAND "unblock"
#define NICE_COMMAND "nice"
#define CAT_COMMAND "cat"
#define FILTER_COMMAND "filter"
#define WC_COMMAND "wc"
#define SH_COMMAND "sh"
#define PHYLO_COMMAND "phylo"
#define MEM_INFO_COMMAND "mem-info"

#define AMPERSAND "&"

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
printmem          - Receives a parameter in hexadecimal format. Displays the next 32 bytes after the given memory direction\n\
ps                - Displays the current processes information\n\
loop              - Creates a process that loops infinitely\n\
kill              - Kills the process with the given pid, receives pid as argument\n\
block             - Blocks the process with the given pid, receives pid as argument\n\
unblock           - Unblocks the process with the given pid, receives pid as argument\n\
nice              - Changes the priority of the process with the given pid, receives pid and priority respectively as arguments\n\
cat               - Prints the contents of the given input\n\
filter            - Prints the contents of the file without consonants\n\
wc                - Prints the number of lines, words and characters of the given input\n\
sh                - Creates a new shell\n\
test-processes    - Runs a test, receives the number of processes to run as an argument\n\
test-mm           - Runs a test, receives a number with the maximum memory as an argument\n\
test-sync         - Runs a test, receives two numbers as arguments, the first one with the amount of iterations\n\
                    and the second one as a flag for semaphore use (0 - sync, 1 - no sync)\n\
test-prio         - Runs a test changing priorities of processes\n\
phylo             - Runs the dining philosophers problem, to add a philosopher during runtime press 'a', to remove one press 'r'\n\
mem-info          - Shows free, allocated and total heap memory\n"

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

void parseString(const char *str, char words[][MAX_WORD_LENGTH + 1], int *numWords);

void shell(int argc, char **argv);
int readBuffer(char *buf, int fd_read, int fd_write, int is_foreground);
int pipedBuffer(char *buf);
void printLine(char *str);
void helpCommand(void);
void printNewline(void);
void testInvalidOpException();
void testDivideByZeroException();

void printMem(int argc, char * argv[]);

void printInfoReg();
void printErrorMessage(char *program, char *errorMessage);
void ask_wait_pid(int is_foreground);

int increaseFontSize();
int decreaseFontSize();

extern void invalidOpcode();
extern void divideZero();

void shell(int argc, char **argv)
{
    int out = 1;

    printString(COMMAND_CHAR, GREEN);
    printf(CURSOR);
    while (out)
    {
        char str[MAX_TERMINAL_CHARS] = {0};
        char *string = str;
        inputRead(&string);
        printNewline();
        out = pipedBuffer(string);
        if (out == -1)
            out = readBuffer(string, 0, 1, 1);

        printString(COMMAND_CHAR, GREEN);
        printf(CURSOR);
    }
}

void printMem(int argc, char * argv[])
{
    char * buf = argv[0];
    int i = 0;
    while (buf[i] == ' ')
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
        printf(":\t");
        printBase(pointer[j], 16);
        printf("h\n");
    }
}

int readBuffer(char *input, int fd_read, int fd_write, int is_foreground)
{
    char words[MAX_WORDS][MAX_WORD_LENGTH + 1]; // Array to store the words
    int numWords;

    parseString(input, words, &numWords);

    char buf[MAX_WORD_LENGTH + 1];
    strncpy(buf, words[0], MAX_WORD_LENGTH);
    buf[MAX_WORD_LENGTH] = '\0';

    char *argv[MAX_WORDS];
    int numArgs = 0;

    for (int i = 1; i < numWords; i++)
    {
        argv[numArgs] = words[i];
        numArgs++;
    }

    argv[numArgs] = NULL;

    if (!strcmp(argv[numArgs - 1], AMPERSAND))
    {
        is_foreground = 0;
        argv[numArgs - 1] = NULL;
        fd_read = pipe_open(AMPERSAND);
    }

    if (!strcmp(buf, ""))
        ;
    else if (!strcmp(buf, PRINTMEM_COMMAND))
    {
        int ret = exec("printmem", &printMem, argv, fd_read, fd_write, 5, is_foreground);
        wait_pid();
        return ret;
    }
    else if (!strcmp(buf, HELP_COMMAND))
    {
        helpCommand();
    }
    else if (!strcmp(buf, CLEAR_COMMAND))
    {
        clear();
    }
    else if (!strcmp(buf, TRON_COMMAND))
    {
        clear();

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
    {
        int ret = exec("inforeg", &printInfoReg, argv, fd_read, fd_write, 5, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;

    }
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
    else if (!strcmp(buf, PS_COMMAND))
    {
        uint64_t ret = exec("ps", &ps, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;
    }
    else if (!strcmp(buf, TEST_PROCESSES_COMMAND))
    {
        char *argv[] = {"2", 0};

        int ret_pid = exec("test_processes", &test_processes, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret_pid;
    }
    else if (!strcmp(buf, TEST_MM_COMMAND))
    {
        int ret_pid = exec("test_mm", &test_mm, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret_pid;
    }
    else if (!strcmp(buf, TEST_SYNC_COMMAND))
    {
        int ret_pid = exec("test_sync", &test_sync, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret_pid;
    }
    else if (!strcmp(buf, TEST_PRIO_COMMAND))
    {
        int ret_pid = exec("test_prio", &test_prio, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret_pid;
    }
    else if (!strcmp(buf, LOOP_COMMAND))
    {
        uint64_t ret = exec("loop", &loop, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;
    }
    else if (!strcmp(buf, KILL_COMMAND))
    {
        int pid;
        pid = atoi(argv[0]);

        if (kill(pid) == pid)
            printf("\nPID %d killed successfully\n", pid);
        else
            printf("\nFailed to kill PID %d\n", pid);
    }
    else if (!strcmp(buf, BLOCK_COMMAND))
    {
        int pid;
        pid = atoi(argv[0]);

        if (block(pid) == pid)
            printf("\nPID %d blocked successfully\n", pid);
        else
            printf("\nFailed to block PID %d\n", pid);
    }
    else if (!strcmp(buf, UNBLOCK_COMMAND))
    {
        int pid;
        pid = atoi(argv[0]);

        if (unblock(pid) == pid)
            printf("\nPID %d unblocked successfully\n", pid);
        else
            printf("\nFailed to unblock PID %d\n", pid);
    }
    else if (!strcmp(buf, NICE_COMMAND))
    {
        int pid, priority;

        pid = atoi(argv[0]);
        priority = atoi(argv[1]);

        int new_pid = change_priority(pid, priority);

        if (new_pid == pid)
            printf("\nPID %d had its priority modified successfully\n", pid);
        else
            printf("\nFailed to modify priority of PID %d\n", pid);
    }
    else if (!strcmp(buf, CAT_COMMAND))
    {
        int ret = exec("cat", &cat, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;
    }
    else if (!strcmp(buf, FILTER_COMMAND))
    {
        int ret = exec("filter", &filter, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;
    }
    else if (!strcmp(buf, WC_COMMAND))
    {
        int ret = exec("wc", &wc, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;
    }
    else if (!strcmp(buf, SH_COMMAND))
    {
        int ret = exec("shell", &shell, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;
    }
    else if (!strcmp(buf, PHYLO_COMMAND))
    {
        int ret = exec("phylo", &phylo, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;
    }
    else if (!strcmp(buf, MEM_INFO_COMMAND))
    {
        int ret = exec("mem_info", &memory_info, argv, fd_read, fd_write, 1, is_foreground);
        ask_wait_pid(is_foreground);
        return ret;
    }
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

void printInfoReg()
{
    long array[REGISTER_NUM] = {0};
    long *arr = (long *)&array;
    infoReg(arr);
    char *registerNames[] = REGISTER_NAMES;
    for (int i = 0; i < REGISTER_NUM; i++)
    {
        printf("%s : ", registerNames[i]);
        printBase(arr[i], 16);
        printf("h\n");
    }
}

int increaseFontSize()
{
    return changeFontSize(INCREASE);
}

int decreaseFontSize()
{
    return changeFontSize(DECREASE);
}

int pipedBuffer(char *buf)
{
    if (!strchr(buf, '|') != 0)
        return -1;

    char *left;
    char *right;

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
        right = NULL;
        return -1;
    }

    // Allocate memory for left and copy characters before '|'
    left = malloc((delimiterPos + 1) * sizeof(char));
    if (left == NULL)
        return -1;

    strncpy(left, buf, delimiterPos);
    (left)[delimiterPos] = '\0';

    // Allocate memory for right and copy characters after '|'
    right = malloc((totalLength - delimiterPos) * sizeof(char));
    if (right == NULL) {
        free(left);
        return -1;
    }

    strncpy(right, buf + delimiterPos + 1, totalLength - delimiterPos);
    (right)[totalLength - delimiterPos - 1] = '\0';

    int fd = pipe_open("pipes");

    long leftPid = readBuffer(left, 0, fd, 0);
    if (leftPid == -1)
    {
        free(left);
        free(right);
        return 2;
    }

    yield();

    long rightPid = readBuffer(right, fd, 1, 1);
    if (rightPid == -1)
    {
        free(left);
        free(right);
        kill(leftPid);
        return 3;
    }

    free(left);
    free(right);
    pipe_close(fd);

    return 1;
}

void parseString(const char *str, char words[][MAX_WORD_LENGTH + 1], int *numWords)
{
    *numWords = 0; // Initialize the number of words to 0

    while (*str != '\0' && *numWords < MAX_WORDS)
    {
        while (*str == ' ')
        {
            str++; // Skip leading spaces
        }

        if (*str == '\0')
        {
            break; // Reached the end of the string
        }

        const char *wordStart = str; // Start of the current word

        while (*str != ' ' && *str != '\0')
        {
            str++; // Move to the next character
        }

        const char *wordEnd = str - 1; // End of the current word

        int wordLength = wordEnd - wordStart + 1;
        if (wordLength > MAX_WORD_LENGTH)
        {
            wordLength = MAX_WORD_LENGTH; // Truncate the word if it exceeds the maximum length
        }

        for (int i = 0; i < wordLength; i++)
        {
            words[*numWords][i] = wordStart[i]; // Copy the word into the array
        }

        words[*numWords][wordLength] = '\0'; // Null-terminate the word
        (*numWords)++;                       // Increment the number of words
    }
    words[*numWords][0] = '\0';
}

void ask_wait_pid(int is_foreground)
{
    if (is_foreground == 1)
    {
        wait_pid();
    }
}
