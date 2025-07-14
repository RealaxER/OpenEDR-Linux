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
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x8ef503cc, "__netlink_kernel_create" },
	{ 0x18c00784, "init_net" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x21144249, "netlink_unicast" },
	{ 0x469ca875, "__nlmsg_put" },
	{ 0x37a0cba, "kfree" },
	{ 0x10473ee, "__alloc_skb" },
	{ 0xb43f9365, "ktime_get" },
	{ 0x9166fada, "strncpy" },
	{ 0x4c9f47a5, "current_task" },
	{ 0xaf88e69b, "kmem_cache_alloc_trace" },
	{ 0x30a93ed, "kmalloc_caches" },
	{ 0x92997ed8, "_printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "5E031643FADB241E313064D");
