#include<linux/sched.h>
#include<linux/pid.h>
#include<linux/cred.h>
#include<linux/sched/signal.h>
#include<linux/dcache.h>
#include<linux/slab.h>
#include<linux/fs.h>
#include<linux/fs_struct.h>
#include<linux/fdtable.h>
#include<linux/net.h>

#define DEFAULT_SUCCESS	1
#define DEFAULT_FAILURE -1

#define PTR_NULL_CHECK(ptr)	if(NULL == ptr) return DEFAULT_FAILURE;

/*
 * Get the task_struct for the given userspace process ID
 *
 */
static inline struct task_struct * get_task_struct_by_pid(int pid) {

	return pid_task(find_vpid(pid), PIDTYPE_PID);	

}

/* 
 * Print task comm, pid, tgid for a given task_struct
 *
 */
static inline int print_task_pid_details(struct task_struct *tsk) {

	PTR_NULL_CHECK(tsk);

	// print basic info
	
	pr_info("Task Binary: %s TGID: %d PID: %d\n",
			tsk->comm,
			tsk->tgid,
			tsk->pid
	       );

	return DEFAULT_SUCCESS;
}

static inline int print_task_parent_pid_details(struct task_struct *tsk) {

	PTR_NULL_CHECK(tsk);

	// Real parent -- either process which started this or init if the original 
	// parent exited 

	pr_info("Parent Task Binary: %s TGID: %d PID: %d\n",
			tsk->real_parent->comm,
			tsk->real_parent->tgid,
			tsk->real_parent->pid
	       );

	return DEFAULT_SUCCESS;
}

/*
 * Get number of threads in task group
 */
static inline int get_task_thread_count(struct task_struct *tsk) {

	return tsk->signal->nr_threads;

}

/*
 * Print binary path
 */
static inline int print_binary_path(struct task_struct *tsk){
	
	char *target_path;
	char *temp_path;
	//sanity check
	PTR_NULL_CHECK(tsk);
	//Access the process memory map
	if(tsk->mm != NULL)
	{
		//hold memory map semaphore
		down_read(&tsk->mm->mmap_sem);
		if(tsk->mm->exe_file !=NULL)
		{
			//Allocate memory to buffer	
			temp_path= kmalloc(PATH_MAX, GFP_KERNEL);
			//Sanity check for memory allocation
			if(temp_path == NULL) panic("memory allocation failed\n");
			//get the path of the process
			target_path = d_path(&tsk->mm->exe_file->f_path,temp_path, PATH_MAX);

			pr_info("Binary Path: %s\n", target_path);
			//free the memory
			kfree(temp_path);
		}
		else
		{
			pr_info("Binary Path:(none)\n");		
		}
		
		//release the semaphore
		up_read(&tsk->mm->mmap_sem);
	}
	else
	{
		pr_info("target_process->mm is NULL. Is it a kernel thread?\n");	
	}

	return DEFAULT_SUCCESS;

}

/*
* Print root and pwd path
*/
static inline int print_root_pwd_path(struct task_struct *tsk){

	char *temp_path;	
	struct path root_path,pwd_path;
	char *root_path_name,*pwd_path_name;
	PTR_NULL_CHECK(tsk);
	
	//get root path
	get_fs_root(tsk->fs, &root_path);
	//Allocate memory to buffer	
	temp_path= kmalloc(PATH_MAX, GFP_KERNEL);
	//Sanity check for memory allocation
	if(temp_path == NULL) panic("memory allocation failed\n");
	root_path_name = d_path(&root_path,temp_path, PATH_MAX);	
	pr_info("Root Path: %s\n",root_path_name);

	//get pwd path
	get_fs_pwd(tsk->fs, &pwd_path);
	pwd_path_name = d_path(&pwd_path,temp_path, PATH_MAX);	
	pr_info("Pwd Path: %s\n",pwd_path_name);

	kfree(temp_path);
	return DEFAULT_SUCCESS;

}

static inline void print_sock_type(short type){
	
	switch(type){

		case SOCK_STREAM: pr_cont("Type: SOCK_STREAM\n"); break;
		case SOCK_DGRAM: pr_cont("Type: SOCK_DGRAM\n"); break;
		case SOCK_RAW: pr_cont("Type: SOCK_RAW\n"); break;
		case SOCK_RDM: pr_cont("Type: SOCK_RDM\n"); break;
		case SOCK_SEQPACKET: pr_cont("Type: SOCK_SEQPACKET\n"); break;
		case SOCK_DCCP: pr_cont("Type: SOCK_DCCP\n"); break;
		case SOCK_PACKET: pr_cont("Type: SOCK_PACKET\n"); break;
	}
}

static inline void print_sock_family(int family){

	switch(family) {

		case AF_UNIX: pr_info("\t\tAddress Family: AF_UNIX/AF_LOCAL \n"); break;
		case AF_INET: pr_info("\t\tAddress Family: AF_INET "); break;
		case AF_INET6: pr_info("\t\tAddress Family: AF_INET6 "); break;
		case AF_PACKET: pr_info("\t\tAddress Family: AF_PACKET\n"); break;

		default: pr_info("\t\tAddress Family: (%d)\n", family);

	}
}

static inline int print_file_descriptor(const void *arg, struct file *f, unsigned fd_value){

	char *temp_path;
	char *file_path;
	struct socket *s;
	int err;
	
	PTR_NULL_CHECK(f);
	//Allocate memory to buffer	
	temp_path= kmalloc(PATH_MAX, GFP_KERNEL);
	//Sanity check for memory allocation
	if(temp_path == NULL) panic("memory allocation failed\n");
	//get file path in char array	
	file_path = d_path(&f->f_path,temp_path, PATH_MAX);
	pr_info("File descriptor: %d File Path: %s",fd_value,file_path);

	//free the memory
	kfree(temp_path);

	//Check if the file descriptor is a socket
	s = sock_from_file(f,&err);
	if(s!=NULL)
	{
		print_sock_type(s->type);
		print_sock_family(s->ops->family);	
	}	

	//want to iterate all files so signal a non-positive value
	return 0;
}

static inline int print_open_fd_details(struct task_struct *tsk){

	PTR_NULL_CHECK(tsk);
	iterate_fd(tsk->files,0, print_file_descriptor,NULL);	
	return DEFAULT_SUCCESS;
}
