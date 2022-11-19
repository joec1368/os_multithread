#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h> 
#include <linux/uaccess.h> 
#include <linux/version.h> 

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0) 
#define HAVE_PROC_OPS 
#endif 
 
#define procfs_name "thread_info" 
 
static struct proc_dir_entry *our_proc_file; 
 
#define PROCFS_MAX_SIZE 1024 
 

/* This structure hold information about the /proc file */ 
static struct proc_dir_entry *our_proc_file; 

/* The buffer used to store character for this module */ 
static char procfs_buffer[PROCFS_MAX_SIZE]; 


/* The size of the buffer */ 
static unsigned long procfs_buffer_size = 0; 

char tid_buffer[200][15];
 
int tid_position = 0;

static ssize_t procfile_read(struct file *filePointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
    char sendbuffer[15*200];
    int start = 0;
    int i = 0 ;
    for(; i < tid_position ; i++){
        int j = 0 ;
        for(; j < 15 ; j++){
            if(tid_buffer[i][j] == '\n'){
                sendbuffer[start++] = ' ';
                break;
            }
            sendbuffer[start++] = tid_buffer[i][j];
        }
    }
    pr_info("procfile send %s \n", sendbuffer);
    int len = sizeof(char) * start; 
    ssize_t ret = len;  
    if (*offset >= len || copy_to_user(buffer, sendbuffer, len)) { 
        pr_info("copy_to_user failed\n"); 
        ret = 0; 
    } else { 
        pr_info("procfile read %s \n", filePointer->f_path.dentry->d_name.name);
        *offset += len; 
    }

    //sudo dmesg
//sudo insmod test.ko
//sudo lsmod | grep test
//sudo rmmod test 
 
     return ret; 
} 

static ssize_t procfile_write(struct file *file, const char __user *buff, 
                              size_t len, loff_t *off) 
{ 
    procfs_buffer_size = len; 
    if (procfs_buffer_size > PROCFS_MAX_SIZE) 
        procfs_buffer_size = PROCFS_MAX_SIZE; 
 
    if (copy_from_user(procfs_buffer, buff, procfs_buffer_size)) // import data from user 
        return -EFAULT; 
 
    procfs_buffer[procfs_buffer_size & (PROCFS_MAX_SIZE - 1)] = '\0'; 
    int i = 0;
    while(1){
      if(procfs_buffer[i] == '\0' || procfs_buffer[i] == ' ') break;
      if((procfs_buffer[i] - '0') < 0 ) break;
      else{
        tid_buffer[tid_position][i] = procfs_buffer[i]; 
        i++;
      }
    }
    tid_buffer[tid_position++][i] = '\n';
    pr_info("read : %s\n", tid_buffer[tid_position-1]); 
    *off += procfs_buffer_size; 
    return procfs_buffer_size; 
} 
 
#ifdef HAVE_PROC_OPS 
static const struct proc_ops proc_file_fops = { 
    .proc_read = procfile_read,
    .proc_write = procfile_write, 
}; 
#else 
static const struct file_operations proc_file_fops = { 
    .read = procfile_read, 
    .write = procfile_write,
}; 
#endif 
 
static int __init procfs1_init(void) 
{ 
    our_proc_file = proc_create(procfs_name, 0777, NULL, &proc_file_fops); 
    if (NULL == our_proc_file) { 
        proc_remove(our_proc_file); 
        pr_alert("Error:Could not initialize /proc/%s\n", procfs_name); 
        return -ENOMEM; 
    } 
 
    pr_info("/proc/%s created\n", procfs_name); 

    return 0; 
} 
 
static void __exit procfs1_exit(void) 
{ 
    proc_remove(our_proc_file); 
    pr_info("/proc/%s removed\n", procfs_name); 
} 
 
module_init(procfs1_init); 
module_exit(procfs1_exit); 
 
//sudo dmesg
//sudo insmod test.ko
//sudo lsmod | grep test
//sudo rmmod test
MODULE_LICENSE("GPL");