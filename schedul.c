#include "schedul.h"
#include "main.h"

unsigned int Systick_timer_counter = 0;

struct Task_container_t tasks_container = {0};

int Schedul_taskContiner_append( struct Task_t* task ){
    if( tasks_container.size >= TASKS_MAX_NR )
        return 1;
    tasks_container.Scheduler_tasks[ tasks_container.size ] = task;
    ++( tasks_container.size );
    return 0;
}
/*
 * append task to tasks_container;
 * those  tasks will be ran by Scheduler;
 * obj_task must had been alloced;
 */
int Schedul_createTaskAndAppend( void(*task_function)(void), void(*task_return)(void), struct Task_t* obj_task ){
    obj_task->function = task_function;

    /* init task stack */
    unsigned int* task_stack_top = &(obj_task->stack[256]);
    *(--task_stack_top) = 0x01000000; /* PSR */
    *(--task_stack_top) = (unsigned int) task_function; /* pc */
    *(--task_stack_top) = (unsigned int) task_return; /* lr */
    *(--task_stack_top) = 0; /*r12 */

    *(--task_stack_top) = 0; /*r3 */
    *(--task_stack_top) = 0; /*r2 */
    *(--task_stack_top) = 0; /* r1 */
    *(--task_stack_top) = 0; /* r0 */

    *(--task_stack_top) = 0; /* r4 */
    *(--task_stack_top) = 0; /* r5 */
    *(--task_stack_top) = 0; /* r6 */
    *(--task_stack_top) = 0; /* r7 */

    *(--task_stack_top) = 0; /* r8 */
    *(--task_stack_top) = 0; /* r9 */
    *(--task_stack_top) = 0; /* r10 */
    *(--task_stack_top) = 0; /* r11 */

    obj_task->psp = (unsigned int )task_stack_top;

    return Schedul_taskContiner_append ( obj_task );
}

#define SET_PENDSV_EXCEPTION_BIT  (SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk)
 volatile unsigned int stack_psp [256] = {0};
int Schedul_init(){
    /* init psp */
    __asm volatile(
        "mov r0, #0                \n"
        "msr psp, r0"
        ::
        :"r0"
    );
    return 0;
}

int PendSV_init( void ){
    /* set priority */
    NVIC_SetPriority( PendSV_IRQn, 0xff);
    return 0;
}

unsigned int thread_mode_lr_value = 0xfffffffd;
__attribute__(( naked ))
void PendSV_Handler( void ){
    __asm volatile(
    //*get conainter
        "cpsid i                    \n"
       "ldr r0, =tasks_container    \n"
   //*get current and next task index
       "ldr r1, [r0, #0x10]         \n"
       "add r2, r1, #1              \n"
       "cmp r2, #2                 \n"    // ; max
       "it ge                       \n"
       "movge r2, #0                  \n"
       "str r2, [r0, #0x10]            \n"
   //*get address of current
        "mov r3, #4                 \n"
       "mul r1, r1, r3             \n"     //; offset
       "add r1, r1, r0              \n"     //; address
       "ldr r1, [r1, #0]            \n"     //; task
   //*get address of next task
       "mul r2, r2, r3             \n"
       "add r2, r2, r0              \n"
       "ldr r2, [r2, #0]            \n"


   //*context progress prev
       "mrs r3, psp                 \n"
       "cmp r3, #0                  \n" /* if first switch */
       "bne simple_condition        \n"

       "mov r0, #0x02               \n" /* psp mode */
       "msr control, r0             \n"
       "isb                         \n"
       "b switch_to_next            \n"

       "simple_condition:           \n"
       "stmdb r3!, {r4-r7}          \n"
       "mov r4, r8                  \n"
       "mov r5, r9                  \n"
       "mov r6, r10                 \n"
       "mov r7, r11                 \n"
       "stmdb r3!, {r4-r7}          \n"
       "str r3, [r1, #1028]         \n"     //; save psp to task->psp
   //*context progress next
       "switch_to_next:             \n"
       "ldr r3, [r2, #1028]         \n"
       "ldmia r3!, {r4-r7}          \n"
       "mov r4, r8                  \n"
       "mov r5, r9                  \n"
       "mov r6, r10                 \n"
       "mov r7, r11                 \n"
       "ldmia r3!, {r4-r7}          \n"
       "msr psp, r3                 \n"
   //*set lr
       "ldr r0, =thread_mode_lr_value \n"  //;mov lr, #0xfffffffd
       "ldr lr, [r0, #0]            \n"
       "cpsie i                     \n"
       "bx lr                       \n"
    );
}
