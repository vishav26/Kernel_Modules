
#include <linux/module.h>
#include <linux/init.h>

static int __init km_init(void){

	pr_info("Welcome to Kernel Module\n");
	return 0;
}

static void __exit km_exit(void){

	pr_info("Wow! You exited early\n");
}

module_init(km_init);
module_exit(km_exit);

MODULE_AUTHOR("Vishav Bansal");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Hello World Module");
