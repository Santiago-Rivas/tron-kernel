GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

GLOBAL syscallINTHandler

GLOBAL registers

EXTERN exceptionDispatcher
EXTERN syscallDispatcher

EXTERN saveKey
EXTERN timer_handler
EXTERN scheduler

EXTERN sys_write
EXTERN sys_read
EXTERN sys_writeAt
EXTERN sys_clearScreen
EXTERN sys_wait
EXTERN sys_time
EXTERN sys_date
EXTERN sys_heightScr
EXTERN sys_widthScr
EXTERN sys_timedRead
EXTERN sys_drawRectangle
EXTERN sys_changeFontSize
EXTERN sys_inforeg
EXTERN sys_beep
EXTERN sys_malloc
EXTERN sys_free
EXTERN sys_exec
EXTERN sys_pipe_open
EXTERN sys_pipes_info
EXTERN sys_pipe_close
EXTERN sys_sem_open
EXTERN sys_sem_close
EXTERN sys_sem_post
EXTERN sys_sem_wait
EXTERN sys_sem_info
EXTERN sys_sem_count
EXTERN sys_get_pid
EXTERN sys_kill
EXTERN sys_change_priority
EXTERN sys_block
EXTERN sys_unblock
EXTERN sys_yield
EXTERN sys_ps
EXTERN sys_wait_pid
EXTERN sys_mem_info


SECTION .text

%macro pushState 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popState 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro


%macro snapshot 0
    cli
    mov [registers+1*8], rax
    mov [registers+2*8], rbx
    mov [registers+3*8], rcx
    mov [registers+4*8], rdx
    mov [registers+5*8], rsi
    mov [registers+6*8], rdi
    mov [registers+7*8], rbp

    ; rsp
    mov rax, rsp
    add rax, 120
    mov [registers+8*8], rax

    mov [registers+9*8], r8
    mov [registers+10*8], r9
    mov [registers+11*8], r10
    mov [registers+12*8], r11
    mov [registers+13*8], r12
    mov [registers+14*8], r13
    mov [registers+15*8], r14
    mov [registers+16*8], r15

    ; rip
    mov rax, [rsp+15*8]
    mov [registers], rax

    mov byte [registersSaved], 1

    sti
%endmacro


%macro exceptionHandler 1
    pushState

    snapshot

    mov rdi, %1 ; pasaje de parametro
    call exceptionDispatcher

    popState
    iretq
%endmacro


_hlt:
    sti
    hlt
    ret

_cli:
    cli
    ret


_sti:
	sti
	ret

picMasterMask:
    push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = 16 bit mask
    out	0A1h, al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
    pushState
    call timer_handler
    mov rdi, rsp
    call scheduler
    mov rsp, rax
    mov al, 20h
    out 20h, al

    popState
    iretq


keyPressed:
    mov rax, 0
.inicio:
    in al, 64h
    and al, 0x01
    je .inicio
    in al, 60h
    ret

;Keyboard
_irq01Handler:
    pushState
    call keyPressed

    cmp al, 0b111000
    jne .end
    popState
    snapshot
    pushState
    jmp .skip_save

.end:
    mov rdi, rax
    call saveKey
.skip_save:
    mov al, 20h
    out 20h, al
    popState
    iretq


; Zero Division Exception
 _exception0Handler:
 	exceptionHandler 0

; Invalid Opcode Exception
 _exception6Handler:
 	exceptionHandler 6

syscallINTHandler:
    ; cli
    ; mov rcx, r10
    ; mov r9, rax ; <== ??
    ; sti

    cmp rax, 0x00
    je .write

    cmp rax, 0x01
    je .read

    cmp rax, 0x02
    je .writeAt

    cmp rax, 0x03
    je .clearScreen

    cmp rax, 0x04
    je .wait

    cmp rax, 0x05
    je .time

    cmp rax, 0x06
    je .date

    cmp rax, 0x07
    je .getScreenHeight

    cmp rax, 0x08
    je .getScreenWidth

    cmp rax, 0x09
    je .timedRead

    cmp rax, 0x0A
    je .drawRectangle

    cmp rax, 0x0B
    je .changeFontsize

    cmp rax, 0x0C
    je .inforeg

    cmp rax, 0x0D
    je .beep

    cmp rax, 0x0E
    je .malloc

    cmp rax, 0x0F
    je .free

    cmp rax, 0x10
    je .exec

    cmp rax, 0x11
    je .pipe_open

    cmp rax, 0x12
    je .pipes_info

    cmp rax, 0x13
    je .pipe_close

    cmp rax, 0x14
    je .sem_open

    cmp rax, 0x15
    je .sem_close

    cmp rax, 0x16
    je .sem_post

    cmp rax, 0x17
    je .sem_wait

    cmp rax, 0x18
    je .sem_info

    cmp rax, 0x19
    je .sem_count

    cmp rax, 0x1A
    je .get_pid

    cmp rax, 0x1B
    je .kill

    cmp rax, 0x1C
    je .block

    cmp rax, 0x1D
    je .unblock

    cmp rax, 0x1E
    je .yield

    cmp rax, 0x1F
    je .change_priority

    cmp rax, 0x20
    je .ps

    cmp rax, 0x21
    je .wait_pid

    cmp rax, 0x22
    je .mem_info

    jmp .end

.write:
    mov rcx, r10
    call sys_write
    jmp .end

.read:
    call sys_read
    jmp .end

.writeAt:
    mov rcx, r10
    call sys_writeAt
    jmp .end
	
.clearScreen:
    call sys_clearScreen
    jmp .end

.wait:
    call sys_wait
    jmp .end

.time:
	call sys_time
	jmp .end
	
.date:
	call sys_date
	jmp .end

.getScreenHeight:
	call sys_heightScr
	jmp .end

.getScreenWidth:
	call sys_widthScr
	jmp .end

.timedRead:
	call sys_timedRead
	jmp .end

.drawRectangle:
        mov rcx, r10
	call sys_drawRectangle
	jmp .end

.changeFontsize:
	call sys_changeFontSize
	jmp .end

.inforeg:
        call sys_inforeg
        jmp .end

.beep:
        call sys_beep
        jmp .end

.malloc:
        call sys_malloc
        jmp .end

.free:
        call sys_free
        jmp .end

.exec:
        push r11
        call sys_exec
        pop r11
        jmp .end

.pipe_open:
        call sys_pipe_open
        jmp .end

.pipes_info:
        call sys_pipes_info
        jmp .end

.pipe_close:
        call sys_pipe_close
        jmp .end

.sem_open:
        call sys_sem_open
        jmp .end

.sem_close:
        call sys_sem_close
        jmp .end

.sem_post:
        call sys_sem_post
        jmp .end

.sem_wait:
        call sys_sem_wait
        jmp .end

.sem_info:
        mov rsi, r10
        call sys_sem_info
        jmp .end

.sem_count:
        call sys_sem_count
        jmp .end

.get_pid:
        call sys_get_pid
        jmp .end

.kill:
        call sys_kill
        jmp .end

.block:
        call sys_block
        jmp .end

.unblock:
        call sys_unblock
        jmp .end

.yield:
        call sys_yield
        jmp .end

.change_priority:
        call sys_change_priority
        jmp .end

.ps:
        call sys_ps
        jmp .end

.wait_pid:
        call sys_wait_pid
        jmp .end

.mem_info:
        call sys_mem_info
        jmp .end

.end:
    push rax
    mov al, 20h
    out 20h, al
    pop rax
    iretq

haltcpu:
	cli
	hlt
	ret

SECTION .bss
    registersSaved resb 1
    aux resq 1
    registers resq 17
