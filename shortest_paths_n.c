/*
Problem Introduction
Now, you would like to compute an optimal way of exchanging the given currency 
c i into all other currencies.
For this, you find shortest paths from the vertex c i to all the other vertices.

Task. 
Given a directed graph with possibly negative edge weights and with n vertices 
and m weighted edges as well as its vertex s, compute the length of shortest 
paths from s to all other vertices of the graph.
Input Format.
A graph is given in the standard format. The next line contains the source 
vertex s. 
Constraints. 1 ≤ n ≤ 10^3 , 0 ≤ m ≤ 10^4, 1 ≤ s ≤ n, edge weights are at most
abs(10^9)
Output Format. 
For all vertices i from 1 to n output the following on a separate line:
• “*”, if there is no path from s to u;
• “-”, if there is a path from s to u, but there is no shortest path from s to 
       u (that is, the distance from s to u is −∞);
• the length of a shortest path otherwise.

Time Limits. C: 2 sec, C++: 2 sec, Java: 3 sec, Python: 10 sec.
             C#: 3 sec, Haskell: 4 sec, JavaScript: 10 sec, Ruby: 10 sec, 
             Scala: 6 sec.
Memory Limit. 512Mb.
Results:
Good job! (Max time used: 0.03/2.00, max memory used: 12189696/536870912.)
 * */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
struct node {
        struct node *next;//queue links
        struct node *save;//saved q links if node is 'deleted' in sbfs 
        struct adj *adj;  //neighbor list
        int id;
        long long cost; //cost path from src so far when running Dijkstra
	int cnt; //if it ever == count of nodes there is a negative cycle
};


struct adj {
        struct adj *next;
        int v; //id of adjacent node
	long long w; //weight/cost of the edge

};

/* global definitions for convenience */
struct node *arrn;  //array of node structures, 1-relative
struct node *head, *tail; //the q
struct adj *arradj; //preallocated to save malloc time in addedge
int cadj;           //next struct adj to allocate
int nodes;          //count of nodes
int edges;          //count of edges

/*nbfs and sbfs could be collapsed, but there would be too much special casing*/
/* simple bfs on a node that is part of a negative cycle. Anything it sees 
 * gets marked with a cost of LLONG_MIN
 */
void sbfs(int src) {
        struct node *ntmp, *head, *tail; //needs a separate q 
        struct adj *atmp, *awrk;

        head = arrn + src; //head of queue
        awrk = head->adj; //used to walk adj lists without destroying them
        tail = head;     //empty queue
        head->cost = LLONG_MIN; //mark a node in a negative cycle
        while(head) {
                if (!(atmp = awrk)) { //all neighbors have been checked
                        ntmp = head;       //save to clear its 'next'
                        head = head->next; //next in queue
                        if (head) {
                                awrk = head->adj;  //its neighbor list
                                ntmp->next = 0;    // make it unqueued
                        }
                        continue;
                }
                ntmp = arrn + atmp->v;
                awrk = atmp->next;
		if (ntmp->cost == LLONG_MIN) { //neighbor already marked
			continue;
		}
		//if not marked, it is not in the queue
		tail->save = tail->next; //preserve the original queue
                tail->next = ntmp;
                tail = ntmp;
		ntmp->cost = LLONG_MIN;
	}
	return;
}


/* remove current head from queue and return the first neighbor of the new
 * head if the queue hasn't gone empty
 */
struct adj * mvhead() {
	struct node *ntmp;
	struct adj *atmp;

	do {
                ntmp = head;       //save to clear its 'next'
		if (head->save) {
			head->next = head->save; //restore linkage after sbfs
			head->save = 0;
		}
		head = head->next; //next in queue
		ntmp->next = 0;    // make it unqueued
	} while (head && (head->cost == LLONG_MIN));

	return head ? head->adj : 0;
}

		

/* 
 * do a breadth-first seach using a queue and starting at src. 
 * Run Dyjkstra since the edges are weighted but add extensions for handling
 * possible negative cycles. Even if a node has been visited, it must be 
 * checked when visiting every neighbor in case the distance is shortened. Also
 * a node might have to be re-queued if some other path visits it and it isn't 
 * still in the queue. If any node is visited > nodes times,  quit. All paths 
 * should have been checked, and something in the negative cycle should have a
 * cnt >= nodes. Then run a simple bfs against the id of the first node to be 
 * visited > nodes times just to mark all nodes that are part of the cycle.  
 */
void nbfs(int src) {
        struct node *ntmp;
        struct adj *atmp, *awrk;

        head = arrn + src; //head of queue
	head->cost = 0; //also marks it as seen
        if (!head->adj)
                return;
	awrk = head->adj; //used to walk adj lists without destroyinh them
        tail = head;
        while(head) {
                /* enqueue each neighbor of head if not already processed,
                 * updating cost and prev. */
                if (!(atmp = awrk)) { //all neighbors have been checked
			awrk = mvhead(); 
                        continue;
                }
                ntmp = arrn + atmp->v;
                awrk = atmp->next;
		if (ntmp->cost == LLONG_MIN)
			continue; //removed as part of a negative cycle
		//whether ntmp has or hasn't been seen, see if this edge can 
		//improve it. If it has never been 'seen' it will be improved.
		//if it isn't already on the queue, put it there so its 
		//neighbors can also be improved
		if ((head->cost + atmp->w) < ntmp->cost) {
                        if (ntmp->cnt++ == nodes) {
				
				sbfs(ntmp->id); //neg cycle, id/rem nodes
                                continue; //negative cycle, all members id'd
                        }
		        ntmp->cost = head->cost + atmp->w;
			if (!ntmp->next && (tail != ntmp)) {
				tail->next = ntmp;
				tail = ntmp;
			}
		}
        }
	return; //finished bfs, no negative cycles
}

/*add endpoint e2 to e1's adjacency list with weight w  */
void addedge(int e1, int e2, int w) {
        struct node *tmp1;
        struct adj *atmp;

        tmp1 = arrn + e1;
        atmp = arradj + cadj++; //get a struct adj
        atmp->v = e2;
	atmp->w = w;
        if (!tmp1->adj)
                atmp->next = 0;
        else
                atmp->next = tmp1->adj;
        tmp1->adj = atmp;
}



/* display nodes and their neighbors */
void show () {
        int i;
        struct node *tmp;
        struct adj *atmp;

        for (i = 1; i <= nodes; ++i) {
                fprintf(stderr, "node: %d  neighbors: ", arrn[i].id);
                atmp = arrn[i].adj;
                while (atmp) {
                        fprintf(stderr, "%d ", atmp->v);
                        atmp = atmp->next;
                }
                fprintf(stderr, "\n");
        }
}

/*build graph from caller input and then, given source and target node 
 *numbers; prints minimum hops (cost in edges) if there is a path, else -1. 
 */
int main () {
        int i, src, targ, e1, e2;
	long long w;

        scanf("%d%d", &nodes, &edges);
        if (!edges) {
                scanf("%d",&src);
		for (i = 1; i <= nodes; ++i) {
			if (i == src)
				printf("0\n");
			else
				printf("*\n");
		}
                return 0;
        }
        arrn = calloc(nodes + 1, sizeof(struct node));
        arradj = malloc(edges * sizeof(struct adj));
        if (!arrn || !arradj)
                exit(100);
        for (i = 1; i <= nodes; ++i) {
                arrn[i].id = i;
                arrn[i].cost = LLONG_MAX;
        }
        for (i = 0; i < edges; ++i) {
                scanf("%d%d%lld", &e1, &e2, &w);
                addedge(e1, e2, w);
        }
        scanf("%d",&src);
        //show();
	if (nodes == 1) {
		printf("0\n");
		return 0;
	}
	nbfs(src);
	for (i = 1; i <= nodes; ++i) {
		if (arrn[i].cost == LLONG_MIN) 
			printf("-\n");
		else if (arrn[i].cost == LLONG_MAX) 
			printf("*\n");
		else printf("%lld\n",arrn[i].cost);
		/*if (arrn[i].cost == LLONG_MIN) 
			printf("- %d\n",arrn[i].cnt);
		else if (arrn[i].cost == LLONG_MAX) 
			printf("* %d\n", arrn[i].cnt);
		else printf("%lld %d\n", arrn[i].cost, arrn[i].cnt);*/
	}
        return 0;
}




/*
Sample 1.
Input:
6 7
1 2 10
2 3 5
1 3 100
3 5 7
5 4 10
4 3 -18
6 1 -1
1
Output:
0
10
-
-
-
*

Sample 2.
Input:
5 4
1 2 1
4 1 2
2 3 2
3 1 -5
4
Output:
-
-
-
0
*

some extra test cases
4 5 
1 2 1 
2 3 1 
3 1 -3 
3 4 1 
4 3 1 
4
:w

10 11
6 1 -1
1 2 10
1 3 100
2 3 5
3 5 7
3 7 2
5 4 10
5 8 3
8 10 2
4 3 -18
4 9 5
1

output:
0
10
-
-
-
*
-
-
-
-

two negative cycles:

9 10
4 5 1
4 1 2
1 2 1
2 3 2
3 1 -5
4 6 2
6 7 1
7 8 2
8 6 -5
8 9 1
4
==
-
-
-
0
1
-
-
-
-



*/
