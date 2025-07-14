#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x2c635209, "module_layout" },
	{ 0x9102ae08, "netlink_kernel_release" },
	{ 0x90de72a9, "register_ftrace_function" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x8ef503cc, "__netlink_kernel_create" },
	{ 0x18c00784, "init_net" },
	{ 0x985722f4, "path_put" },
	{ 0x2d39b0a7, "kstrdup" },
	{ 0x66cb4fab, "d_path" },
	{ 0x1c0fe0f9, "user_path_at_empty" },
	{ 0xb43f9365, "ktime_get" },
	{ 0x2d5f69b3, "rcu_read_unlock_strict" },
	{ 0xda359018, "__task_pid_nr_ns" },
	{ 0xf9a24c28, "init_pid_ns" },
	{ 0x38bbd2e, "from_kuid" },
	{ 0x67abec70, "init_user_ns" },
	{ 0xaf88e69b, "kmem_cache_alloc_trace" },
	{ 0x30a93ed, "kmalloc_caches" },
	{ 0x8522d6bc, "strncpy_from_user" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x1000e51, "schedule" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0x21144249, "netlink_unicast" },
	{ 0x37a0cba, "kfree" },
	{ 0x69acdf38, "memcpy" },
	{ 0x469ca875, "__nlmsg_put" },
	{ 0x10473ee, "__alloc_skb" },
	{ 0xe914e41e, "strcpy" },
	{ 0x9166fada, "strncpy" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0x754d539c, "strlen" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x800473f, "__cond_resched" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xa5a1793e, "unregister_ftrace_function" },
	{ 0x6091797f, "synchronize_rcu" },
	{ 0x6f935e35, "ftrace_set_filter_ip" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x63026490, "unregister_kprobe" },
	{ 0xfcca5424, "register_kprobe" },
	{ 0x4c9f47a5, "current_task" },
	{ 0x1e6d26a8, "strstr" },
	{ 0xa0aa95d4, "kthread_stop" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0x9ce84760, "wake_up_process" },
	{ 0x851c15cf, "kthread_create_on_node" },
	{ 0x5a921311, "strncmp" },
	{ 0x92997ed8, "_printk" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "CBA68B4CF313697495AEE68");
