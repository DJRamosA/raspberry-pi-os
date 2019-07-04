#include "sched.h"
#include "irq.h"
#include "printf.h"

#define TIME_SLICE                  5
#define S                          30

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS*3] = {&(init_task), };
int nr_tasks = 1;

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}

void append(int p,struct task_struct* c)
{
	struct task_struct * pc;
	for(int i = NR_TASKS*p; i < NR_TASKS*(p+1); p++){
		pc = task[i];
		if(pc){
			pc = c;
		}
	}
}


void _schedule(void)
{
	preempt_disable();
	int next;
	struct task_struct * p;
	while (1) {
		int count = 0;
		next = 0;
		//regla 1,2 y 4
		if(current->counter > TIME_SLICE){
			for (int i = NR_TASKS*3; i >= 0 ; i--){
				if (p->counter > TIME_SLICE){
					int pri = i/NR_TASKS;
					task[i]->counter = 0;
					append(pri+1, task[i]);
					task[i] == NULL;
				}
			}

			for (int i = NR_TASKS*3; i >= 0 ; i--){
				p = task[i];
				if(p && p->state == TASK_RUNNING && p->counter < TIME_SLICE){
					next = i;	
				}
				
			}
			if (next != -1){
					switch_to(task[next]);
					break;
			}
		}
		if(current->state == TASK_RUNNING) {
			current->counter++;
			break;
		}
		//regla 5
		if (count == S)
		{
			for (int i = NR_TASKS*3; i >= NR_TASKS ; i--){
				int pri = i/NR_TASKS;
				append(pri-1, task[i]);
			}
		}
		//regla 3
		
		
		
	}

	preempt_enable();
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
	preempt_enable();
}


void timer_tick()
{
	--current->counter;
	if (current->counter>0 || current->preempt_count >0) {
		return;
	}
	current->counter=0;
	enable_irq();
	_schedule();
	disable_irq();
}
