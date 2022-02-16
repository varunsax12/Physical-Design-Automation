#ifndef __POLISH_EXPRESSION_H__
#define __POLISH_EXPRESSION_H__

#include <vector>
#include <string>
#include <unordered_map>

/*
* Run constraints
*/
#define M3TIMEOUT 100

/*
* Partition types
*/
#define H_t "H"
#define V_t "V"

/*
* Type for the circuit modules
* to hold all the relevant data
*/
typedef struct cirModule_t
{
    float height;
    float width;
    float aspectRatio;
    float area;
    std::string name;
    std::pair<float, float> placement;
    // Room specific data (to be used if module is room)
    bool isModule;
    std::string module1;
    bool isModule1mod; // if module1 is module
    std::string module2;
    bool isModule2mod; // if module2 is module
    std::string partitionType;
} cirModule_t;

/*
* Type for custome return in the compute_area_wrapper
* function
*/
typedef struct area_return_t
{
    float width = 0;
    float heigth = 0;
    std::string name = "";
} area_return_t;

class PolishExpression
{
private:
    // To hold the current polish expression
	std::vector<std::string> currExp;
    // To hold the operand count per index
    std::vector<int> operandCountVec;
    // To hold the operator count per index
    std::vector<int> operatorCountVec;
    // To hold the moduleList
    std::unordered_map<std::string, cirModule_t> moduleList;

public:

    /*
    * Base constructor
    */
    PolishExpression();

    /*
    * Constructor to create the vector of required size
    * @param size -> number of modules in floorplan
    */
    PolishExpression(int size);

    /*
    * Getter for polish expression held
    * @return polish expression held
    */
    std::vector<std::string> get_polish_expression();

    /*
    * Function to clear the module placement data
    */
    void clear_module_placement();

    /*
    * Function to create the random polish expression from module list
    * @param size -> number of modules in floorplan
    * @param moduleList -> modules in the input file
    */
	void create_random_expression();

    /*
    * Function to update the polish expression
    * @param inExpression -> input expression
    */
    void update_expression(std::vector<std::string> inExpression);

    /*
    * Function to compute operator and operand count arrays
    */
    void update_op_vector();

    /*
    * Function to add module to module list
    * @param inName -> name of module
    * @param inModule -> module details of type cirModule_t
    */
    void add_module(std::string inName, cirModule_t inModule);

    /*
    * Function to verify balloting property (or tree skewed)
    * @param index -> index to check balloting at
    * @return bool if balloting property satisfied
    * 
    * Logic: #operands > #operators
    */
    bool check_balloting_property(int index);

    /*
    * Function to compute area
    * @param generatePlotData: if plot data needs to be generated for python script
    * @return float of area value
    */
    float compute_area(bool generatePlotData = false);

    /*
    * Function to swap elements and update count vec
    * @param operandIndex -> index of operand
    * @param operatorIndex -> index of operator
    * 
    * NOTE: Both operand and operator are adjacent
    */
    bool op_swap(int operandIndex, int operatorIndex, bool updateCounters);

    /*
    * Function to find random operator
    * @return index of operator
    */
    int find_element(bool findOperator);

    /*
    * Function to perform move M1 operand swap
    * @return bool -> if move successful
    *
    * NOTE: No change in operator and operand count
    *  => always skewed
    */
    bool moveM1();

    /*
    * Function to perform move M2 chain invert
    * @return bool -> if move successful
    *
    * NOTE: No change in operator and operand count
    *  => always skewed
    */
    bool moveM2();

    /*
    * Function to perform move M3 operator/operand swap
    * @return bool -> if move successful
    *
    * NOTE: Change in operand and operator count required
    */
    bool moveM3();
	
    /*
    * Print polish expression
    */
    void print_expression(bool debug = true);

    /*
    * Print modules list
    */
    void print_modules();

    /*
    * Generate plot file for python script
    */
    void generate_plot_file();

    /*
    * Destructor for the class
    */
    ~PolishExpression();
};

/*
* Function to compute the area through the tree (post-ordered)
* @param currList: current expression
* @param moduleList: module details map
* @param generatePlotData: if plot data needs to be generated for python script
* @return data struct of width, height, name (if return from leaf node)
*
* NOTE: name will be used to create placement data for graph
*
* Logic: Using stack based approach to ensure that logic follow bottom left to top right logic
* which requires iterating from left to right.
* Recursion based approach goes from right to left -> reverse => placement computation will be complicated
*/
float compute_area_wrapper(std::vector<std::string>& currList,
    std::unordered_map<std::string, cirModule_t>& moduleList, bool generatePlotData);

/*
* Function to check if element is operator
* @param inStr -> string to check
* @return bool if the string is an operator
*/
bool is_operator(std::string inStr);

/*
* Function to check if operator is vertical partition
* @param inStr -> string to check
* @return bool if string in vertical partition
*/
bool is_vertical_partition(std::string inStr);

/*
* Function to check if operator is horizontal partition
* @param inStr -> string to check
* @return bool if string in horizontal partition
*/
bool is_horizontal_partition(std::string inStr);

/*
* Function to select a move based on temperature
* @param inTemp -> temperature
* @param maxTemp -> maximum temperature
* @return int of move type to run
*/
int select_move(float inTemp, float maxTemp);

/*
* Function to flip the partition
* @param inPartition -> partition type
*/
std::string invert_partition(std::string inPartition);

#endif // !__POLISH_EXPRESSION_H__

