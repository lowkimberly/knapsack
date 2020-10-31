
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <ctime>
using namespace std;

/*
	First solution: Given the array of items, we go through all 2^n sets and add
	up the weight/profit. if it fits in the knapsack, and the profit is greater
	than our previous solution, pick that one.
*/
int knapsack1(int num_items, int max_weight, int item_array[][2]){
	int max_profit = 0;
	//number of sets represented in binary
	int binary_counter[num_items]; //indexes represent items, same order as item_array, values either 1 or 0
	for (int init_bc = 0; init_bc < num_items; init_bc++) binary_counter[init_bc] = 0; //start it at 0
		
	for (int set_number = 0; set_number < pow(2.0,num_items)-1 ; set_number++) {
		//do this for all 2^n sets
		int bin_index=0;
		//move up the array/counter and increase by 1
		//binary_counter stays the same every iteration
		while (binary_counter[bin_index]==1) {
			binary_counter[bin_index]=0;
			bin_index=bin_index+1;
		}
		//if we haven't reached the end, we add 1 and have a new set
		if(bin_index<num_items) {
			binary_counter[bin_index]=1;
			//weight/profit of this set
			int local_weight = 0;
			int local_profit = 0;
			for(int wp_counter = 0; wp_counter < num_items; wp_counter++){ //add everything with a value of 1
				if (binary_counter[wp_counter] == 1) {
					local_weight += item_array[wp_counter][0];
					local_profit += item_array[wp_counter][1];
				}
			}
			if ( (local_weight <= max_weight) && (local_profit > max_profit) ) 	max_profit = local_profit;
		} // else we've reached the very end of the counter, don't do anything		
	}//went through all the subsets
	return max_profit;
}

/*
	second solution:
*/
int knapsack2(int num_items, int max_weight, int item_array[][2], int &max_p, int i, int sum_w, int sum_p){
	if (sum_p > max_p) max_p = sum_p; //max_p is the profit
	if (i >= num_items) return max_p; //i starting at 0
	//recursion
	if (sum_w + item_array[i][0] <= max_weight ) {
		//if the current weight + new item less than the max
		//add weight and profit, then do next item
		knapsack2(num_items, max_weight, item_array, max_p, i+1, sum_w+item_array[i][0], sum_p+item_array[i][1]);
	}
	//the if statement didn't work, or we went over weight, or if did work because we want to check ALL branches
	knapsack2(num_items, max_weight, item_array, max_p, i+1, sum_w, sum_p);
	
}
/*
	main: Read input file, turn items for 1 problem into an array, then run the
	solutions and compare.
*/
int main(int argc, char* argv[]) {
//(first n)	(solution 1 profit)	(solution 2 profit)	(solution 1 time)	(solution 2 time)
	FILE * input_ptr = fopen("input.txt","r");
	if (input_ptr == NULL) {
		cout << "File missing!";
		return 1;
	}

	FILE * output_ptr = fopen ("output.txt", "w"); //make new output file

	char line_buf[256];
	//the first line will be items/weight
	while(fgets(line_buf,256,input_ptr)){
		//store total items we have + how much can the bag hold		
		string str = line_buf;
		int space_loc = str.find(" ");		
		int num_items = atoi( str.substr(0,space_loc).c_str() );
		int max_weight = atoi( str.substr(space_loc, strlen(line_buf)).c_str() );

		//have the item array
		int item_array[num_items][2];
		for(int i=0; i<num_items; i++){
			//get all the items and store
			fgets(line_buf,256,input_ptr);
			str = line_buf;
			int space_loc = str.find(" ");		
			int weight = atoi( str.substr(0,space_loc).c_str() );
			int profit = atoi( str.substr(space_loc, strlen(line_buf)).c_str() );

			item_array[i][0] = weight;
			item_array[i][1] = profit;
		}
		//all n items are now in the array

		//start timing
		time_t start1 = time(0);
		int profit1 = knapsack1(num_items, max_weight, item_array);
		time_t end1 = time(0);
		int span1 = end1-start1;

		//solution 2
		int profit2 = 0;
		time_t start2 = time(0);
		knapsack2(num_items, max_weight, item_array, profit2, 0, 0, 0);
		time_t end2 = time(0);
		int span2 = end2-start2;
		
		cout << "Solution 1: N: " << num_items << " Profit: " << profit1 << " Time: " << span1 << endl;
		cout << "Solution 2: N: " << num_items << " Profit: " << profit2 << " Time: " << span2 << endl;
		
		fprintf(output_ptr, "%d %d %d %d %d\n", num_items, profit1, profit2, span1, span2);
	} //next problem
	fclose(output_ptr);
	fclose(input_ptr);
	return 0;
}

