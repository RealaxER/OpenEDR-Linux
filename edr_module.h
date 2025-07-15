#ifndef __EDR_PROBE_H__
#define __EDR_PROBE_H__

/*config*/


#define EDR_QUEUE_RING_BUFFER 1

static inline void syscall_get_arguments(struct task_struct *task,
					 struct pt_regs *regs,
					 unsigned long *args)
{
#ifdef CONFIG_X86_32
    args[0] = regs->bx;
    args[1] = regs->cx;
    args[2] = regs->dx;
    args[3] = regs->si;
    args[4] = regs->di;
    args[5] = regs->bp;
#elif CONFIG_X86_64
# ifdef CONFIG_IA32_EMULATION
	if (task->thread_info.status & TS_COMPAT) {
		*args++ = regs->bx;
		*args++ = regs->cx;
		*args++ = regs->dx;
		*args++ = regs->si;
		*args++ = regs->di;
		*args   = regs->bp;
	} else
#endif
	{
		*args++ = regs->di;
		*args++ = regs->si;
		*args++ = regs->dx;
		*args++ = regs->r10;
		*args++ = regs->r8;
		*args   = regs->r9;
	}
#elif CONFIG_ARM64
	args[0] = regs->orig_x0;
	args++;

	memcpy(args, &regs->regs[1], 5 * sizeof(args[0]));
#elif CONFIG_ARM
	args[0] = regs->ARM_ORIG_r0;
	args++;

	memcpy(args, &regs->ARM_r0 + 1, 5 * sizeof(args[0]));
#endif

}

static inline void set_syscall_return(struct pt_regs *regs, long ret)
{
#if defined(CONFIG_X86_64) || defined(CONFIG_X86_32)
    regs->ax = ret;

#elif defined(CONFIG_ARM64)
    regs->regs[0] = ret;

#elif defined(CONFIG_ARM)
    regs->ARM_r0 = ret;

#elif defined(CONFIG_RISCV)
    regs->a0 = ret;

#elif defined(CONFIG_MIPS)
    regs->regs[2] = ret;

#else
# error "Unsupported architecture: define syscall return register"
#endif
}

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_32)
asmlinkage void just_return_func(void);

asm(
	".text\n"
	".type just_return_func, @function\n"
	".globl just_return_func\n"
	"just_return_func:\n"
		ASM_RET
	".size just_return_func, .-just_return_func\n"
);

void override_function_with_return(struct pt_regs *regs)
{
	regs->ip = (unsigned long)&just_return_func;
}
#elif defined(CONFIG_ARM64)
static inline void instruction_pointer_set(struct pt_regs *regs,
		unsigned long val)
{
	regs->pc = val;
}
#elif defined(CONFIG_ARM)
#define instruction_pointer(regs)	(regs)->ARM_pc
static inline void instruction_pointer_set(struct pt_regs *regs,
					   unsigned long val)
{
	instruction_pointer(regs) = val;
}
#endif
NOKPROBE_SYMBOL(override_function_with_return);

static inline void set_function_ip(struct pt_regs *regs, void *func)
{
    unsigned long addr = (unsigned long)func;

#if defined(CONFIG_X86_64)
    regs->ip = addr; // hoặc regs->rip, kiểm tra trong arch/x86/include/asm/ptrace.h

#elif defined(CONFIG_X86_32)
    regs->ip = addr; // hoặc regs->eip

#elif defined(CONFIG_ARM64)
    regs->pc = addr;

#elif defined(CONFIG_ARM)
    regs->ARM_pc = addr;

#elif defined(CONFIG_RISCV)
    regs->epc = addr;

#elif defined(CONFIG_MIPS)
    regs->epc = addr;

#elif defined(CONFIG_PPC64) || defined(CONFIG_PPC)
    regs->nip = addr;

#elif defined(CONFIG_S390)
    regs->psw.addr = addr;

#else
# error "Unsupported architecture: define instruction pointer register"
#endif
}

#endif