#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include <sstream>
#include <math.h>
using namespace std;

/*
	node contains an int vector parent tree, that keeps track of how we got to this node. bound + profit must be unsigned long; int is too small.
*/
struct node {
	int level;
	unsigned long profit;
	float weight;
	unsigned long bound;
	vector<int> parent_tree;
};
/*
Bound function. remember I'm starting from 0, so my indices are different from the textbook.
REMEMBER UNSIGNED LONG.
*/
unsigned long bound(node u, float max_weight, int num_items, int weights[], int profits[]) {
	int j, k;
	float totweight;
	unsigned long result;
	if (u.weight >= max_weight) return 0;
	else {
		result = u.profit;
		j = u.level+1;
		totweight = u.weight;
		//add items until full
		//less than, NOT less and equal to
		while (j < num_items && (totweight + weights[j] <= max_weight)) {
			totweight += weights[j];
			result += profits[j];
			j++;
		}
		//no more full items, so use a fraction now
		k=j;
		if (k< num_items) result += (unsigned long)((max_weight - totweight) * (float)profits[k]/(float)weights[k]);
	}
	return result;
}

class compare_nodes {
    public:
    bool operator()(node& u, node& v) //u < v = higher bounds at top
    {
       if (u.bound < v.bound) return true;
       return false;
    }
};

int best_first_bb(int num_items, int max_weight, int weights[],int profits[], unsigned long &max_profit, int orig_numbers[], string &output_items) {
	priority_queue<node, vector<node>, compare_nodes> pq;
	node v; //parent
	node u; //child
	//initial node
	v.level = 0;
	v.profit = profits[0];
	v.weight = weights[0];
	max_profit = 0;
	v.bound = bound(v,max_weight,num_items,weights,profits);
	if (num_items < 31) v.parent_tree.push_back(v.level);//parent path + current node

	pq.push(v);

	while (!pq.empty()){
		node new_v = pq.top();
		pq.pop();
		//if we think we can get a better profit, look ahead to child node
		if (new_v.bound > max_profit) {
			u.level = new_v.level+1;
			u.weight = new_v.weight+ weights[u.level];
			u.profit = new_v.profit+ profits[u.level];
			//so long as weight < max weight and we have more profit than previously
			if (u.weight <= max_weight && u.profit > max_profit) {
				max_profit = u.profit;
				if (num_items < 31) {
					u.parent_tree = new_v.parent_tree;
					u.parent_tree.push_back(u.level);
					//go through the parent tree, check all nodes, set as best series of items
					stringstream ss;
					for (int i = 0; i < u.parent_tree.size(); i++) {
						ss << orig_numbers[u.parent_tree[i]] << " " << weights[u.parent_tree[i]]<< " " << profits[u.parent_tree[i]] << "\n";
					}
					output_items.assign(ss.str());
				}
			}
			//can u give us a better profit?
			u.bound = bound(u,max_weight,num_items,weights,profits);
			if (u.bound > max_profit) pq.push(u);
			//can skipping over u give us a better profit?
			u.weight = new_v.weight;
			u.profit = new_v.profit;
			if (num_items < 31) u.parent_tree = new_v.parent_tree;
			//bound skipping over child, adding a level to symbolize that it was skipped
			u.bound = bound(u,max_weight,num_items,weights,profits);
			if (u.bound > max_profit) {
				pq.push(u);
			}
//if (pq.size()%1000 == 0)printf("%d %u\n",pq.size(),(int)max_profit);
		}
	}
}

//sort items by greatest profit/weight ratio
bool compare_loot (vector<int> a,vector<int> b) {
//if same, smaller weight first so we can fit more items
	if  ( ((float)a[1]/(float)a[0]) == ((float)b[1]/(float)b[0]) ) {
		return (a[0] < b[0]);
	}
	return ( ( (float)a[1]/(float)a[0] ) > ( (float)b[1]/(float)b[0] ) ); 
}

int main(int argc, char* argv[]) {
	//(first n)	(solution 1 profit)	(solution 1 time)
	FILE * input_ptr = fopen(argv[1],"r");
	if (input_ptr == NULL) {
		cout << "Input file missing!";
		return 1;
	}
	FILE * output_ptr = fopen (argv[2], "w"); //make new output file
	if (input_ptr == NULL) {
		cout << "Output file missing!";
		return 1;
	}
	
	char line_buf[128];
	//the first line will be items/weight
	while(fgets(line_buf,128,input_ptr)){
		vector <vector< int> > item_vector;

		//store total items we have + how much can the bag hold		
		string str = line_buf;
		int space_loc = str.find(" ");		
		int num_items = atoi( str.substr(0,space_loc).c_str() );
		int max_weight = atoi( str.substr(space_loc, strlen(line_buf)).c_str() );

		//have the item array
		for(int i=0; i<num_items; i++){
			//get all the items and store
			fgets(line_buf,128,input_ptr);
			str = line_buf;
			int space_loc = str.find(" ");		
			int weight = atoi( str.substr(0,space_loc).c_str() );
			int profit = atoi( str.substr(space_loc, strlen(line_buf)).c_str() );
			//make a new item, push into item vector
			vector<int> new_item;
			new_item.push_back(weight);
			new_item.push_back(profit);
			new_item.push_back(i+1);
			item_vector.push_back(new_item);
		}
		//all n items are now in the array. sort, copy into arrays! arrays are much faster than using the vector itself.
		sort(item_vector.begin(),item_vector.end(),compare_loot);
		int profits[num_items];
		int weights[num_items];
		int orig_numbers[num_items];
		for (int i = 0; i < num_items; i++){
			weights[i] = item_vector[i][0];
			profits[i] = item_vector[i][1];
			orig_numbers[i] = item_vector[i][2];
		}

		unsigned long max_profit = 0;
		string output_items(""); //keep track of how we got the max profit

		time_t start = time(0);
		best_first_bb(num_items, max_weight, weights, profits, max_profit,orig_numbers,output_items);
		time_t end = time(0);
		int span = end-start;
		fprintf(output_ptr, "%d %u %d\n", num_items, max_profit,span); //%u is for unsigned long.
		if (num_items < 31) fprintf(output_ptr,"%s",output_items.c_str());
	} //next problem
	fclose(output_ptr);
	fclose(input_ptr);
	return 0;
}

