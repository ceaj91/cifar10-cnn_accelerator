/* Linux driver for FPGA accelerator for convolutional neural network used for recognition of motor vehicles */

/* Kernel headers */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/ioport.h>
#include <asm/io.h>

/* DMA headers */

#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/interrupt.h>

MODULE_AUTHOR("y23-g02 - Ivan David Ivan");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("CNN IP core driver");

#define DRIVER_NAME "cnn_driver" 
#define DEVICE_NAME "cnn_device"
#define BUFF_SIZE 20

/* -------------------------------------- */
/* --------CNN IP RELATED MACROS--------- */
/* -------------------------------------- */

#define BIAS_INPUT_LEN			128*2	

#define CONV0_PICTURE_INPUT_LEN		34*34*3*2
#define CONV0_WEIGHTS_INPUT_LEN		3*3*3*32*2
#define CONV0_PICTURE_OUTPUT_LEN	32*32*32*2

#define CONV1_PICTURE_INPUT_LEN	 	18*18*32*2
#define CONV1_WEIGHTS_INPUT_LEN		3*3*32*32/2*2
#define CONV1_PICTURE_OUTPUT_LEN	16*16*32*2

#define CONV2_PICTURE_INPUT_LEN	 	10*10*32*2
#define CONV2_WEIGHTS_INPUT_LEN		3*3*32*64/4*2
#define CONV2_PICTURE_OUTPUT_LEN	8*8*64*2

#define MAX_PKT_LEN			CONV0_PICTURE_OUTPUT_LEN


#define IP_COMMAND_LOAD_BIAS		0x0001
#define IP_COMMAND_LOAD_WEIGHTS0	0x0002
#define IP_COMMAND_LOAD_CONV0_INPUT	0x0004
#define IP_COMMAND_START_CONV0		0x0008
#define IP_COMMAND_LOAD_WEIGHTS1	0x0010
#define IP_COMMAND_LOAD_CONV1_INPUT	0x0020
#define IP_COMMAND_START_CONV1		0x0040
#define IP_COMMAND_LOAD_WEIGHTS2	0x0080
#define IP_COMMAND_LOAD_CONV2_INPUT	0x0100
#define IP_COMMAND_START_CONV2		0x0200
#define IP_COMMAND_RESET		0x0400
#define IP_COMMAND_READ_CONV0_OUTPUT	0x0800
#define IP_COMMAND_READ_CONV1_OUTPUT	0x1000
#define IP_COMMAND_READ_CONV2_OUTPUT	0x2000


/* -------------------------------------- */
/* ----------DMA RELATED MACROS---------- */
/* -------------------------------------- */

#define MM2S_CONTROL_REGISTER       	0x00
#define MM2S_STATUS_REGISTER        	0x04
#define MM2S_SRC_ADDRESS_REGISTER   	0x18
#define MM2S_TRNSFR_LENGTH_REGISTER 	0x28

#define S2MM_CONTROL_REGISTER       	0x30
#define S2MM_STATUS_REGISTER       	0x34
#define S2MM_DST_ADDRESS_REGISTER   	0x48
#define S2MM_BUFF_LENGTH_REGISTER   	0x58

#define DMACR_RESET			0x04
#define IOC_IRQ_FLAG			1 << 12
#define ERR_IRQ_EN			1 << 14


/* -------------------------------------- */
/* --------FUNCTION DECLARATIONS--------- */
/* -------------------------------------- */

static int  cnn_probe(struct platform_device *pdev);
static int  cnn_remove(struct platform_device *pdev);
int         cnn_open(struct inode *pinode, struct file *pfile);
int         cnn_close(struct inode *pinode, struct file *pfile);
ssize_t     cnn_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t     cnn_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

static int  __init cnn_init(void);
static void __exit cnn_exit(void);

static int cnn_mmap(struct file *f, struct vm_area_struct *vma_s);
static irqreturn_t cnn_isr(int irq, void* dev_id);
static irqreturn_t dma_MM2S_isr(int irq, void* dev_id);
static irqreturn_t dma_S2MM_isr(int irq, void* dev_id);
irq_handler_t cnn_handler_irq = &cnn_isr;
irq_handler_t dma_MM2S_handler_irq = &dma_MM2S_isr;
irq_handler_t dma_S2MM_handler_irq = &dma_S2MM_isr;

int dma_init(void __iomem *base_address);
unsigned int dma_simple_write(dma_addr_t TxBufferPtr, unsigned int pkt_len, void __iomem *base_address); 
unsigned int dma_simple_read(dma_addr_t TxBufferPtr, unsigned int pkt_len, void __iomem *base_address);

/* -------------------------------------- */
/* -----------GLOBAL VARIABLES----------- */
/* -------------------------------------- */

struct cnn_info
{
	unsigned long mem_start;
	unsigned long mem_end;
	void __iomem *base_addr;
	int irq_num;
};

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device_cnn;
static struct device *my_device_dma;
static struct cdev *my_cdev;
static struct cnn_info *dma_p = NULL;
static struct cnn_info *cnn_p = NULL;

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = cnn_open,
	.release = cnn_close,
	.read = cnn_read,
	.write = cnn_write,
	.mmap = cnn_mmap
};

static struct of_device_id cnn_of_match[] = {
	{ .compatible = "cnn_ip", },
	{ .compatible = "dma", },
	{ /* end of list */ },
};

MODULE_DEVICE_TABLE(of, cnn_of_match);

static struct platform_driver cnn_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table	= cnn_of_match,
	},
	.probe		= cnn_probe,
	.remove		= cnn_remove,
};

dma_addr_t tx_phy_buffer;
u16 *tx_vir_buffer;

/* -------------------------------------- */
/* -------INIT AND EXIT FUNCTIONS-------- */
/* -------------------------------------- */

/* Init function being called and executed only once by insmod command. */

static int __init cnn_init(void)
{
	int ret = 0;
	int i = 0;

	// printk(KERN_INFO "[cnn_init] Initialize Module \"%s\"\n", DEVICE_NAME);

	/* Dynamically allocate MAJOR and MINOR numbers. */
	ret = alloc_chrdev_region(&my_dev_id, 0, 2, "CNN_region");
	if(ret)
	{
		printk(KERN_ALERT "[cnn_init] Failed CHRDEV!\n");
		return -1;
	}
	// printk(KERN_INFO "[cnn_init] Successful CHRDEV!\n");

	/* Creating NODE file */

	/* Firstly, class_create is used to create class to be used as a parametar going forward. */
	my_class = class_create(THIS_MODULE, "cnn_class");
	if(my_class == NULL)
	{
		printk(KERN_ALERT "[cnn_init] Failed class create!\n");
		goto fail_0;
	}
	// printk(KERN_INFO "[cnn_init] Successful class chardev1 create!\n");

	/* Secondly, device_create is used to create devices in a region. */
	my_device_cnn = device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 0), NULL, "cnn-ip");
	if(my_device_cnn == NULL)
	{
		goto fail_1;
	}
	// printk(KERN_INFO "[cnn_init] Device cnn-ip created\n");


	my_device_dma = device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 1), NULL, "dma");
	if(my_device_dma == NULL)
	{
		goto fail_2;
	}
	// printk(KERN_INFO "[cnn_init] Device dma created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 2);
	if(ret)
	{
		printk(KERN_ERR "[cnn_init] Failed to add cdev\n");
		goto fail_3;
	}
	// printk(KERN_INFO "[cnn_init] Module init done\n");

	/* Making sure that virtual addresses are mapped to physical addresses that are coherent */
	ret = dma_set_coherent_mask(my_device_dma, DMA_BIT_MASK(64));
	if(ret < 0)
	{
		printk(KERN_WARNING "[cnn_init] DMA coherent mask not set!\n");
	}
	else
	{
		// printk(KERN_INFO "[cnn_init] DMA coherent mask set\n");
	}

	tx_vir_buffer = dma_alloc_coherent(my_device_dma, 32*32*32*2, &tx_phy_buffer, GFP_KERNEL);
	// printk(KERN_INFO "[cnn_init] Virtual and physical addresses coherent starting at %x and ending at %x\n", tx_phy_buffer, tx_phy_buffer+(uint)(32*32*32*2));
	if(!tx_vir_buffer)
	{
		printk(KERN_ALERT "[cnn_init] Could not allocate dma_alloc_coherent for img");
		goto fail_4;
	}
	else
	{
		// printk("[cnn_init] Successfully allocated memory for dma transaction buffer\n");
	}
	
	for (i = 0; i < MAX_PKT_LEN/4; i++)
	{
		tx_vir_buffer[i] = 0x00000000;
	}
	
	// printk(KERN_INFO "[cnn_init] DMA memory reset.\n");
	return platform_driver_register(&cnn_driver);

	fail_4:
		cdev_del(my_cdev);
	fail_3:
		device_destroy(my_class, MKDEV(MAJOR(my_dev_id),1));
	fail_2:
		device_destroy(my_class, MKDEV(MAJOR(my_dev_id),0));
	fail_1:
		class_destroy(my_class);
	fail_0:
		unregister_chrdev_region(my_dev_id, 2);
	return -1;
} 

/* Exit function being called and executed only once by rmmod command. */

static void __exit cnn_exit(void)
{
    	/* Reset DMA memory */
	int i = 0;
	for (i = 0; i < MAX_PKT_LEN/4; i++) 
	{
		tx_vir_buffer[i] = 0x00000000;
	}
	
	// printk(KERN_INFO "[cnn_exit] DMA memory reset\n");

	/* Exit Device Module */
	platform_driver_unregister(&cnn_driver);
	cdev_del(my_cdev);
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),0));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),1));
	class_destroy(my_class);
	unregister_chrdev_region(my_dev_id, 2);
	dma_free_coherent(my_device_dma, MAX_PKT_LEN, tx_vir_buffer, tx_phy_buffer);
	// printk(KERN_INFO "[cnn_exit] Exit device module finished\"%s\".\n", DEVICE_NAME);
}

module_init(cnn_init);
module_exit(cnn_exit);  


/* -------------------------------------- */
/* -----PROBE AND REMOVE FUNCTIONS------- */
/* -------------------------------------- */

/* Probe function attempts to find and match a device connected to system with a driver that exists in a system */
/* If successful, memory space will be allocated for a device */

int device_fsm = 0;

static int cnn_probe(struct platform_device *pdev) 
{
	struct resource *r_mem;
	int rc = 0;
	const char *comp = of_get_property(pdev->dev.of_node, "compatible", NULL);
	
	// if(comp)  	printk(KERN_INFO "Probing %s\n", comp);
	// else 	printk(KERN_INFO "Not found\n");

	if(comp == "dma") device_fsm = 0;
	if(comp == "cnn_ip") device_fsm = 1;
	
	/* Get physical register address space from device tree */

	switch(device_fsm)
	{
	case 0:
		
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if(!r_mem)
		{
			printk(KERN_ALERT "cnn_probe: Failed to get reg resource.\n");
			return -ENODEV;
		}

		printk(KERN_ALERT "[cnn_probe] Probing dma_p\n");
		
		/* Allocate memory space for structure cnn_info */ 
		dma_p = (struct cnn_info *) kmalloc(sizeof(struct cnn_info), GFP_KERNEL);
		if(!dma_p) 
		{
			printk(KERN_ALERT "[cnn_probe] Could not allocate CNN device\n");
			return -ENOMEM;
		}

		/* Put phisical addresses in cnn_info structure */
		dma_p->mem_start = r_mem->start;
		dma_p->mem_end = r_mem->end;

		/* Reserve that memory space for this driver */
		if(!request_mem_region(dma_p->mem_start, dma_p->mem_end - dma_p->mem_start + 1,	DEVICE_NAME)) 
		{
			printk(KERN_ALERT "[cnn_probe] Could not lock memory region at %p\n",(void *)dma_p->mem_start);
			rc = -EBUSY;
			goto error4;
		}

		/* Remap physical addresses to virtual addresses */
		dma_p->base_addr = ioremap(dma_p->mem_start, dma_p->mem_end - dma_p->mem_start + 1);
		if (!dma_p->base_addr) 
		{
			printk(KERN_ALERT "[cnn_probe] Could not allocate memory\n");
			rc = -EIO;
			goto error5;
		}
		
		// printk(KERN_INFO "[cnn_probe] dma base address start at %x\n", dma_p->base_addr);
		
		/* Get irq number */
/*		
		
		dma_p->irq_num = platform_get_irq(pdev, 0);
		if(!dma_p->irq_num)
		{
			printk(KERN_ERR "[cnn_probe] Could not get IRQ resource\n");
			rc = -ENODEV;
			goto error5;
		}

		if (request_irq(dma_p->irq_num, dma_MM2S_isr, 0, DEVICE_NAME, dma_p)) {
			printk(KERN_ERR "[cnn_probe] Could not register IRQ %d\n", dma_p->irq_num);
			return -EIO;
			goto error6;
		}
		else {
			printk(KERN_INFO "[cnn_probe] Registered MM2S IRQ %d\n", dma_p->irq_num);
		}
		if(!dma_p->irq_num)
		{
			printk(KERN_ERR "[cnn_probe] Could not get IRQ resource\n");
			rc = -ENODEV;
			goto error5;
		}

		if (request_irq(dma_p->irq_num, dma_S2MM_isr, 0, DEVICE_NAME, dma_p)) {
			printk(KERN_ERR "[cnn_probe] Could not register IRQ %d\n", dma_p->irq_num);
			return -EIO;
			goto error6;
		}
		else {
			printk(KERN_INFO "[cnn_probe] Registered S2MM IRQ %d\n", dma_p->irq_num);
		}
*/
		/* INIT DMA */
		dma_init(dma_p->base_addr);
		
		// printk(KERN_NOTICE "[cnn_probe] CNN platform driver registered - dma\n");
		device_fsm++;	
		return 0;

		error6:
			iounmap(dma_p->base_addr);
		error5:
			release_mem_region(dma_p->mem_start, dma_p->mem_end - dma_p->mem_start + 1);
			kfree(dma_p);
		error4:
			return rc;			
	break;
	
	
	case 1:
		
		/* Get physical register address space from device tree */
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if(!r_mem)
		{
			printk(KERN_ALERT "cnn_probe: Failed to get reg resource.\n");
			return -ENODEV;
		}
		
		printk(KERN_ALERT "[cnn_probe] Probing cnn_p\n");
		
		/* Allocate memory space for structure cnn_info */ 
		cnn_p = (struct cnn_info *) kmalloc(sizeof(struct cnn_info), GFP_KERNEL);
		if(!cnn_p) 
		{
			printk(KERN_ALERT "[cnn_probe] Could not allocate CNN device\n");
			return -ENOMEM;
		}

		/* Put phisical addresses in cnn_info structure */
		cnn_p->mem_start = r_mem->start;
		cnn_p->mem_end = r_mem->end;

		/* Reserve that memory space for this driver */
		if(!request_mem_region(cnn_p->mem_start, cnn_p->mem_end - cnn_p->mem_start + 1,	DEVICE_NAME)) 
		{
			printk(KERN_ALERT "[cnn_probe] Could not lock memory region at %p\n",(void *)cnn_p->mem_start);
			rc = -EBUSY;
			goto error1;
		}

		/* Remap physical addresses to virtual addresses */
		cnn_p->base_addr = ioremap(cnn_p->mem_start, cnn_p->mem_end - cnn_p->mem_start + 1);
		if (!cnn_p->base_addr) 
		{
			printk(KERN_ALERT "[cnn_probe] Could not allocate memory\n");
			rc = -EIO;
			goto error2;
		}
		
		// printk(KERN_INFO "[cnn_probe] cnn-ip base address start at %x\n", cnn_p->base_addr);
			
		/* Get irq number */
		cnn_p->irq_num = platform_get_irq(pdev, 0);
		if(!cnn_p->irq_num)
		{
			printk(KERN_ERR "[cnn_probe] Could not get IRQ resource\n");
			rc = -ENODEV;
			goto error2;
		}

		if (request_irq(cnn_p->irq_num, cnn_isr, IRQF_TRIGGER_RISING, DEVICE_NAME, cnn_p)) {
			printk(KERN_ERR "[cnn_probe] Could not register IRQ %d\n", cnn_p->irq_num);
			return -EIO;
			goto error3;
		}
		else {
			printk(KERN_INFO "[cnn_probe] Registered IRQ %d\n", cnn_p->irq_num);
		}

		enable_irq(cnn_p->irq_num);
		iowrite32(IP_COMMAND_RESET, cnn_p->base_addr);
		// printk(KERN_INFO "[cnn_probe] CNN IP reset\n");

		// printk(KERN_NOTICE "[cnn_probe] CNN platform driver registered - cnn-ip \n");
		return 0;

		error3:
			iounmap(cnn_p->base_addr);
		error2:
			release_mem_region(cnn_p->mem_start, cnn_p->mem_end - cnn_p->mem_start + 1);
			kfree(cnn_p);
		error1:
			return rc;			
	break;
	
	default:
		// printk(KERN_INFO "[cnn_probe] Device FSM in illegal state.\n");
		return -1;
	break;
    }
}

static int cnn_remove(struct platform_device *pdev) 
{
	switch (device_fsm)
	{
	case 0: 
		printk(KERN_ALERT "[cnn_remove] cnn_p device platform driver removed\n");
		// iowrite32(0, cnn_p->base_addr);
		free_irq(cnn_p->irq_num, cnn_p);
		// printk(KERN_INFO "[cnn_remove] IRQ number for cnn free\n");
		iounmap(cnn_p->base_addr);
		release_mem_region(cnn_p->mem_start, cnn_p->mem_end - cnn_p->mem_start + 1);
		kfree(cnn_p);
	break;

	case 1:
		printk(KERN_ALERT "[cnn_remove] dma_p platform driver removed\n");
		// iowrite32(0, dma_p->base_addr);
		free_irq(dma_p->irq_num, NULL);
		// printk(KERN_INFO "[cnn_remove] IRQ number for dma free\n");
		iounmap(dma_p->base_addr);
		release_mem_region(dma_p->mem_start, dma_p->mem_end - dma_p->mem_start + 1);
		kfree(dma_p);
		--device_fsm;
	break;

	default:
		// printk(KERN_INFO "[cnn_remove] Device FSM in illegal state. \n");
		return -1;
	}
	
	// printk(KERN_INFO "[cnn_remove] Succesfully removed driver\n");
	return 0;
}

/* -------------------------------------- */
/* ------OPEN AND CLOSE FUNCTIONS-------- */
/* -------------------------------------- */

int cnn_open(struct inode *pinode, struct file *pfile)
{
//	printk(KERN_INFO "CNN FILE OPENED\n");
	return 0;
}

int cnn_close(struct inode *pinode, struct file *pfile)
{
//	printk(KERN_INFO "CNN FILE CLOSE\n");
	return 0;
}


/* -------------------------------------- */
/* -------READ AND WRITE FUNCTIONS------- */
/* -------------------------------------- */

int transaction_over = 0;
volatile int ip_process_over = 0;
int input_command;

ssize_t cnn_read(struct file *pfile, char __user *buf, size_t length, loff_t *offset)
{		
	int minor = MINOR(pfile->f_inode->i_rdev);

	switch(minor)
	{
	/* Reading from CNN */
	case 0:
		// NOTHING TO DO HERE
		printk(KERN_WARNING "[cnn_read] Reading from CNN not allowed\n");
		
	break;
	
	/* Reading from DMA */
	case 1:
		// NOTHING TO DO HERE
		printk(KERN_WARNING "[cnn_read] Reading from DMA not allowed. Data should be memory mapped using mmap and memcpy functions from inside app.\n");
	break;
	
	default:
		printk(KERN_WARNING "[cnn_read] Invalid read command\n");
	break;
	}
	
	return 0;
}

ssize_t cnn_write(struct file *pfile, const char __user *buf, size_t length, loff_t *offset)
{
	char buff[BUFF_SIZE]; 
	int ret = 0;
	int dumb =0;
	int minor = MINOR(pfile->f_inode->i_rdev);
	ret = copy_from_user(buff, buf, length);  
	if(ret)
	{
		printk(KERN_WARNING "[cnn_write] Copy from user failed\n");
		return -EFAULT;
	}  
	buff[length] = '\0';
	
	switch(minor)
	{
		/* Writing into CNN */
		case 0:
			// printk(KERN_INFO "[cnn_write] Writing into cnn-ip");
			sscanf(buff, "%d", &input_command);  
			
			/* Check if command is valid */
	
			if(input_command != IP_COMMAND_LOAD_BIAS 		&&
			   input_command != IP_COMMAND_LOAD_WEIGHTS0 		&&
			   input_command != IP_COMMAND_LOAD_CONV0_INPUT 	&&
			   input_command != IP_COMMAND_START_CONV0		&&
			   input_command != IP_COMMAND_LOAD_WEIGHTS1 		&&
			   input_command != IP_COMMAND_LOAD_CONV1_INPUT 	&&
			   input_command != IP_COMMAND_START_CONV1		&&
			   input_command != IP_COMMAND_LOAD_WEIGHTS2 		&&
			   input_command != IP_COMMAND_LOAD_CONV2_INPUT 	&&
			   input_command != IP_COMMAND_START_CONV2		&&
			   input_command != IP_COMMAND_RESET	 		&&
			   input_command != IP_COMMAND_READ_CONV0_OUTPUT	&&
			   input_command != IP_COMMAND_READ_CONV1_OUTPUT	&&
			   input_command != IP_COMMAND_READ_CONV2_OUTPUT)
			{
				printk(KERN_WARNING "[cnn_write] Wrong CNN command! %d\n", input_command);
				return 0;
			}
		
			
			/* Start DMA to send data if LOAD command is issued */

			switch(input_command)
			{
			// Write command 
			case IP_COMMAND_LOAD_BIAS:
				dma_simple_write(tx_phy_buffer, BIAS_INPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: LOAD BIAS\n");
			break;
			
			case IP_COMMAND_LOAD_WEIGHTS0:
				dma_simple_write(tx_phy_buffer, CONV0_WEIGHTS_INPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: LOAD WEIGHTS0\n");
			break;
			
			case IP_COMMAND_LOAD_CONV0_INPUT:
				dma_simple_write(tx_phy_buffer, CONV0_PICTURE_INPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: LOAD INPUT PICTURE CONV0\n");
			break;
			
			case IP_COMMAND_LOAD_WEIGHTS1:
				dma_simple_write(tx_phy_buffer, CONV1_WEIGHTS_INPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: LOAD WEIGHTS1\n");
			break;
			
			case IP_COMMAND_LOAD_CONV1_INPUT:
				dma_simple_write(tx_phy_buffer, CONV1_PICTURE_INPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: LOAD INPUT PICTURE CONV1\n");
			break;
			
			case IP_COMMAND_LOAD_WEIGHTS2:
				dma_simple_write(tx_phy_buffer, CONV2_WEIGHTS_INPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: LOAD WEIGHTS2\n");
			break;
			
			case IP_COMMAND_LOAD_CONV2_INPUT:
				dma_simple_write(tx_phy_buffer, CONV2_PICTURE_INPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: LOAD INPUT PICTURE CONV2\n");
			break;
			
			
			// Read command 
			
			case IP_COMMAND_READ_CONV0_OUTPUT:
				dma_simple_read(tx_phy_buffer, CONV0_PICTURE_OUTPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: READ CONV0 OUTPUT\n");
			break;
			
			case IP_COMMAND_READ_CONV1_OUTPUT:
				dma_simple_read(tx_phy_buffer, CONV1_PICTURE_OUTPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: READ CONV1 OUTPUT\n");
			break;
			
			case IP_COMMAND_READ_CONV2_OUTPUT:
				dma_simple_read(tx_phy_buffer, CONV2_PICTURE_OUTPUT_LEN, dma_p->base_addr);
				// printk(KERN_INFO "[cnn_write] Starting DMA transaction: READ CONV2 OUTPUT\n");
			break;
			
			default:
				// NOT A LOAD OR READ COMMAND
			break;
			}
			/* Write into CNN IP */
			ip_process_over = 1;
			iowrite32((u32)input_command, cnn_p->base_addr);
			if(input_command != IP_COMMAND_RESET)
			{
				while(ip_process_over == 1);
			}

		// printk(KERN_INFO "[cnn_write] Writing finished!");
		ip_process_over = 0;
		transaction_over = 0;
		break;
		
		/* Writing into DMA */
		case 1:
			// NOTHING TO DO HERE
		//	printk(KERN_WARNING "[cnn_write] Writing into DMA not allowed. Data should be memory mapped using mmap and memcpy functions from inside app.\n");
		break;
		
		default:
			printk(KERN_WARNING "[cnn_write] Invalid write command\n");
		break;
	}
	
	return length;
}

/* -------------------------------------- */
/* ------------MMAP FUNCTION------------- */
/* -------------------------------------- */

static int cnn_mmap(struct file *f, struct vm_area_struct *vma_s)
{
	int ret = 0;
	long length = vma_s->vm_end - vma_s->vm_start;

	// printk(KERN_INFO "[cnn_dma_mmap] DMA TX Buffer is being memory mapped\n");

	if(length > 32*32*32*2)
	{
		return -EIO;
		printk(KERN_ERR "[cnn_dma_mmap] Trying to mmap more space than it's allocated\n");
	}

	ret = dma_mmap_coherent(my_device_dma, vma_s, tx_vir_buffer, tx_phy_buffer, length);
	if(ret < 0)
	{
		printk(KERN_ERR "[cnn_dma_mmap] Memory map failed\n");
		return ret;
	}
	return 0;
}


/* -------------------------------------- */
/* ------INTERRUPT SERVICE ROUTINES------ */
/* -------------------------------------- */

static irqreturn_t dma_MM2S_isr(int irq, void* dev_id)
{
	unsigned int IrqStatus;  
	
	/* DMA transaction has been complited and interrupt occures, flag needs to be cleared */
	/* Clearing MM2S flag */
	
	IrqStatus = ioread32(dma_p->base_addr + MM2S_STATUS_REGISTER);
	iowrite32(IrqStatus | 0x00005000, dma_p->base_addr + MM2S_STATUS_REGISTER);
	
	/* Tell rest of the code that interrupt has happened */
	transaction_over = 0;
	
	// printk(KERN_INFO "[dma_MM2S_isr] Finished DMA MM2S transaction!\n");

	return IRQ_HANDLED;
}

static irqreturn_t dma_S2MM_isr(int irq, void*dev_id)
{
	unsigned int IrqStatus;  
	
	/* DMA transaction has been complited and interrupt occures, flag needs to be cleared */
	/* Clearing S2MM flag */
	
	IrqStatus = ioread32(dma_p->base_addr + S2MM_STATUS_REGISTER);
	iowrite32(IrqStatus | 0x00005000, dma_p->base_addr + S2MM_STATUS_REGISTER);
	
	/* Tell rest of the code that interrupt has happened */
	transaction_over = 0;
	
	// printk(KERN_INFO "[dma_S2MM_isr] Finished DMA S2MM transaction!\n");

	return IRQ_HANDLED;
}

static irqreturn_t cnn_isr(int irq, void*dev_id)
{
	ip_process_over = 0;
	//printk(KERN_INFO "[cnn_isr] IP finished operation %x\n", input_command);
	return IRQ_HANDLED;
}

/* -------------------------------------- */
/* ------------DMA FUNCTIONS------------- */
/* -------------------------------------- */

int dma_init(void __iomem *base_address)
{
	/* 
	 * In order for DMA to work proprely, it's internal control registers should be configurated first 
	 * There is a series of steps needed to be complited before every DMA transcation
	 * This one is the initial step that does the following inside a memory-mapped MM2S_DMACR and S2MM_DMACR register:
	 *  - Reset DMA by setting bit 3
	 *  - Allow interrupts by setting bits 12 and 14 (these interrupts will signal the CPU when the transaction is complited or an error has accured)
	*/
	
	u32 MM2S_DMACR_reg = 0;
	u32 S2MM_DMACR_reg;
	u32 en_interrupt = 0;
	u32 temp = 0;
	
	// For debug purpose first we read status register
	temp = ioread32(base_address + 4);
	//// printk(KERN_INFO "Initial state of STATUS reg is %u\n", temp);	

	/* Writing to MM2S_DMACR register. Setting reset bit (3rd bit) */
	iowrite32(DMACR_RESET, base_address + MM2S_CONTROL_REGISTER);

	// printk(KERN_INFO "[dma_init] Writing %d into %x", DMACR_RESET, base_address+MM2S_CONTROL_REGISTER);
	temp = ioread32(base_address + 0);
	// printk(KERN_INFO "[debug - ioread] After reseting control reg is %u\ [should be 65538 probably]\n", temp);

	
	/* Reading from MM2S_DMACR register inside DMA */
	MM2S_DMACR_reg = ioread32(base_address + MM2S_CONTROL_REGISTER); 
	// printk(KERN_INFO "[debug - ioread] Reading control reg is %u [probably should be still 65538]\n", MM2S_DMACR_reg);
	

	/* Setting 13th and 15th bit in MM2S_DMACR to enable interrupts */
	en_interrupt = MM2S_DMACR_reg | IOC_IRQ_FLAG | ERR_IRQ_EN;
	// printk(KERN_INFO "[debug] int flag is %u\n", en_interrupt);

	iowrite32(en_interrupt, base_address + MM2S_CONTROL_REGISTER);
	// printk(KERN_INFO "[dma_init] To enable interrupt and error check, writing %d into %x", en_interrupt, base_address+MM2S_CONTROL_REGISTER);
	temp = ioread32(base_address + 0);
	// printk(KERN_INFO "[debug - iowrite/ioread] After enabling interrupt and error check, control reg is %u [should be 86018]\n", temp);
	
	/* Same steps should be taken for S2MM_DMACR register */

	/* Writing to S2MM_DMACR register. Setting reset bit (3rd bit) */
	iowrite32(DMACR_RESET, base_address + S2MM_CONTROL_REGISTER);

	/* Reading from S2MM_DMACR register inside DMA */
	S2MM_DMACR_reg = ioread32(base_address + S2MM_CONTROL_REGISTER); 
	
	/* Setting 13th and 15th bit in S2MM_DMACR to enable interrupts */
	en_interrupt = S2MM_DMACR_reg | IOC_IRQ_FLAG | ERR_IRQ_EN;
	iowrite32(en_interrupt, base_address + S2MM_CONTROL_REGISTER);

	// printk(KERN_INFO "[dma_init] DMA init done\n");
	return 0;
}

unsigned int dma_simple_write(dma_addr_t TxBufferPtr, unsigned int pkt_len, void __iomem *base_address) 
{
	u32 MM2S_DMACR_reg = 0;
	u32 temp = 0;
	u32 en_interrupt;	
	
	MM2S_DMACR_reg = ioread32(base_address + MM2S_CONTROL_REGISTER); 
	
	en_interrupt = MM2S_DMACR_reg | IOC_IRQ_FLAG | ERR_IRQ_EN;
	// // printk(KERN_INFO "[debug] int flag is %u\n", en_interrupt);
	
	iowrite32(en_interrupt, base_address + MM2S_CONTROL_REGISTER);
	// // printk(KERN_INFO "[dma_init] To enable interrupt and error check, writing %d into %x", en_interrupt, base_address+MM2S_CONTROL_REGISTER);


	/* READ from MM2S_DMACR register */
	MM2S_DMACR_reg = ioread32(base_address + MM2S_CONTROL_REGISTER);	

	// // printk(KERN_INFO "[debug - ioread] Initial control register before any changes inside dma_simple_write: %u [should be 86018]\n", MM2S_DMACR_reg);


	temp = ioread32(base_address + 4);
	// // printk(KERN_INFO "[debug] Before starting DMA, STATUS reg LSB bit is %u [should be 1 - halted]\n", temp & 0x1);


	/* Set RS bit in MM2S_DMACR register (this bit starts the DMA) */
	iowrite32(0x1 |  MM2S_DMACR_reg, base_address + MM2S_CONTROL_REGISTER);
	
	// // printk(KERN_INFO "[dma_simple_write] Writing %d at address %x\n", 0x1 | MM2S_DMACR_reg, base_address + MM2S_CONTROL_REGISTER);
	temp = ioread32(base_address + 0);
	// // printk(KERN_INFO "[debug - iowrite/ioread] After starting RS bit, control register is %u [should be 68019]\n", temp);


	temp = ioread32(base_address + 4);
	// // printk(KERN_INFO "[debug] After starting DMA, STATUS reg LSB bit is %u [should be 0 - running]\n", temp & 0x1);


	/* Write into MM2S_SA register the value of TxBufferPtr. 
	 * With this, the DMA knows from where to start - this is the first address of data that needs to be transfered. 
	*/
	iowrite32((u32)TxBufferPtr, base_address + MM2S_CONTROL_REGISTER + MM2S_SRC_ADDRESS_REGISTER);

	// // printk(KERN_INFO "[dma_simple_write] Writing starting buffer address %x at address %x\n", (int)TxBufferPtr, base_address + MM2S_CONTROL_REGISTER + MM2S_SRC_ADDRESS_REGISTER);
	temp = ioread32(base_address + 0x18);
	// // printk(KERN_INFO "[debug - iowrite/ioread] After writing starting address: %u [should be value from previous message]\n", temp);
	


	/* Write into MM2S_LENGTH register. This is the length of a tranaction. */
	iowrite32(pkt_len, base_address + MM2S_CONTROL_REGISTER + MM2S_TRNSFR_LENGTH_REGISTER);
	// // printk(KERN_INFO "[dma_simple_write] Writing length of transaction %d at address %x\n", pkt_len, base_address + MM2S_CONTROL_REGISTER + MM2S_TRNSFR_LENGTH_REGISTER);
	temp = ioread32(base_address + 0x28);
	// // printk(KERN_INFO "[debug - iowrite/ioread] After writing length: %u [should be 128 for bias]\n", temp);
	return 0;
}


unsigned int dma_simple_read(dma_addr_t TxBufferPtr, unsigned int pkt_len, void __iomem *base_address) 
{
	u32 S2MM_DMACR_reg;

	/* READ from S2MM_DMACR register */
	S2MM_DMACR_reg = ioread32(base_address + S2MM_CONTROL_REGISTER);

	/* Set RS bit in S2MM_DMACR register (this bit starts the DMA) */
	iowrite32(0x1 |  S2MM_DMACR_reg, base_address + S2MM_CONTROL_REGISTER);

	/* Write into S2MM_SA register the value of TxBufferPtr. 
	 * With this, the DMA knows from where to start writing into - this is the first address of data that needs to be transfered. 
	*/
	iowrite32((u32)TxBufferPtr, base_address + S2MM_DST_ADDRESS_REGISTER); 
	
	/* NOTE: no need for: base_address + S2MM_DST_ADDRESS_REGISTER + S2MM_CONTROL_REGISTER since 
	 * S2MM_CONTROL_REGISTER address is alreay accounted for inside S2MM_DST_ADDRESS_REGISTER
	*/

	/* Write into S2MM_LENGTH register. This is the length of a tranaction. */
	iowrite32(pkt_len, base_address + S2MM_BUFF_LENGTH_REGISTER);
	return 0;
}
