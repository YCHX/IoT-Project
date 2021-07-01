#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/semaphore.h>

#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/cdev.h>

static char itrpt_flag[4];
struct dentry *debug_entry_dir;
static int debug_read_size;

static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM  = 1;
static unsigned int mydevice_major;

static struct cdev mydevice_cdev;
static struct class *mydevice_class = NULL;


static struct semaphore g_dev_probe_sem;
static int g_platform_probe_flag;
static unsigned long g_base_addr;
static unsigned long g_size;
static int g_irq;
static void __iomem *g_ioremap_addr;

static int probe(struct platform_device *pdev);
static int remove(struct platform_device *pdev);

static struct of_device_id test_driver_dt_ids[] = {
	{
		.compatible = "altr,pio-20.10"
	},
	{ /* end of table */ }
};

static struct platform_driver driver = {
    .probe = probe,
	.remove = remove,
	.driver = {
		.name = "Dev-driver",
		.owner = THIS_MODULE,
		.of_match_table = test_driver_dt_ids,
	},
};

static int debug_open(struct inode *inode, struct file *file)
{
	printk("debug_open\n");
	//debug_read_size = 4;
	return 0;
}

static ssize_t debug_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int len;
	printk("debug_read\n");
	printk("read_size: %d\n",debug_read_size);
	
	if (debug_read_size > 0){
		
		//snprintf(itrpt_flag, sizeof(itrpt_flag), "%lu", (unsigned long)iore);
		len = sprintf(buf, "%s\n", itrpt_flag);
		debug_read_size = 0;
		
		return len;
	}else{
		len = sprintf(buf, "e");
		return 0;
	}
}

static ssize_t debug_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("debug_write\n");

	sscanf(buf, "%s", itrpt_flag);
	return count;
}

static struct file_operations debug_fops = {
    .owner = THIS_MODULE,
	.open = debug_open,
    .read  = debug_read,
    .write = debug_write,
};

static irqreturn_t
interrupt(int irq, void *dev_id)
{
	uint32_t io_result;

	if (irq == g_irq) {
		pr_info("Interrupt %d occured\n", irq);
		io_result = ioread32(g_ioremap_addr);
		pr_info("pio: %x\n", io_result);
		switch (io_result){
			case 0:
				itrpt_flag[0] = '0';
				break;
			case 1:
				itrpt_flag[0] = '1';
				break;
			case 2:
				itrpt_flag[0] = '2';
				break;
			case 3:
				itrpt_flag[0] = '3';
				break;
		}
		debug_read_size = 4;
		/* reset edgecapture register */
		iowrite32(0xf, g_ioremap_addr+12);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static int
probe(struct platform_device *pdev)
{
    int ret;
	struct resource *r;
	struct resource *mem_region;
	int irq;
	uint32_t io_result;
  
	pr_info("probe\n");

	ret = -EBUSY;
	if (down_interruptible(&g_dev_probe_sem))
		return -ERESTARTSYS;
	if (g_platform_probe_flag != 0)
		goto bad_exit_return;
  
	ret = -EINVAL;

    r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r != NULL) {
	  pr_info("r->start = 0x%08lx\n", (long unsigned int)r->start);
	  pr_info("r->end = 0x%08lx\n", (long unsigned int)r->end);
	  pr_info("r->name = %s\n", r->name);
	} else {
	  pr_err("IORESOURCE_MEM, 0 does not exits\n");
	  goto bad_exit_return;
	}
	g_base_addr = r->start;
	g_size = resource_size(r);
	irq = platform_get_irq(pdev, 0);
	g_irq = irq;
	if (irq < 0) {
	  pr_err("get_irq faile\n");
	  goto bad_exit_return;
	} else {
	  pr_info("irq = %d\n", irq);
	}

	mem_region = request_mem_region(g_base_addr,g_size,"test_hw_region");
	pr_info("request_mem_region\n");
	if (mem_region == NULL){
	  pr_err("request_mem_region failed: test_driver\n");
	  goto bad_exit_return;
	}

	pr_info("ioremap\n");
	g_ioremap_addr = ioremap(g_base_addr, g_size);
	if (g_ioremap_addr == NULL){
	  pr_err("ioremap failed: test_driver\n");
	  goto bad_exit_release_mem_region;
	}

	 pr_info("probe registers\n");
	 io_result = ioread32(g_ioremap_addr);
	 pr_info("pio+0: %x (data)\n", io_result);
	 io_result = ioread32(g_ioremap_addr+4);
	 pr_info("pio+4: %x (direction)\n", io_result);
	 io_result = ioread32(g_ioremap_addr+8);
	 pr_info("pio+8: %x (interrupt mask)\n", io_result);
	 io_result = ioread32(g_ioremap_addr+12);
	 pr_info("pio+12: %x (edgecapture)\n", io_result);

	 pr_info("set interrupt mask\n");
	 iowrite32(0xf, g_ioremap_addr+8);
	 iowrite32(0xf, g_ioremap_addr+12);

	  ret = request_irq(irq, interrupt, 0, driver.driver.name, &driver);
	  if(ret) {
	    pr_err("request_irq_failed");
	    goto bad_exit_iounmap;
	  }
    

	g_platform_probe_flag = 1;
	up(&g_dev_probe_sem);
	pr_info("probe exit!!\n");
	return 0;
  
bad_exit_iounmap:
	iounmap(g_ioremap_addr);
bad_exit_release_mem_region:
	release_mem_region(g_base_addr, g_size);
bad_exit_return:
	up(&g_dev_probe_sem);
	pr_info("probe bad_exit\n");
	return ret;
};

static int
remove(struct platform_device *pdev)
{
	pr_info("test_remove\n");

	free_irq(g_irq, &driver);

	iounmap(g_ioremap_addr);
	release_mem_region(g_base_addr, g_size);

	if (down_interruptible(&g_dev_probe_sem))
		return -ERESTARTSYS;
	g_platform_probe_flag = 0;
	up(&g_dev_probe_sem);
	return 0;
}

MODULE_DEVICE_TABLE(of, test_driver_dt_ids);

MODULE_LICENSE("GPL");

static int __init
init(void)
{
	int ret;
    int err;
    dev_t dev;
	int alloc_ret ;

	pr_info("test_init_enter\n");
	sema_init(&g_dev_probe_sem, 1);
	ret = platform_driver_register(&driver);
    

	if (ret != 0) {
		pr_err("platform_driver_register returned %d\n", ret);
		return ret;
	}
	pr_info("dev-driver registerd\n");

	
	

	alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE,MINOR_NUM, "test_driver");
	if (alloc_ret != 0) {
		printk(KERN_ERR  "alloc_chrdev_region = %d\n", alloc_ret);
		return -1;
	}
	mydevice_major = MAJOR(dev);
	dev = MKDEV(mydevice_major, MINOR_BASE);

	cdev_init(&mydevice_cdev, &debug_fops);
	mydevice_cdev.owner = THIS_MODULE;

	err = cdev_add(&mydevice_cdev, dev, MINOR_NUM);
	if (err != 0) {
		printk(KERN_ERR  "cdev_add = %d\n", alloc_ret);
		unregister_chrdev_region(dev, MINOR_NUM);
		return -1;
	}

	mydevice_class = class_create(THIS_MODULE, "test_device");
	if (IS_ERR(mydevice_class)) {
		printk(KERN_ERR  "class_create\n");
		cdev_del(&mydevice_cdev);
		unregister_chrdev_region(dev, MINOR_NUM);
		return -1;
	}

	
		device_create(mydevice_class, NULL, MKDEV(mydevice_major, 0), NULL, "test_device%d", 0);
	


	// debug_entry_dir = debugfs_create_dir("test_driver", NULL);
	// if (debug_entry_dir == NULL){
	// 	printk(KERN_ERR "debugfs_create_dir\n");
	// 	return -ENOMEM;
	// }

	// debugfs_create_file("itrpt_flag", S_IRUGO | S_IWUGO, debug_entry_dir, NULL, &debug_fops);

	return 0;
}

static void
driver_exit(void)
{
	dev_t dev;
	pr_info("do exit for dev-driver\n");
	platform_driver_unregister(&driver);
	pr_info("exit from dev-driver\n");

	dev = MKDEV(mydevice_major, MINOR_BASE);
	
		device_destroy(mydevice_class, MKDEV(mydevice_major, 0));
	

	class_destroy(mydevice_class);

	cdev_del(&mydevice_cdev);

	unregister_chrdev_region(dev, MINOR_NUM);
	// debugfs_remove_recursive(debug_entry_dir);
}
module_init(init);
module_exit(driver_exit);



