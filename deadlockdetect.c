#include "kernel/types.h"
#include "user/user.h"

#define MAX_P 5
#define MAX_R 4

int wait_for[MAX_P][MAX_P];
int alloc[MAX_P][MAX_R];
int request[MAX_P][MAX_R];
int num_proc;
int visited[MAX_P], rec_stack[MAX_P];
int cycle_found;

void init() {
    for (int i = 0; i < MAX_P; i++) {
        for (int j = 0; j < MAX_P; j++) wait_for[i][j] = 0;
        for (int j = 0; j < MAX_R; j++) { alloc[i][j] = 0; request[i][j] = 0; }
        visited[i] = 0; rec_stack[i] = 0;
    }
    cycle_found = 0;
}

void build_graph() {
    printf("\n=== Building Wait-For Graph ===\n");
    for (int i = 0; i < num_proc; i++) {
        for (int j = 0; j < num_proc; j++) {
            if (i != j) {
                for (int r = 0; r < MAX_R; r++) {
                    if (request[i][r] > 0 && alloc[j][r] > 0) {
                        wait_for[i][j] = 1;
                        printf("Edge: P%d -> P%d (P%d waiting for R%d held by P%d)\n", i, j, i, r, j);
                        break;
                    }
                }
            }
        }
    }
}

int detect_cycle(int node, int path[], int len) {
    visited[node] = 1;
    rec_stack[node] = 1;
    path[len] = node;
    
    for (int i = 0; i < num_proc; i++) {
        if (wait_for[node][i]) {
            if (!visited[i]) {
                if (detect_cycle(i, path, len + 1)) return 1;
            } else if (rec_stack[i]) {
                cycle_found = 1;
                printf("\n*** DEADLOCK DETECTED! ***\nCycle: ");
                for (int j = 0; j <= len; j++) printf("P%d -> ", path[j]);
                printf("P%d\n", i);
                return 1;
            }
        }
    }
    rec_stack[node] = 0;
    return 0;
}

int detect_deadlock() {
    printf("\n=== Running Cycle Detection ===\n");
    for (int i = 0; i < num_proc; i++) { visited[i] = 0; rec_stack[i] = 0; }
    
    int path[MAX_P];
    for (int i = 0; i < num_proc; i++)
        if (!visited[i])
            if (detect_cycle(i, path, 0)) return 1;
    
    if (!cycle_found) printf("No deadlock - graph is acyclic.\n");
    return cycle_found;
}

void scenario_no_deadlock() {
    printf("\n\n=== SCENARIO 1: No Deadlock ===\n");
    num_proc = 4; init();
    
    alloc[0][0] = 1; request[0][1] = 1;
    alloc[1][1] = 1; request[1][2] = 1;
    alloc[2][2] = 1;
    alloc[3][3] = 1;
    
    build_graph();
    detect_deadlock();
}

void scenario_deadlock() {
    printf("\n\n=== SCENARIO 2: Deadlock (Circular Wait) ===\n");
    num_proc = 4; init();
    
    alloc[0][0] = 1; request[0][1] = 1;
    alloc[1][1] = 1; request[1][2] = 1;
    alloc[2][2] = 1; request[2][0] = 1;
    alloc[3][3] = 1;
    
    build_graph();
    detect_deadlock();
}

int main() {
    printf("========================================\n");
    printf("Deadlock Detection using RAG\n");
    printf("========================================\n");
    
    scenario_no_deadlock();
    scenario_deadlock();
    
    printf("\n========================================\n");
    exit(0);
}
