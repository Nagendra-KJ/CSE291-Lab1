
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>
#include <unistd.h>
#define L1_SIZE 32 * 1024
#define L2_SIZE 256 * 1024
#define BUFF_SIZE 2 * L2_SIZE

void prime(char *l2_eviction_buffer)
{
	char tmp = 0;
	for (volatile int i = 0; i < L2_SIZE; i++)
	{
		tmp = l2_eviction_buffer[i];
	}
	for (volatile int i = 0; i < L1_SIZE; ++i)
	{
		tmp = l2_eviction_buffer[L2_SIZE + i];
	}
}

int main(int argc, char **argv)
{
    // [Bonus] TODO: Put your covert channel setup code here
    //
    char *buf = (void *) malloc(BUFF_SIZE * sizeof(char));
    char *l2_eviction_buffer = ((uintptr_t) buf + 65536) & ~0xffff;
    printf("The base pointer for probing is %p\n", l2_eviction_buffer);
    volatile int found[256] = {0};
    volatile int time;

    volatile bool listening = true;
    printf("Please press enter.\n");
    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);
    usleep(1000 * 1000 * 3);
    printf("Receiver now listening.\n");

    while (listening) {

        // [Bonus] TODO: Put your covert channel code here
	for (volatile int i = 0; i < 1; ++i) {
    		prime((char *)l2_eviction_buffer);
		lfence();
	}
	usleep(10);
	for (volatile int i = 0; i < 256; ++i)
	{
	        //char *access_ptr = buf + (i << 6);
		time = measure_one_block_access_time(l2_eviction_buffer + (i << 6));
		if (time > 40 && time < 100) {
			found[i]++;
			if (found[i] > 100) {
				printf("Received message %d\n", i);
				listening = false;
				break;
			}
		}
	}
    }

    for (int i = 0; i < 256; ++i)
	    printf("%d - %d\n", i, found[i]);

    printf("Receiver finished.\n");
    return 0;
}


