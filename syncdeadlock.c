#include "kernel/types.h"
#include "user/user.h"

#define NUM_PROC 5
#define NUM_CYCLES 3
#define NUM_RES 3

int proc_needs[NUM_PROC][2] = {{0,1}, {1,2}, {0,2}, {0,1}, {1,2}};

struct state {
    volatile int avail[NUM_RES];
    volatile int alloc[NUM_PROC][NUM_RES];
    volatile int locks[NUM_RES];
};

void delay(int n) { for (volatile int j = 0; j < n; j++); }

int acquire(struct state *s, int res, int pid) {
    char *names[] = {"Printer", "Scanner", "Disk"};
    printf("P%d: Requesting %s...\n", pid, names[res]);
    
    while (s->avail[res] <= 0) delay(1000);
    while (s->locks[res] == 1) delay(1000);
    s->locks[res] = 1;
    
    if (s->avail[res] > 0) {
        s->avail[res]--;
        s->alloc[pid][res] = 1;
        s->locks[res] = 0;
        printf("P%d: Got %s (avail: %d)\n", pid, names[res], s->avail[res]);
        return 1;
    }
    s->locks[res] = 0;
    return 0;
}

void release(struct state *s, int res, int pid) {
    char *names[] = {"Printer", "Scanner", "Disk"};
    while (s->locks[res] == 1) delay(1000);
    s->locks[res] = 1;
    s->avail[res]++;
    s->alloc[pid][res] = 0;
    printf("P%d: Released %s (avail: %d)\n", pid, names[res], s->avail[res]);
    s->locks[res] = 0;
}

void philosopher(int pid, struct state *s) {
    int r1 = proc_needs[pid][0];
    int r2 = proc_needs[pid][1];
    int first = (r1 < r2) ? r1 : r2;
    int second = (r1 < r2) ? r2 : r1;
    
    for (int cyc = 0; cyc < NUM_CYCLES; cyc++) {
        printf("\n--- P%d: Cycle %d ---\n", pid, cyc+1);
        printf("P%d: THINKING\n", pid);
        delay(20000);
        
        printf("P%d: HUNGRY\n", pid);
        acquire(s, first, pid);
        delay(10000);
        acquire(s, second, pid);
        
        printf("P%d: EATING\n", pid);
        delay(30000);
        
        release(s, second, pid);
        release(s, first, pid);
        printf("P%d: Finished cycle %d\n", pid, cyc+1);
    }
    printf("P%d: COMPLETED\n", pid);
    exit(0);
}

int main() {
    printf("========================================\n");
    printf("Combined Sync & Deadlock Avoidance\n");
    printf("========================================\n");
    printf("Resources: Printer(2), Scanner(1), Disk(2)\n");
    
    struct state *s = (struct state *)shm_get();
    if ((uint64)s == 0) { printf("shm_get failed\n"); exit(1); }
    
    s->avail[0] = 2; s->avail[1] = 1; s->avail[2] = 2;
    for (int i = 0; i < NUM_PROC; i++)
        for (int j = 0; j < NUM_RES; j++)
            s->alloc[i][j] = 0;
    for (int i = 0; i < NUM_RES; i++) s->locks[i] = 0;
    
    for (int i = 0; i < NUM_PROC; i++) {
        int p = fork();
        if (p == 0) { s = (struct state *)shm_get(); philosopher(i, s); }
        delay(5000);
    }
    
    for (int i = 0; i < NUM_PROC; i++) wait(0);
    
    printf("\n========================================\n");
    printf("All processes completed!\n");
    printf("========================================\n");
    exit(0);
}
