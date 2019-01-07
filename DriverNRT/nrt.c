#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/capability.h>

MODULE_LICENSE("GPL");

static inline void set_perm(int perm)
{
        struct cred *cred;
        if ((cred = prepare_creds())) {
                cap_raise(cred->cap_effective, perm);
                commit_creds(cred);
        }
}

static int nrt_open(struct inode *inode, struct file *filep)
{
  set_perm(CAP_IPC_LOCK);
  set_perm(CAP_SYS_RAWIO);
  set_perm(CAP_SYS_NICE);
  set_perm(CAP_NET_BIND_SERVICE); 
  return 0;
}

static int nrt_release(struct inode *inode, struct file *filep)
{
  return 1;
}

struct file_operations nrt_fops =
{
  owner:      THIS_MODULE,
  open:       nrt_open,
  release:    nrt_release,
};

static int __init init_mod(void)
{
  int result;
  if ((result = register_chrdev(20, "nrt", &nrt_fops)) < 0)
    goto fail;
  printk("NRT started...\n");
  return 0;
 fail:
  return -1;
}

static void __exit end_mod(void)
{
  unregister_chrdev(20, "nrt");
  printk("NRT ended...\n");
}

module_init(init_mod);
module_exit(end_mod);
