#ifndef _SYSCALL_MANAGER_H_
#define _SYSCALL_MANAGER_H_

#include <timer.h>
#include <stdint.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <interrupts.h>
#include <memoryManager.h>
#include <process.h>
#include <pipe.h>
#include <semaphore.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2


#define REGISTER_NUM 17
#define REGISTER_NAMES {"RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"}


uint64_t sys_write(uint8_t fd, char *string, uint64_t n, Color color);
uint64_t sys_read(uint8_t fd, char * buf, uint32_t count);
uint64_t sys_writeAt(uint16_t x, uint16_t y, char *string, Color color);
uint64_t sys_time();
uint64_t sys_date();
uint64_t sys_clearScreen();
uint64_t sys_drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);
uint64_t sys_beep(uint32_t frequency);
uint64_t sys_widthScr();
uint64_t sys_heightScr();
uint64_t sys_wait(uint32_t millis);
uint64_t sys_timedRead(uint8_t fd, char * buf, uint32_t count, uint32_t millis);
uint64_t sys_inforeg(uint64_t * array);
uint64_t sys_changeFontSize(uint32_t dif);
void * sys_malloc(uint64_t memSize);
void sys_free(void * ptr);
uint64_t sys_exec(char * name, void * program, char ** argv, uint64_t read_fd, uint64_t write_fd, uint64_t priority);
uint64_t sys_pipe_open(char* name);
uint64_t sys_pipes_info();
uint64_t sys_pipe_close(int id);
uint64_t sys_sem_open(char *name, int value);
uint64_t sys_sem_close(char *name);
uint64_t sys_sem_post(char *name);
uint64_t sys_sem_wait(char *name);
uint64_t sys_sem_info(int idx, p_sem buffer);
uint64_t sys_sem_count();
uint64_t sys_get_pid();
uint64_t sys_kill(uint64_t pid);
uint64_t sys_change_priority(uint64_t pid, uint64_t priority);
uint64_t sys_block(uint64_t pid);
uint64_t sys_unblock(uint64_t pid);
void sys_yield(void);
void sys_ps(void);

#endif
