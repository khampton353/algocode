/* check whether a given graph has a cycle.
 * Input:
 * first line, #nodes n followed by #edges e
 * next e lines: source follwed by dest node #s of the edge
 * output: 1 if a cycle is found else 0
 * grader output: Max time used: 0.00/1.00, max memory used: 7938048/536870912.
 */

#include <stdio.h>
#include<stdlib.h>

struct node {
	struct node *next;
	struct adj *adj;
	int id;
	int from;
};


struct adj {
	struct adj *next;
	struct node *v;
};


struct node *arrn;


/* starting with the node at src, explore using dfs to assign a prev to every 
 * node reachable from src. use a node's next pointer to keep a list-based stack
 * while its adjacency list is explored. a visited node found on the stack means
 * there is a cycle so return 1 else 0; note this destroys the adjacency lists. 
 */
int label (int src) {
	struct node *stk = 0, *v, *prev;
	struct adj *atmp;


	stk = arrn + src; 
	if (stk->from >= 0)
		return 0; //assume this is part of some DAG
	stk->next = stk;
	stk->from = 0; //0 will be updated if src gets found
	while (stk) {
		if (!(atmp = stk->adj)) {
			v = stk;
			if (stk->next == stk) {
				stk->next = 0;
				break;
			}
			stk = stk->next;
			v->next = 0;
			continue;
		}
		stk->adj = atmp->next; //might be null
		v = atmp->v;
		if (v->from >= 0) { //this neighbor has been visited
		       if (v->next) //it is on the stack
			       return 1;
		       else 
			       continue;
		}
		v->from = stk->id; //mark visited 
		v->next = stk;     //push v on the stk
		stk = v;
	}
	return 0;
}


/* given an edge, update the adjacency list for the source with the target
 * caller insures that e1 and e2 are in bounds for arr
 */
void addedge(int e1, int e2) {
	struct node *tmp1, *tmp2, *arr = arrn;
	struct adj *atmp;
	int i;

	tmp1 = arrn + e1;
	tmp2 = arrn + e2;
	atmp = malloc(sizeof(struct adj)); // better if allocated from a pool!
	if (!atmp) exit(201);
	atmp->v = tmp2;
	if (!tmp1->adj) 
		atmp->next = 0;
	else
		atmp->next = tmp1->adj;
	tmp1->adj = atmp;
}

void show (int nodes) {
	int i;
	struct node *tmp;
	struct adj *atmp;

	for (i = 1; i <= nodes; ++i) {
		fprintf(stderr, "node: %d  neighbors: ", arrn[i].id);
		atmp = arrn[i].adj;
		while (atmp) {
			fprintf(stderr, "%d ", atmp->v->id);
			atmp = atmp->next;
		}
		fprintf(stderr, "\n");
	}
}

/*build graph from caller input and then, determine whether the
 *graph contains a cycle. prints 1 if there is a cycle, else 0. 
 */
int main () {
	int i, nodes, edges, e1, e2;

	scanf("%d%d", &nodes, &edges);
	arrn = calloc(nodes + 1, sizeof(struct node));
	if (!arrn) exit(100);
	for (i = 1; i <= nodes; ++i) {
		arrn[i].id = i;
		arrn[i].from = -1;
	}
	for (i = 0; i < edges; ++i) {
		scanf("%d%d", &e1, &e2);
		if ((e1 > nodes) || (e2 > nodes))
			exit(200);
		if (e1 == e2) {
            printf("1\n");
            exit(0);
        }
		addedge(e1, e2);
	}
	//show(nodes);
    for (i = 1; i <= nodes; ++i) {
        if (label(i)) {
			printf("1\n");
			exit(0);
		}
    }

	printf("0\n");
	return 0;
}

