/*
 * must alloc task object at main function ;
 */
#pragma once
#define TASKS_MAX_NR 4
#define TASK_T_OFFSET_FUNCTION 0
#define TASK_T_OFFSET_STACK 4
#define TASK_T_OFFSET_PSP 4*( 256 + 1)  /* 1028 */
#define TASK_STACK_SIZE 256   /* item size 4 */
#define TASK_T_STACK_LENGTH 1024
#define TASK_T_SIZE TASK_T_OFFSET_PSP+4 /* 1032 */

#define TASK_CONTAINER_ITEM_SIZE 4
#define TASK_CONTAINER_SIZE TASKS_MAX_NR
#define TASK_CONTAINER_OFFSET_LIST 0
#define TASK_CONTAINER_OFFSET_INDEX TASK_CONTAINER_ITEM_SIZE*TASK_CONTAINER_SIZE /* 16 */
#define TASK_CONTAINER_OFFSET_SIZE TASK_CONTAINER_OFFSET_INDEX+4 
/*PendSV

 *get conainter
    ldr r0, =tasks_container
*get current and next task index
    ldr r1, [r0, #0x10]
    add r2, r1, #1
    cmp r2, #4     ; max 
    it ge
    mov r2, #0
*get address of current
    muli r1, r1, #4 ; offset in list
    add r1, r1, r0 ; address in list
    ldr r1, [r1, #0] ; address of task
*get address of next task
    muli r2, r2, #4
    add r2, r2, r0
    ldr r2, [r2, #0]


*context prev
    mrs r3, psp
    cbz r3, switch_to_next
    stmdb r3!, {r4-r7}
    mov r4, r8
    mov r5, r9
    mov r6, r10
    mov r7, r11
    stmdb r3!, {r4-r7}
    str r3, [r1, #1028] ; save psp to task psp item
*context next
    switch_to_next:
    ldr r3, [r2, #1028]
    ldmia r3!, {r4-r7}
    mov r4, r8
    mov r5, r9
    mov r6, r10
    mov r7, r11
    ldmia r3!, {r4-r7}
    msr psp, r3
*set lr
    ldr r0, =thread_mode_lr_value   ;mov lr, #0xfffffffd
    ldr lr, [r0, #0]
    ret
 */

struct Task_t{
    void (*function)( void );                
    unsigned int stack[ TASK_STACK_SIZE ];     
    unsigned int psp;                   
};
/* tasks container , used by scheduler */
struct Task_container_t {
    struct Task_t* Scheduler_tasks[ TASKS_MAX_NR ] ; 
    int index; 
    int size;  
};
extern unsigned int Systick_timer_counter;
extern struct Task_container_t tasks_container ;
/* tasks list */

/* registors offset in array
 * PSR PC LR r12 r3 r2 r1 r0 | r7 r6 r5 r4 | r11 r10 r9 r8
 * {r4-r11} restore by soft
 * {r0-r3},lr,pc,psr restore by hardware
 */


 int Schedul_createTaskAndAppend(
        void(*task_function)(void),
        void(*task_return)(void),
        struct Task_t* obj_task
    );
int Schedul_init();

#define SET_PENDSV_EXCEPTION_BIT  (SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk)
int PendSV_init( void );

