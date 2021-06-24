#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/semaphore.h>

#include <linux/fs.h>


static struct semaphore g_dev_probe_sem;
static int g_platform_probe_flag;
static unsigned long g_base_addr;
static unsigned long g_size;
static int g_irq;
static void __iomem *g_ioremap_addr;

static int probe(struct platform_device *pdev);
static int remove(struct platform_device *pdev);

static struct of_device_id driver_dt_ids[] = {
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
		.of_match_table = driver_dt_ids,
	},
};

static irqreturn_t
interrupt(int irq, void *dev_id)
{
	uint32_t io_result;

	if (irq == g_irq) {
		pr_info("Interrupt %d occured\n", irq);
		io_result = ioread32(g_ioremap_addr);
		pr_info("pio: %x\n", io_result);
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

	if (down_interruptible(&g_dev_probe_sem))
		return -ERESTARTSYS;
	g_platform_probe_flag = 0;
	up(&g_dev_probe_sem);
	return 0;
}

MODULE_DEVICE_TABLE(of, driver_dt_ids);

MODULE_LICENSE("GPL");

static int __init
init(void)
{
	int ret;
    int err;
    dev_t dev;

	pr_info("test_init_enter\n");
	sema_init(&g_dev_probe_sem, 1);
	ret = platform_driver_register(&driver);
    

	if (ret != 0) {
		pr_err("platform_driver_register returned %d\n", ret);
		return ret;
	}
	pr_info("dev-driver registerd\n");

	return 0;
}

static void
exit(void)
{
	pr_info("do exit for dev-driver\n");
	platform_driver_unregister(&driver);
	pr_info("exit from dev-driver\n");
}
module_init(init);
module_exit(exit);

static ssize_t read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    printk("myDevice_read\n");
    buf[0] = 'A';
    return 1;
}

struct file_operations s_myDevice_fops = {
    
    .read    = read,
    
};

