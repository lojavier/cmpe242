#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x195c2787, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x25b30618, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0x55c64b49, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xf95e7316, __VMLINUX_SYMBOL_STR(gpiod_unexport) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x2b7c100a, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0x83243f2d, __VMLINUX_SYMBOL_STR(gpiod_set_debounce) },
	{ 0xec7ccae3, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x1fcd6241, __VMLINUX_SYMBOL_STR(gpiod_export) },
	{ 0xafb574ea, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0xd5f508b9, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x8bc1b5b8, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x733bbb51, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xf286ce23, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xdf2df03d, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0x4b9fab3e, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x79063479, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x373db350, __VMLINUX_SYMBOL_STR(kstrtoint) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "FAA70528636CD47D75C68C4");
