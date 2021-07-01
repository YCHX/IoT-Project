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

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r != NULL) {
	  pr_info("r->start = 0x%08lx\n", (long unsigned int)r->start);
	  pr_info("r->end = 0x%08lx\n", (long unsigned int)r->end);
	  pr_info("r->name = %s\n", r->name);
	} else {
	  pr_err("IORESOURCE_MEM, 0 does not exits\n");
	  goto bad_exit_return;
	}
	g_test_base_addr = r->start;
	g_test_size = resource_size(r);
	irq = platform_get_irq(pdev, 0);
	g_test_irq = irq;
	if (irq < 0) {
	  pr_err("get_irq faile\n");
	  goto bad_exit_return;
	} else {
	  pr_info("irq = %d\n", irq);
	}

	test_mem_region = request_mem_region(g_test_base_addr,g_test_size,"test_hw_region");
	pr_info("request_mem_region\n");
	if (test_mem_region == NULL){
	  pr_err("request_mem_region failed: test_driver\n");
	  goto bad_exit_return;
	}

	pr_info("ioremap\n");
	g_ioremap_addr = ioremap(g_test_base_addr, g_test_size);
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

	  ret = request_irq(irq, int_test_interrupt, 0, test_driver.driver.name, &test_driver);
	  if(ret) {
	    pr_err("request_irq_failed");
	    goto bad_exit_iounmap;
	  }

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

	free_irq(g_test_irq, &test_driver);

	iounmap(g_ioremap_addr);
	release_mem_region(g_test_base_addr, g_test_size);

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
