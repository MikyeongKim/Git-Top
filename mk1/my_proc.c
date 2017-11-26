#include <linux/module.h>
#include <linux/tty.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define MODULE_VERSION	"1.0"

// 모듈의 이름이다. 
// 이 모듈이름으로 /proc밑에 디렉토리가 생성된다. 
#define MODULE_NAME		"myproc"

// 생성된 /proc/MODULE_NAME 밑에 아래 2개의 파일이 
// 생성된다. 
#define FOO_FILE		"foo"
#define JIFFIE_FILE		"jiffies" 

#define FOOBAR_LEN 8

// foo 파일에 저장될 데이터 구조체
struct fb_data_t
{
		char name[FOOBAR_LEN + 1];
			char value[FOOBAR_LEN + 1];
};
struct fb_data_t foo_data;


static struct proc_dir_entry *example_dir, *foo_file, *jiffies_file;

// 사용자가 cat등을 통해서 /proc/[MODULE_NAME]/jiffies파일을 열면 
// 커널은 이 함수를 호출해서 해당 정보를 넘겨준다. 
static int proc_read_jiffies(char *page, char **start, off_t off,
					   int count, int *eof, void *data)
{
		int len;

			MOD_INC_USE_COUNT;
				// 사용자가 이해하기 쉬운 포맷으로 만든다. 
				// cat, vi등을 사용해서 이 파일을 열경우 
				// jiffies = 1234 와 같은 형식으로 보인다. 
				len = sprintf(page, "jiffies = %ld\n", jiffies);
					// 해당 내용을 printk를 통해서 로그로 남긴다.  
					// 이 데이터는 /var/log/message로 출력된다.
					printk("<1> read jiffies = %ld\n", jiffies);
						MOD_DEC_USE_COUNT;

							return len;
}

// 사용자가 /proc/[MODULE_NAME]/foo 파일을 열었을 때 
// 출력해주는 정보 
static int proc_read_foobar(char *page, char **start, off_t off, 
									int count, int *eof, void *data)
{
		int len;
			struct fb_data_t *fb_data = (struct fb_data_t *)data;

				MOD_INC_USE_COUNT;
					// fb_data구조체의 내용을 보시쉽게 만들어서 출력해준다. 
					len = sprintf(page, "%s = %s",
												fb_data->name, fb_data->value);
						MOD_DEC_USE_COUNT;
							return len;
}

// 사용자는 /proc/[MODULE_NAME]/foo에 내용을 쓰기를 원할때도 있을 것이다.
// 이 때 이함수가 호출된다.
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
								// echo "xxxxx" > /proc/[MODULE_NAME]/foo 등으로 입력받은 값을
								// fb_data->value에 저장한다. 
								if (copy_from_user(fb_data->value, buffer, len))
										{
													MOD_DEC_USE_COUNT;
															return -EFAULT;
																}

									fb_data->value[len] = 0x00;
										MOD_DEC_USE_COUNT;

											return len;

}

// 커널 모듈 초기화 함수
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

						// JIFFILE파일의 경우 단지 읽기만 허용한다.
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

											// FOO_FILE의 경우 읽기와 쓰기 모두 가능해도록 해야 하며 
											// 각각의 경우에 호출될 함수를 지정해 줘야 한다. 
											foo_file = create_proc_entry(FOO_FILE, 0644, example_dir);
												if (foo_file == NULL)
														{
																	rv = -ENOMEM;
																			goto no_foo;
																				}

													strcpy(foo_data.name, "foo");
														strcpy(foo_data.value, "dream");

															// 커널 자료구조에 저장될 데이터 
															foo_file->data = &foo_data;
																// 읽기를 요청했을 때 호출될 함수
																foo_file->read_proc = proc_read_foobar;
																	// 쓰기가 요청되었을 때 호출될 함수
																	foo_file->write_proc = proc_write_foobar;
																		foo_file->owner = THIS_MODULE;

																			printk("<1> %s %s initialised\n", "myproc", "1.0");

																				// 제대로 초기화가 이루어졌다면 0을 리턴해야 한다. 
																				return 0;
																					no_foo:
																						remove_proc_entry(FOO_FILE, example_dir);
																							no_jiffies:
																								remove_proc_entry(MODULE_NAME, NULL);
																									out:
																										return rv;
}

// cleanup 함수
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
