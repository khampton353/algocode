/* Count the strongly connected components of a graph, where nodes
 * in a component are mutually reachable, but not reachable after
 * taking a path to another component.
 *
 * Idea is, dfs a reverse graph for each node (that doesn't get
 * included in an earlier dfs along the way). Then dfs the main
 * graph using rdfs results to isolate and count components.
 * nodes: 1 - 10^4 edges: 0 - 10^4
 * Grader result:
 * Max time used: 0.01/1.00, max memory used: 11558912/536870912
 */
#include <stdio.h>
#include<stdlib.h>

/* node structure. NOTE that pre c and post c have nothing to do with the 
 * node id in arrg. They are put there by rdfs to track the post counts and
 * their nodes in sorted order for the dfs calls after rdfs has been run
 */
struct node {
	struct node *next; // in DFS, form a stack of examined nodes
	struct adj *adj; // adjacency list
	int id;
	int cid;  //indicates 'seen'. also component count/id for main graph
	int prec; //pre count = reverse graph, post count node in arrg
	int postc; //post count = reverse graph, post count node in arrg
	struct adj *wrk; //used in reverse graph so 'adj' isn't destroyed
};

/* edges for a node */
struct adj {
	struct adj *next;
	struct node *v;
};



struct node *arrg; //graph
struct node *arrr; //reverse graph
int cid; //component id for the graph; at the end, the count of components
int rcid; /*reverse cid. when redoing a reverse graph, if the cid of a node is 
not equal to the current cid, treat it as unseen*/
int rcnt; //counter for reverse graph dfs
int pidx; /* in rdfs, use the main graph prec/postc at this index to track 
	     the post counts and their nodes as they are written*/
int nodes;
int cnodes; //when reducing the graph/arrg, counter of remaining nodes


//   at this point you have the highest node in the reverse graph. use it to 
//   dfs in the main graph. bump cid and set it into everything the high 
//   node reaches. if a node is found for the first time --cnodes
//   ALSO MAKE IT UNREACHABLE IN THE REVERSE GRAPH, MAYBE SET CID TO -1 /////// 
//   in that dfs if a node was seen, skip it. when the initial node 
//   is to be popped off the stack, pop it off and go back to while cnodes 
//
void dfs (int src) {
	struct node *stk = 0, *v;
	struct adj *atmp;


	stk = arrg + src; 
	if (stk->cid > 0)
		return; //already processed
	stk->next = stk;
	stk->cid = ++cid; //expects caller to ensure this node is present
	(arrr + src)->cid = -1; //remove it from the reverse graph
	--cnodes;
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
		if (v->cid > 0) { //this neighbor has been visited
		       continue;
		}
		v->cid = cid; //mark visited 
		v->next = stk;     //push v on the stk
		stk = v;
		(arrr + v->id)->cid = -1;
		--cnodes;

	}
	return;
}


/* given an edge, update the adjacency list for the source with the target
 * caller insures that e1 and e2 are in bounds for arr.
 */
void addedge(struct node *arr, int e1, int e2) {
	struct node *tmp1, *tmp2;
	struct adj *atmp;

	tmp1 = arr + e1;
	tmp2 = arr + e2;
	atmp = malloc(sizeof(struct adj)); //can speed up things if from a pool
	if (!atmp) exit(201);
	atmp->v = tmp2;
	if (!tmp1->adj) 
		atmp->next = 0;
	else
		atmp->next = tmp1->adj;
	tmp1->adj = atmp;
}


//go do a rdfs that looks like the following
//get a node loop: for i = 1; i<=nodes; ++i
//get a node. IF IT HAS A CID OF -1 SKIP IT AT EVERY LEVEL 
//walk through all the nodes in the reverse graph setting their 
//cid to rcid and incrementing rcnt for their pre and post. if you find a node 
//with a cid that matches rcid ignore it. if the initial node is to be popped 
//from the stack, pop it and go to get a node
//

void rdfs (int src) {
	struct node *stk = 0, *v;
	struct adj *atmp;


	stk = arrr + src;
	stk->next = stk;
	stk->wrk = stk->adj; //need to preserve adjacency list 
	stk->cid = rcid; //expects caller to check that cid != -1
	stk->prec = ++rcnt;
	while (stk) {
		if (!(atmp = stk->wrk)) {
			v = stk;
			v->postc = ++rcnt;
			++pidx;
			arrg[pidx].postc = rcnt; //borrow unused arrg fields
			arrg[pidx].prec = v->id; //for postc nodes min order
			if (stk->next == stk) {
				stk->next = 0;
				break;
			}
			stk = stk->next;
			v->next = 0;
			continue;
		}
		stk->wrk = atmp->next; //might be null
		v = atmp->v;         //the neighbor
		if (v->cid < 0)
			continue; //already 'removed' from the graph
		if (v->cid == rcid)  //has been visited during this (r)dfs
			       continue;
		v->cid = rcid; //mark visited this rdfs pass
		v->wrk = v->adj;
                v->prec = ++rcnt;		
		v->next = stk;     //push v on the stk
		stk = v;
	}
	return;
}



//docid
//set cnodes to nodes (or maybe just reduce nodes 
//rcid is  0
//while cnodes go do reverse dfs ( dfs on the reverse graph)
//bump rcid
//set rcnt to 0

//   at this point you have the highest node in the reverse graph. use it to 
//   dfs the main graph. bump cid and set it into everything the high 
//   node reaches. if a node is found for the first time --cnodes
//   ALSO MAKE IT UNREACHABLE IN THE REVERSE GRAPH, MAYBE SET CID TO -1 /////// 
//   in that dfs if a node was seen, skip it. when the initial node 
//   is to be popped off the stack, pop it off and go back to while cnodes 
//
//once cnodes is 0, return to main
//
void docid() {
	int i, tmp;
	
        cnodes = nodes;
	++rcid; //identifies this pass of the reverse dfs
	rcnt = 0; //global, so every rdfs pass builds on it anew
	for (i = 1; i <= nodes; ++i) {
		if ((arrr[i].cid == -1) || (arrr[i].cid == rcid)) 
			continue; //removed or processed
		rdfs(i);
	}
	tmp = arrg[pidx].prec;
	while (cnodes) { //decremented in dfs. assert cnodes while pidx
		while (pidx && arrr[tmp].cid == -1)
			tmp = arrg[--pidx].prec;
		dfs(tmp); //node with largest postc
		
	}
	return;
}


void show (struct node *arr, int nodes) {
	int i;
	struct node *tmp;
	struct adj *atmp;

	for (i = 1; i <= nodes; ++i) {
		fprintf(stderr, "node: %d  neighbors: ", arr[i].id);
		atmp = arr[i].adj;
		while (atmp) {
			fprintf(stderr, "%d ", atmp->v->id);
			atmp = atmp->next;
		}
		fprintf(stderr, "\n");
	}
}

/*build graph from caller input and then, given source and target node 
 *numbers prints 1 if there is a path, else 0. 
 */
int main () {
	int i, edges, e1, e2;

	scanf("%d%d", &nodes, &edges);
	if (!edges) {
		printf("%d\n", nodes);
		exit(0);
	}

	/* TODO move nodes==1 block HERE!! no need to do allocations etc */

	arrg = calloc(nodes + 1, sizeof(struct node));
	arrr= calloc(nodes + 1, sizeof(struct node));

	/* TODO allocate an array of 2 * edges adjacency structures HERE!!
	 * individual mallocs is a waste of time. split in half, assign
	 * to arrg nodes from the beginning, arrr from the middle.
	 */

	if (!arrg || !arrr) exit(100);

	for (i = 1; i <= nodes; ++i) {
		arrr[i].id = i;
		arrg[i].id = i;
	}
	if (nodes == 1) {
		for (i = 0; i < edges; ++i) {
			scanf("%d%d", &e1, &e2);
		}
		printf("1\n");
		exit(0);
	}
	for (i = 0; i < edges; ++i) {
		scanf("%d%d", &e1, &e2);
		if ((e1 > nodes) || (e2 > nodes))
			exit(200);
		if (e1 != e2) {
		    addedge(arrg, e1, e2);
			addedge(arrr, e2, e1);
		}
	}
	//show(arrg, nodes);
	//show(arrr, nodes);
	docid();
	printf("%d\n", cid);
	return 0;
}

