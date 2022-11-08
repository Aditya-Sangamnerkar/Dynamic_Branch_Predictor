
#include<iostream>
#include <bits/stdc++.h> // pow
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// all counters are two bit long
#define counterMax 3
#define counterMin 0

// branch predictor parameters to run the simulator 
typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char*             bp_name;
}bp_params;

// each entry in the branch history table is uniquified by its index and counter 
struct BranchHistoryTableEnrty{
	int counter;
};


class BimodalBranchPredictor
{
public:

	int m;
	int branch_history_table_length;
	BranchHistoryTableEnrty* branch_history_table;
	int mispredictions;
    int predictions;

	
	BimodalBranchPredictor(int m);
	int getBranchIndex(int branch_program_counter);
	int predict(int index);
	void updateBranchPredictor(int index, int actual_outcome);
	void updateParameters(int prediction, int actual_outcome);
	void runBimodalPredictor(int branch_program_counter, char taken);
    void display_simulator();



};
void BimodalBranchPredictor :: runBimodalPredictor(int branch_program_counter, char taken)
{
	int actual_outcome = (taken == 't') ? 1 : 0;
	int index = this->getBranchIndex(branch_program_counter);
	int prediction = this->predict(index);
	updateBranchPredictor(index, actual_outcome);
	updateParameters(prediction, actual_outcome);
}


void BimodalBranchPredictor :: updateParameters(int prediction, int actual_outcome)
{
	if(actual_outcome != prediction)
		this->mispredictions++;
    this->predictions++;
}
void BimodalBranchPredictor :: updateBranchPredictor(int index, int actual_outcome)
{
	if(actual_outcome){
		if(this->branch_history_table[index].counter < counterMax)
			this->branch_history_table[index].counter++;
	}
	else{
		if(this->branch_history_table[index].counter > counterMin)
			this->branch_history_table[index].counter--;
	}
}

int BimodalBranchPredictor :: predict(int index)
{
	if(this->branch_history_table[index].counter >= 2 )
		return 1; // predict taken

	return 0; // predict not taken

}

int BimodalBranchPredictor :: getBranchIndex(int branch_program_counter)
{
	// get the (m+1)th to 2nd bit of the branch program counter
	int index_mask = ((1 << this->m) - 1) << 2;
	int index = (branch_program_counter & index_mask) >> 2;
	return index;

}

BimodalBranchPredictor :: BimodalBranchPredictor(int m)
{
		// initialize the branch predictor parameters
		this->m = m; //#index bits for the branch history register

		// initialize the branch history table
		this->branch_history_table_length = std::pow(2, m);
		this->branch_history_table = new BranchHistoryTableEnrty[this->branch_history_table_length];
		for(int i=0; i<this->branch_history_table_length; i++)
			this->branch_history_table[i].counter = 2;

		// initialize parameters
		this->mispredictions = 0;
        this->predictions = 0;
}
void BimodalBranchPredictor :: display_simulator()
{

    std::cout << "OUTPUT";
    std::cout << "\n number of predictions : " << this->predictions ;
    std::cout << "\n number of mispredictions : " << this->mispredictions;
    printf("\n misprediction rate : %.2lf %%", ((double)this->mispredictions/this->predictions)*100);
    std::cout << "\n FINAL BIMODAL CONTENTS ";
    for(int i=0 ;i < this->branch_history_table_length; i++){

        std::cout << "\n" << i << "  " << this->branch_history_table[i].counter;
    }

}

class GshareBranchPredictor
{
    public:

    int m,n;
    int branch_history_table_length;
    BranchHistoryTableEnrty* branch_history_table;
    int mispredictions;
    int predictions;
    int global_branch_history_register;

    
    GshareBranchPredictor(int m, int n);
    int getBranchIndex(int branch_program_counter);
    int predict(int index);
    void updateBranchPredictor(int index, int actual_outcome);
    void updateGlobalBranchHistoryRegister(int actual_outcome);
    void updateParameters(int prediction, int actual_outcome);
    void runGsharePredictor(int branch_program_counter, char taken);
    void display_simulator();
};

void GshareBranchPredictor :: runGsharePredictor(int branch_program_counter, char taken)
{
    int actual_outcome = (taken == 't') ? 1 : 0;
    int index = this->getBranchIndex(branch_program_counter);
    int prediction = this->predict(index);
    updateBranchPredictor(index, actual_outcome);
    updateGlobalBranchHistoryRegister(actual_outcome);
    updateParameters(prediction, actual_outcome);  
}
void GshareBranchPredictor :: updateParameters(int prediction, int actual_outcome)
{
    if(actual_outcome != prediction)
        this->mispredictions++;
    this->predictions++;
}
void GshareBranchPredictor :: updateGlobalBranchHistoryRegister(int actual_outcome)
{
    if(this->n > 0){
        this->global_branch_history_register = this->global_branch_history_register >> 1;
        int bhr_mask = actual_outcome << (this->n - 1);
        this->global_branch_history_register = this->global_branch_history_register | bhr_mask;
    }
}
void GshareBranchPredictor :: updateBranchPredictor(int index, int actual_outcome)
{
 if(actual_outcome){
        if(this->branch_history_table[index].counter < counterMax)
            this->branch_history_table[index].counter++;
    }
    else{
        if(this->branch_history_table[index].counter > counterMin)
            this->branch_history_table[index].counter--;
    }   
}
int GshareBranchPredictor :: predict(int index)
{
    if(this->branch_history_table[index].counter >= 2 )
        return 1; // predict taken

    return 0; // predict not taken
}

int GshareBranchPredictor :: getBranchIndex(int branch_program_counter)
{
    // get the (m+1)th to 2nd bit of the branch program counter
    int index_mask = ((1 << this->m) - 1) << 2;
    int index = (branch_program_counter & index_mask) >> 2;
    if(this->n > 0){
    // xor the upper n bits out of the m bits of the index
    int xor_mask = this->global_branch_history_register << (this->m-this->n);
    index = index ^ xor_mask;
    }
    return index;
}
GshareBranchPredictor :: GshareBranchPredictor(int m, int n)
{
    // initialize branch predictor parameters
    this->m = m; // #index bits for the branch history register
    this->n = n; // bits in global branch history register

    // initialize the branch history table
    this->branch_history_table_length = std::pow(2, m);
    this->branch_history_table = new BranchHistoryTableEnrty[this->branch_history_table_length];
    for(int i=0; i<this->branch_history_table_length; i++)
        this->branch_history_table[i].counter = 2;

    // initialize parameters
    this->mispredictions = 0;
    this->predictions = 0;

    // initialize global branch history register
    this->global_branch_history_register = 0;
}

void GshareBranchPredictor :: display_simulator()
{

	std::cout << "OUTPUT";
	std::cout << "\n number of predictions : " << this->predictions ;
	std::cout << "\n number of mispredictions : " << this->mispredictions;
	printf("\n misprediction rate : %.2lf %%", ((double)this->mispredictions/this->predictions)*100);
	std::cout << "\n FINAL GSHARE CONTENTS ";
	for(int i=0 ;i < this->branch_history_table_length; i++){

		std::cout << "\n" << i << "  " << this->branch_history_table[i].counter;
	}

}

class HybridBranchPredictor
{
public:
    BimodalBranchPredictor* bp1;
    GshareBranchPredictor*  bp2;
    int m1;
    int m2;
    int n;
    int k;
    BranchHistoryTableEnrty* chooser_table;
    int chooser_table_length;

   HybridBranchPredictor(int m1, int m2, int n, int k);
  
   int getBranchIndex(int branch_program_counter);
   void runHybridPredictor(int branch_program_counter, char taken);
   void display_simulator();

};

void HybridBranchPredictor :: display_simulator()
{
    std::cout << "OUTPUT";
    std::cout << "\n number of predictions : " << this->bp1->predictions + this->bp2->predictions;
    std::cout << "\n number of mispredictions : " << this->bp1->mispredictions + this->bp2->mispredictions;
    printf("\n misprediction rate : %.2f %%", ((float)(this->bp1->mispredictions + this->bp2->mispredictions)/(this->bp1->predictions + this->bp2->predictions))*100);
    std::cout << "\n FINAL CHOOSER CONTENTS ";
    for(int i=0 ;i < this->chooser_table_length; i++){

        std::cout << "\n" << i << "  " << this->chooser_table[i].counter;
    }
    
    std::cout << "\n FINAL GSHARE CONTENTS";
    for(int i=0 ;i < this->bp2->branch_history_table_length; i++){

        std::cout << "\n" << i << "  " << this->bp2->branch_history_table[i].counter;
    }
    std::cout << "\n FINAL BIMODAL CONTENTS";
    for(int i=0 ;i < this->bp1->branch_history_table_length; i++){

        std::cout << "\n" << i << "  " << this->bp1->branch_history_table[i].counter;
    }
}

void HybridBranchPredictor :: runHybridPredictor(int branch_program_counter, char taken)
{
    int actual_outcome = (taken == 't') ? 1 : 0;

    // prediction from bimodal
    int index_bimodal = bp1->getBranchIndex(branch_program_counter);
    int prediction_bimodal = bp1->predict(index_bimodal);

    // prediction from gshare
    int index_gshare = bp2->getBranchIndex(branch_program_counter);
    int prediction_gshare = bp2->predict(index_gshare);

    // branch index in chooser table
    int index_chooser = getBranchIndex(branch_program_counter);

    int prediction_flag = -1;

    // choose prediction from either gshare or bimodal
    if(this->chooser_table[index_chooser].counter >= 2)
        prediction_flag = 2; // gshare
    else
        prediction_flag = 1; // bimodal

    // update branch predictor based on prediction_flag
    if(prediction_flag == 1)
    {
        // update bimodal 
        bp1->updateBranchPredictor(index_bimodal, actual_outcome);
        bp1->updateParameters(prediction_bimodal, actual_outcome);
    }
    if(prediction_flag == 2)
    {
        // update gshare
        bp2->updateBranchPredictor(index_gshare, actual_outcome);
        bp2->updateParameters(prediction_gshare, actual_outcome);
    }

    // update gshare bhr always
    bp2->updateGlobalBranchHistoryRegister(actual_outcome);

    // update branch chooser counter
    if(actual_outcome == prediction_gshare && actual_outcome!=prediction_bimodal){
        if(this->chooser_table[index_chooser].counter < counterMax)
            this->chooser_table[index_chooser].counter++;
    }
    if(actual_outcome != prediction_gshare && actual_outcome == prediction_bimodal){
        if(this->chooser_table[index_chooser].counter > counterMin)
            this->chooser_table[index_chooser].counter--;
    }
}

int HybridBranchPredictor :: getBranchIndex(int branch_program_counter)
{
    // get the (k+1)th to 2nd bit of the branch program counter
    int index_mask = ((1 << this->k) - 1) << 2;
    int index = (branch_program_counter & index_mask) >> 2;
    return index;
}

HybridBranchPredictor :: HybridBranchPredictor(int m1, int m2, int n, int k)
{
   // initialize branch predictor parameters
   this->m1 = m1;
   this->m2 = m2;
   this->n = n;
   this->k = k;

   // initialize branch predictors
   bp1 = new BimodalBranchPredictor(this->m2);
   bp2 = new GshareBranchPredictor(this->m1, this->n);

   // initialize chooser table
   this->chooser_table_length = pow(2, k);
   this->chooser_table = new BranchHistoryTableEnrty[this->chooser_table_length];
   for(int i=0; i<this->chooser_table_length; i++)
        this->chooser_table[i].counter = 1;
}


int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file

    int predictor_select = 0;

    BimodalBranchPredictor* bp1 = NULL;
    GshareBranchPredictor* bp2 = NULL;
    HybridBranchPredictor* bp3 =  NULL;
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
        predictor_select = 1;
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);
         predictor_select = 2;

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);
         predictor_select = 3;
    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    if(predictor_select == 1)
        bp1 = new BimodalBranchPredictor(params.M2);
    if(predictor_select == 2)
        bp2 = new GshareBranchPredictor(params.M1, params.N);
    if(predictor_select == 3)
        bp3 = new HybridBranchPredictor(params.M1, params.M2, params.N, params.K);

    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        
        outcome = str[0];
        // if (outcome == 't')
        //     printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        // else if (outcome == 'n')
        //     printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        /*************************************
            Add branch predictor code here
        **************************************/
        if(predictor_select == 1 && bp1!=NULL)
            bp1->runBimodalPredictor(addr, outcome);
        if(predictor_select == 2 && bp2!=NULL)
            bp2->runGsharePredictor(addr, outcome);
        if(predictor_select == 3 && bp3!=NULL)
            bp3->runHybridPredictor(addr, outcome);
    }
    if(predictor_select == 1 && bp1!=NULL)
        bp1->display_simulator();
    if(predictor_select == 2 && bp2!=NULL)
        bp2->display_simulator();
    if(predictor_select == 3 && bp3!=NULL){
        bp3->display_simulator();

        
    }
    return 0;
}
