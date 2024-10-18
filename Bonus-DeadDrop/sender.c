
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>
#include <unistd.h>

// [Bonus] TODO: define your own buffer size
#define BUFF_SIZE (1<<21)
//#define BUFF_SIZE 1024

int main(int argc, char **argv)
{
    // Allocate a buffer using huge page
    // See the handout for details about hugepage management
    void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE |
                    MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    printf("The address of buffer is %p\n", buf);
    
    if (buf == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }
    // The first access to a page triggers overhead associated with
    // page allocation, TLB insertion, etc.
    // Thus, we use a dummy write here to trigger page allocation
    // so later access will not suffer from such overhead.
    *((char *)buf) = 1; // dummy write to trigger page allocation

    volatile char tmp = 0;
    char *access_ptr;

    volatile bool sending = true;
    // [Bonus] TODO:
    // Put your covert channel setup code here

    printf("Please type a message.\n");
    char text_buf[128];
    fgets(text_buf, sizeof(text_buf), stdin);
    int set_no = string_to_int(text_buf);
    access_ptr = (char *) buf + (set_no << 6);
    while (sending) {
    __asm__ (
    "mov (%1), %0\n\t"
    "mov 0x10000(%1), %0\n\t"
    "mov 0x20000(%1), %0\n\t"
    "mov 0x30000(%1), %0"
    : "=r" (tmp)
    : "r" (access_ptr)
    : "memory"
	);
    lfence();
    }

    printf("The accessed addresses are %p\n", access_ptr);
    printf("Sender finished.\n");
    return 0;
}


