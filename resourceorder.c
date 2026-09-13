#include "kernel/types.h"
#include "user/user.h"

#define DELAY 50000

struct locks {
    volatile int lock1;
    volatile int lock2;
    int mode;
};

void acquire(volatile int *lock, char *name, int pid) {
    printf("P%d: Acquiring %s...\n", pid, name);
    while (*lock == 1) for (volatile int j = 0; j < 1000; j++);
    *lock = 1;
    printf("P%d: Got %s\n", pid, name);
}

void release(volatile int *lock, char *name, int pid) {
    *lock = 0;
    printf("P%d: Released %s\n", pid, name);
}

void work(int pid) {
    printf("P%d: Working...\n", pid);
    for (volatile int j = 0; j < DELAY; j++);
}

void proc_a(struct locks *l) {
    if (l->mode == 0) {
        acquire(&l->lock1, "Lock1", 'A');
        for (volatile int j = 0; j < DELAY; j++);
        acquire(&l->lock2, "Lock2", 'A');
        work('A');
        release(&l->lock2, "Lock2", 'A');
        release(&l->lock1, "Lock1", 'A');
    } else {
        acquire(&l->lock1, "Lock1", 'A');
        acquire(&l->lock2, "Lock2", 'A');
        work('A');
        release(&l->lock2, "Lock2", 'A');
        release(&l->lock1, "Lock1", 'A');
    }
    printf("P%d: Done\n", 'A');
    exit(0);
}

void proc_b(struct locks *l) {
    if (l->mode == 0) {
        acquire(&l->lock2, "Lock2", 'B');
        for (volatile int j = 0; j < DELAY; j++);
        acquire(&l->lock1, "Lock1", 'B');
        work('B');
        release(&l->lock1, "Lock1", 'B');
        release(&l->lock2, "Lock2", 'B');
    } else {
        acquire(&l->lock1, "Lock1", 'B');
        acquire(&l->lock2, "Lock2", 'B');
        work('B');
        release(&l->lock2, "Lock2", 'B');
        release(&l->lock1, "Lock1", 'B');
    }
    printf("P%d: Done\n", 'B');
    exit(0);
}

int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("Resource Ordering Demo\n");
    printf("========================================\n");
    
    int mode = (argc >= 2) ? atoi(argv[1]) : 1;
    struct locks *l = (struct locks *)shm_get();
    if ((uint64)l == 0) { printf("shm_get failed\n"); exit(1); }
    
    l->lock1 = 0; l->lock2 = 0; l->mode = mode;
    
    if (mode == 0) {
        printf("*** BAD MODE: Different lock orders (may hang) ***\n");
    } else {
        printf("*** FIXED MODE: Same lock order (safe) ***\n");
    }
    
    int p1 = fork();
    if (p1 == 0) { l = (struct locks *)shm_get(); proc_a(l); }
    
    for (volatile int j = 0; j < 10000; j++);
    
    int p2 = fork();
    if (p2 == 0) { l = (struct locks *)shm_get(); proc_b(l); }
    
    wait(0); wait(0);
    
    printf("\nBoth processes completed!\n");
    exit(0);
}
