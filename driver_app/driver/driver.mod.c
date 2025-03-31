#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
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
	{ 0x30bafec6, "dma_mmap_attrs" },
	{ 0x122c3a7e, "_printk" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xedc03953, "iounmap" },
	{ 0x77358855, "iomem_resource" },
	{ 0x1035c7c2, "__release_region" },
	{ 0x37a0cba, "kfree" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xeea3c1d8, "__class_create" },
	{ 0x6c333d48, "device_create" },
	{ 0x4c260c62, "cdev_alloc" },
	{ 0xff0d65c6, "cdev_add" },
	{ 0xe716aaca, "dma_set_coherent_mask" },
	{ 0xe3589312, "dma_alloc_attrs" },
	{ 0xde6c83c2, "__platform_driver_register" },
	{ 0x3e2bc4cf, "device_destroy" },
	{ 0x6a2dd7c, "class_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x43d34239, "cdev_del" },
	{ 0x64c56e5a, "platform_driver_unregister" },
	{ 0xdb435cf9, "dma_free_attrs" },
	{ 0x3f2c3bbe, "platform_get_resource" },
	{ 0x7affd727, "kmalloc_caches" },
	{ 0x3d650a07, "kmalloc_trace" },
	{ 0x85bd1608, "__request_region" },
	{ 0xde80cd09, "ioremap" },
	{ 0xa9245a62, "platform_get_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xfcec0987, "enable_irq" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xa19b956, "__stack_chk_fail" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xa78af5f3, "ioread32" },
	{ 0x4a453f53, "iowrite32" },
	{ 0x453e7dc, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Ccnn_ip");
MODULE_ALIAS("of:N*T*Ccnn_ipC*");
MODULE_ALIAS("of:N*T*Cdma");
MODULE_ALIAS("of:N*T*CdmaC*");

MODULE_INFO(srcversion, "62680C320BF8D087C9CDE83");
