/*
 *  fourinarow.c - Connect 4 module.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/random.h>

#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include <asm/uaccess.h>
#include <asm/errno.h>


#define DEV_CLASS_MODE ((umode_t)(S_IRUGO|S_IWUGO))


static char *my_class_devnode(struct device *dev, umode_t *mode);
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file* filp, char __user *buff, size_t len, loff_t* off);
static ssize_t device_write(struct file* filp, const char __user *buff, size_t len, loff_t* off);

//Connect 4
static void setupBoard(void);
static int checkBoard(void);
static void place(int col);
static void randomize(void);

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_release,
	.read = device_read,
        .write = device_write
};

struct mychar_device_data {
        struct cdev cdev;
};

//Connect 4 Vars
static char** board;
static int* column;
static int* rem;
static int* random;
static char pChoice, cChoice;
static int turn;

//character device driver vars
static int majorNumber = 0;
static char msg[1000];
static struct class *chardev_class = NULL;
static struct mychar_device_data char_data;

int init_module(void){
	int err, i;
	dev_t dev;

	err = alloc_chrdev_region(&dev, 0, 1, "fourinarow");
	majorNumber = MAJOR(dev);

	chardev_class = class_create(THIS_MODULE, "fourinarow");
	chardev_class->devnode = my_class_devnode;
	cdev_init(&char_data.cdev, &fops);
	char_data.cdev.owner = THIS_MODULE;
	char_data.cdev.ops = &fops;

	cdev_add(&char_data.cdev, MKDEV(majorNumber, 0), 1);
	device_create(chardev_class, NULL, MKDEV(majorNumber, 0), NULL, "fourinarow");
	//Connect 4
	board = (char**)kmalloc(sizeof(char*) * 8, GFP_KERNEL);
	rem = kmalloc(sizeof(int) * 8, GFP_KERNEL);
	random = kmalloc(sizeof(int) * 8, GFP_KERNEL);
	column = kmalloc(sizeof(int) * 8, GFP_KERNEL);
	turn = -1;
	for(i = 0; i < 8; i++){
		board[i] = kmalloc(sizeof(char*) * 8, GFP_KERNEL);
	}
	rem[0] = 3;
	rem[1] = 5;
	rem[2] = 7;
	rem[3] = 11;
	rem[4] = 13;
	rem[5] = 17;
	rem[6] = 19;
	rem[7] = 23;
	setupBoard();
	return 0;
}

void cleanup_module(void){
	int i;
	device_destroy(chardev_class, MKDEV(majorNumber, 0));

	class_unregister(chardev_class);
	class_destroy(chardev_class);

	unregister_chrdev_region(MKDEV(majorNumber, 0), MINORMASK);
	//Connect 4
	for(i = 0; i < 8; i++){
		kfree(board[i]);
	}
	kfree(board);
	kfree(rem);
	kfree(random);
	kfree(column);
}

//fops functions
static int device_open(struct inode *inode, struct file *file){
	printk("Device open\n");
	return 0;
}

static int device_release(struct inode *inode, struct file *file){
	printk("Device close\n");
	return 0;
}

static ssize_t device_read(struct file* filp, char __user *buff, size_t len, loff_t* off){
	printk("Read!\n");
	return simple_read_from_buffer(buff, len, off, msg, 1000);
}

static ssize_t device_write(struct file* filp, const char __user *buff, size_t len, loff_t* off){
	int i, result, check;
	char lines[100];
	char input[100];
	if(copy_from_user(input, buff, len)){
		return -EFAULT;
	}
	input[len] = '\0';
	memset(msg, 0, sizeof(msg));
	if(strcmp(input, "RESET R\n") == 0){
		printk("Reseting Player using: R\n");
		pChoice = 'R';
		cChoice = 'Y';
		setupBoard();
		sprintf(msg, "OK\n");
	} else if(strcmp(input, "RESET Y\n") == 0){
		printk("Reseting Player using: Y\n");
                pChoice = 'Y';
                cChoice = 'R';
                setupBoard();
		sprintf(msg, "OK\n");
	} else if(strcmp(input, "BOARD\n") == 0){
		printk("Printing BOARD\n");
		sprintf(msg, "  A B C D E F G H\n-----------------\n");
		for(i = 0; i < 8; i++){
			sprintf(lines, "%d|%c %c %c %c %c %c %c %c\n", 8 - i, board[i][0], board[i][1], board[i][2], board[i][3], board[i][4], board[i][5], board[i][6], board[i][7]);
			strcat(msg, lines);
			memset(lines, 0, 100);
		}
        } else if(strcmp(input, "DROPC A\n") == 0){
		printk("DROPCing A\n");
		place(0);
        } else if(strcmp(input, "DROPC B\n") == 0){
		printk("DROPCing B\n");
		place(1);
        } else if(strcmp(input, "DROPC C\n") == 0){
		printk("DROPCing C\n");
		place(2);
        } else if(strcmp(input, "DROPC D\n") == 0){
		printk("DROPCing D\n");
		place(3);
        } else if(strcmp(input, "DROPC E\n") == 0){
		printk("DROPCing E\n");
		place(4);
        } else if(strcmp(input, "DROPC F\n") == 0){
		printk("DROPCing F\n");
		place(5);
        } else if(strcmp(input, "DROPC G\n") == 0){
		printk("DROPCing G\n");
		place(6);
        } else if(strcmp(input, "DROPC H\n") == 0){
		printk("DROPCing H\n");
		place(7);
        } else if(strcmp(input, "CTURN\n") == 0){
		printk("CTURN Dropping\n");
		if(turn == -1){
			sprintf(msg, "NOGAME\n");
		} else if(turn == 0){
			sprintf(msg, "OOT\n");
		} else {
			randomize();
			check = 0;
			//add to column
			for(i = 0; i < 8; i++){
				if(column[random[i]] > 0){
					board[column[random[i]]--][random[i]] = cChoice;
					i = 8;
					check = 1;
				}
			}
			if(check == 0){
				for(i = 0; i < 8; i++){
   	                             if(column[i] > 0){
        	                                board[column[i]--][i] = cChoice;
                	                        i = 8;
						check = 1;
                        	        }
				}
                        }

                        result = checkBoard();
			turn--;
                        if(result == -1){
				 if(column[0] == -1 && column[1] == -1 && column[2] == -1 && column[3] == -1
                                 && column[4] == -1 && column[5] == -1 && column[6] == -1 && column[7] == -1){
                                        sprintf(msg , "TIE\n");
                                 } else {
                                	sprintf(msg, "OK\n");
				 }
                        } else if(result == 1){
                                sprintf(msg, "LOSE\n");
				turn = -1;
                        }
		}

        } else {
		sprintf(msg, "OOT\n");
		printk("Invalid");
        }
	msg[sizeof(msg) -1] = '\0';
	return len;
}

static char *my_class_devnode(struct device *dev, umode_t *mode){
    if (mode != NULL){
        *mode = DEV_CLASS_MODE;
    }
        return NULL;
}

//Connect 4 Functions
static void setupBoard(void){
	int i, j;
        for(i = 0; i < 8; i++){
                for(j = 0; j < 8; j++){
                        board[i][j] = '0';
                }
        }
	for(i = 0; i < 8; i++){
		column[i] = 7;
	}
	turn = 0;
}

static int checkBoard(void){
	int i, j;
	for(i = 0; i < 8; i++){
		for(j = 0; j < 8; j++){
			if(i <= 4 && board[i][j] == board[i+1][j] && board[i+1][j] == board[i+2][j]
			&& board[i+2][j] == board[i+3][j]){
				if(pChoice == board[i][j]){
					return 0;
				} else if(cChoice == board[i][j]){
					return 1;
				}
			} else if(j <= 4 && board[i][j] == board[i][j+1] && board[i][j+1] == board[i][j+2]
			&& board[i][j+2] == board[i][j+3]){
				if(pChoice == board[i][j]){
					return 0;
				} else if(cChoice == board[i][j]){
					return 1;
				}
			} else if(i <= 4 && j <= 4 && board[i][j] == board[i+1][j+1] && board[i+1][j+1] == board[i+2][j+2]
 			&& board[i+2][j+2] == board[i+3][j+3]){
                                if(pChoice == board[i][j]){
                                        return 0;
                                } else if(cChoice == board[i][j]){
                                        return 1;
                                }
			} else if(i <= 4 && j >=3 && board[i][j] == board[i+1][j-1] && board[i+1][j-1] == board[i+2][j-2]
			&& board[i+2][j-2] == board[i+3][j-3]){
                                if(pChoice == board[i][j]){
                                        return 0;
                                } else if(cChoice == board[i][j]){
                                        return 1;
                                }
			}
		}
	}
	return -1;
}

static void place(int col){
	int result;
	if(turn == -1){
		sprintf(msg, "NOGAME\n");
	} else if(turn == 1){
                 sprintf(msg, "OOT\n");
        } else {
                if(column[col] <= -1){
			sprintf(msg, "OOT\n");
                } else {
                        board[column[col]--][col] = pChoice;
                        result = checkBoard();
			turn++;
                        if(result == -1){
				if(column[0] == -1 && column[1] == -1 && column[2] == -1 && column[3] == -1
				&& column[4] == -1 && column[5] == -1 && column[6] == -1 && column[7] == -1){
					sprintf(msg , "TIE\n");
				} else {
                        		sprintf(msg, "OK\n");
				}
			} else if(result == 0){
                                sprintf(msg, "WIN\n");
				turn = -1;
                        }
                }
	}
}

static void randomize(void){
	int i;
	int rand = get_random_int() % 8 + 1;
	for(i = 0; i < 8; i++){
		random[i] = (rand * rem[i] + i) % 8;
	}
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul Abili");
MODULE_DESCRIPTION("A Connect 4 Game");
