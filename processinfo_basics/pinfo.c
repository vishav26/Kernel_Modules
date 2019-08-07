
#include <linux/module.h>
#include <linux/init.h>

#include "processlibrary.h"

static int process_id;
module_param(process_id, int, S_IWUSR | S_IRUGO);
MODULE_PARM_DESC(process_id,"Process ID of target process");

static int __init lkm_init(void) {
	
	struct task_struct *target_process;
	struct task_struct *process_thread;
	
	//Get task_struct for userspace process id
	target_process = get_task_struct_by_pid(process_id);
	if(!IS_ERR_OR_NULL(target_process)){
	
		pr_info("****************PROCESS DETAILS***************\n\n");
		pr_info("Received Process ID: %d\n", process_id);

		// Print task Binary name (truncated), TGID, PID
		print_task_pid_details(target_process);

		// Print task Parent details 
		print_task_parent_pid_details(target_process);

		// Print number of threads 
		pr_info("\nNumber of threads: %d\n", 
		get_task_thread_count(target_process));

		// Print details of each thread
		pr_info("\nDetails of threads: \n");
		for_each_thread(target_process, process_thread) {
			
			print_task_pid_details(process_thread);

		}

		print_binary_path(target_process);

		print_root_pwd_path(target_process);
		pr_info("\n\n");		
		pr_info("**********************************************\n\n");

		
	

	}
	else{
		pr_info("Invalid Process ID: %d Received\n", process_id);
		return -EINVAL;
	}
	
	return 0;
}

static void __exit lkm_exit(void){

	pr_info("Exiting ...\n");
}

module_init(lkm_init);
module_exit(lkm_exit);

MODULE_AUTHOR("Vishav Bansal");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Print Process Details");
