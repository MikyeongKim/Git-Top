#include <linux/module.h>
#include <linux/tty.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define MODULE_VERSION	"1.0"

// ����� �̸��̴�. 
// �� ����̸����� /proc�ؿ� ���丮�� �����ȴ�. 
#define MODULE_NAME		"myproc"

// ������ /proc/MODULE_NAME �ؿ� �Ʒ� 2���� ������ 
// �����ȴ�. 
#define FOO_FILE		"foo"
#define JIFFIE_FILE		"jiffies" 

#define FOOBAR_LEN 8

// foo ���Ͽ� ����� ������ ����ü
struct fb_data_t
{
		char name[FOOBAR_LEN + 1];
			char value[FOOBAR_LEN + 1];
};
struct fb_data_t foo_data;


static struct proc_dir_entry *example_dir, *foo_file, *jiffies_file;

// ����ڰ� cat���� ���ؼ� /proc/[MODULE_NAME]/jiffies������ ���� 
// Ŀ���� �� �Լ��� ȣ���ؼ� �ش� ������ �Ѱ��ش�. 
static int proc_read_jiffies(char *page, char **start, off_t off,
					   int count, int *eof, void *data)
{
		int len;

			MOD_INC_USE_COUNT;
				// ����ڰ� �����ϱ� ���� �������� �����. 
				// cat, vi���� ����ؼ� �� ������ ����� 
				// jiffies = 1234 �� ���� �������� ���δ�. 
				len = sprintf(page, "jiffies = %ld\n", jiffies);
					// �ش� ������ printk�� ���ؼ� �α׷� �����.  
					// �� �����ʹ� /var/log/message�� ��µȴ�.
					printk("<1> read jiffies = %ld\n", jiffies);
						MOD_DEC_USE_COUNT;

							return len;
}

// ����ڰ� /proc/[MODULE_NAME]/foo ������ ������ �� 
// ������ִ� ���� 
static int proc_read_foobar(char *page, char **start, off_t off, 
									int count, int *eof, void *data)
{
		int len;
			struct fb_data_t *fb_data = (struct fb_data_t *)data;

				MOD_INC_USE_COUNT;
					// fb_data����ü�� ������ ���ý��� ���� ������ش�. 
					len = sprintf(page, "%s = %s",
												fb_data->name, fb_data->value);
						MOD_DEC_USE_COUNT;
							return len;
}

// ����ڴ� /proc/[MODULE_NAME]/foo�� ������ ���⸦ ���Ҷ��� ���� ���̴�.
// �� �� ���Լ��� ȣ��ȴ�.
static int proc_write_foobar(struct file *foke, const char *buffer,
									unsigned long count, void *data)
{
		int len;
			struct fb_data_t *fb_data = (struct fb_data_t *)data;

				MOD_INC_USE_COUNT;
					if (count > FOOBAR_LEN)
								len = FOOBAR_LEN;
						else
									len = count;

							printk("<1> DATA COPY %d\n", len);
								// echo "xxxxx" > /proc/[MODULE_NAME]/foo ������ �Է¹��� ����
								// fb_data->value�� �����Ѵ�. 
								if (copy_from_user(fb_data->value, buffer, len))
										{
													MOD_DEC_USE_COUNT;
															return -EFAULT;
																}

									fb_data->value[len] = 0x00;
										MOD_DEC_USE_COUNT;

											return len;

}

// Ŀ�� ��� �ʱ�ȭ �Լ�
static int init_myproc(void)
{
		int rv = 0;

			printk("<1> Module Start\n");
				example_dir = proc_mkdir(MODULE_NAME, NULL);
					if (example_dir == NULL)
							{
										rv = -ENOMEM;
												printk("<1> mkdir failure\n");
														goto out;
															}

						// JIFFILE������ ��� ���� �б⸸ ����Ѵ�.
						example_dir->owner = THIS_MODULE;
							jiffies_file = create_proc_read_entry(JIFFIE_FILE, 0444, example_dir,
																		proc_read_jiffies, NULL);
								if (jiffies_file == NULL)
										{
													rv = -ENOMEM;
															printk("<1> read entry failure\n");
																	goto no_jiffies;
																		}
									printk("<1> OK MAKE MODULE\n");
										jiffies_file->owner = THIS_MODULE; 

											// FOO_FILE�� ��� �б�� ���� ��� �����ص��� �ؾ� �ϸ� 
											// ������ ��쿡 ȣ��� �Լ��� ������ ��� �Ѵ�. 
											foo_file = create_proc_entry(FOO_FILE, 0644, example_dir);
												if (foo_file == NULL)
														{
																	rv = -ENOMEM;
																			goto no_foo;
																				}

													strcpy(foo_data.name, "foo");
														strcpy(foo_data.value, "dream");

															// Ŀ�� �ڷᱸ���� ����� ������ 
															foo_file->data = &foo_data;
																// �б⸦ ��û���� �� ȣ��� �Լ�
																foo_file->read_proc = proc_read_foobar;
																	// ���Ⱑ ��û�Ǿ��� �� ȣ��� �Լ�
																	foo_file->write_proc = proc_write_foobar;
																		foo_file->owner = THIS_MODULE;

																			printk("<1> %s %s initialised\n", "myproc", "1.0");

																				// ����� �ʱ�ȭ�� �̷�����ٸ� 0�� �����ؾ� �Ѵ�. 
																				return 0;
																					no_foo:
																						remove_proc_entry(FOO_FILE, example_dir);
																							no_jiffies:
																								remove_proc_entry(MODULE_NAME, NULL);
																									out:
																										return rv;
}

// cleanup �Լ�
static void cleanup_myproc(void) 
{
		printk("<1> END\n"); 
			remove_proc_entry(FOO_FILE, example_dir);
				remove_proc_entry(JIFFIE_FILE, example_dir);
}

module_init(init_myproc);
module_exit(cleanup_myproc);

MODULE_AUTHOR("yundream");
MODULE_DESCRIPTION("Sanmpe proc");
