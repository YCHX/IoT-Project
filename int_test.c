#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/semaphore.h>

static struct semaphore g_dev_probe_sem;
static int g_platform_probe_flag;
static unsigned long g_test_base_addr;
static unsigned long g_test_size;
static int g_test_irq;
static void __iomem *g_ioremap_addr;

static int test_probe(struct platform_device *pdev);
static int test_remove(struct platform_device *pdev);

static struct of_device_id test_driver_dt_ids[] = {
	{
		.compatible = "altr,pio-20.10"
	},
	{ /* end of table */ }
};

static struct platform_driver test_driver = {
	.probe = test_probe,
	.remove = test_remove,
	.driver = {
		.name = "test_driver",
		.owner = THIS_MODULE,
		.of_match_table = test_driver_dt_ids,
	},
};

static irqreturn_t
int_test_interrupt(int irq, void *dev_id)
{
	uint32_t io_result;

	if (irq == g_test_irq) {
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
test_probe(struct platform_device *pdev)
{
	int ret;
	struct resource *r;
	struct resource *test_mem_region;
	int irq;
	uint32_t io_result;
  
	pr_info("test_probe\n");

	ret = -EBUSY;
	if (down_interruptible(&g_dev_probe_sem))
		return -ERESTARTSYS;
	if (g_platform_probe_flag != 0)
		goto bad_exit_return;
  
	ret = -EINVAL;

	g_platform_probe_flag = 1;
	up(&g_dev_probe_sem);
	pr_info("test_probe exit!!\n");
	return 0;
  
bad_exit_iounmap:
	iounmap(g_ioremap_addr);
bad_exit_release_mem_region:
	release_mem_region(g_test_base_addr, g_test_size);
bad_exit_return:
	up(&g_dev_probe_sem);
	pr_info("test_probe bad_exit\n");
	return ret;
}

static int
test_remove(struct platform_device *pdev)
{
	pr_info("test_remove\n");

	if (down_interruptible(&g_dev_probe_sem))
		return -ERESTARTSYS;
	g_platform_probe_flag = 0;
	up(&g_dev_probe_sem);
	return 0;
}

MODULE_DEVICE_TABLE(of, test_driver_dt_ids);

MODULE_LICENSE("GPL");

static int __init
int_test_init(void)
{
	int ret;

	pr_info("test_init_enter\n");
	sema_init(&g_dev_probe_sem, 1);
	ret = platform_driver_register(&test_driver);
	if (ret != 0) {
		pr_err("platform_driver_register returned %d\n", ret);
		return ret;
	}
	pr_info("int_test registerd\n");

	return 0;
}

static void
int_test_exit(void)
{
	pr_info("do exit for int_test\n");
	platform_driver_unregister(&test_driver);
	pr_info("exit from int_test\n");
}
module_init(int_test_init);
module_exit(int_test_exit);
