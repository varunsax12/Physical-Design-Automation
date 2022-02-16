
#include <iostream>
#include <random>
#include <stack>
#include <fstream>

#include "PolishExpression.h"
#include "HelperFuncs.h"

// Create random number generator
std::random_device rd;
std::default_random_engine randGenerator{ rd() };
// Range of distribution is 0->9 as grid positions are 2 columns * 5 rows = 10
std::uniform_int_distribution<int> randDistribution(0, 100);

/*
* Base constructor
*/
PolishExpression::PolishExpression() {}

/*
* Constructor to create the vector of required size
* @param size -> number of modules in floorplan
*/
PolishExpression::PolishExpression(int size)
{
    // Logic for n modules, there will be n-1 partitions
    this->currExp.reserve(2 * size - 1);
    this->operandCountVec.reserve(2 * size - 1);
    this->operatorCountVec.reserve(2 * size - 1);
}

/*
* Function to clear the module placement data
*/
void PolishExpression::clear_module_placement()
{
    for (auto x : this->moduleList)
    {
        this->moduleList[x.first].placement = std::make_pair(0, 0);
    }
}

/*
* Getter for polish expression held
* @return polish expression held
*/
std::vector<std::string> PolishExpression::get_polish_expression()
{
    return this->currExp;
}

/*
* Constructor to create the initial random polish expression
* @param size -> number of modules in floorplan
* @param moduleList -> modules in the input file
*/
void PolishExpression::create_random_expression()
{
    // Logic for n modules, there will be n-1 partitions
    this->currExp.reserve(0);
    this->operandCountVec.reserve(0);
    this->operatorCountVec.reserve(0);
    // To track the modules added into the polish expression
    int modulesAdded = 0;
    // Flag to add the first partition after adding 2 modules, then 1 partition after each module
    bool firstPartitionAdded = false;
    for (auto currModule : this->moduleList)
    {
        // Add the operand
        this->currExp.push_back(currModule.first);
        ++modulesAdded;
        if (firstPartitionAdded == false && modulesAdded == 2)
        {
            // Add partition or operator
            this->currExp.push_back(V_t);
            firstPartitionAdded = true;
        }
        else if (firstPartitionAdded == true)
        {
            // Add partition or operator
            this->currExp.push_back(V_t);
        }
    }
    this->update_op_vector();
}

/*
* Function to update the polish expression
* @param inExpression -> input expression
*/
void PolishExpression::update_expression(std::vector<std::string> inExpression)
{
    this->currExp.clear();
    this->currExp.resize(0);
    this->operandCountVec.resize(0);
    this->operatorCountVec.resize(0);
    this->currExp = inExpression;
    this->update_op_vector();
}

/*
* Function to compute operator and operand count arrays
*/
void PolishExpression::update_op_vector()
{
    int operandCounter = 0, operatorCounter = 0;
    for (auto x : this->currExp)
    {
        if (is_operator(x))
        {
            // current entry is operator
            ++operatorCounter;
        }
        else
        {
            // current entry is operand
            ++operandCounter;
        }
        this->operandCountVec.push_back(operandCounter);
        this->operatorCountVec.push_back(operatorCounter);
    }
}

/*
* Function to add module to module list
* @param inName -> name of module
* @param inModule -> module details of type cirModule_t
*/
void PolishExpression::add_module(std::string inName, cirModule_t inModule)
{
    this->moduleList[inName] = inModule;
}

/*
* Function to verify balloting property
* @param index -> index to check balloting at
* 
* Logic: #operands > #operators
*/
bool PolishExpression::check_balloting_property(int index)
{
    return this->operandCountVec[index] > this->operatorCountVec[index];
}

/*
* Function to swap elements and update count vec
* @param operandIndex -> index of operand
* @param operatorIndex -> index of operator
*
* NOTE: Both operand and operator are adjacent
*/
bool PolishExpression::op_swap(int operandIndex, int operatorIndex, bool updateCounters = false)
{
    std::swap(this->currExp[operandIndex], this->currExp[operatorIndex]);
    
    if (updateCounters)
    {
        // Update the counters
        // NOTE: Only works on adjacent swaps
        // Eg: If order 3...H changes to H...3
        if (operandIndex < operatorIndex)
        {
            // Skip doing anything to i = operatorIndex as that
            // will have same count values finalll
            for (int i = operandIndex; i < operatorIndex; ++i)
            {
                this->operandCountVec[i]--;
                this->operatorCountVec[i]++;
            }
        }
        // Eg: If order H...3 changes to 3...H
        else // operandIndex > operatorIndex, cannot be equal
        {
            // Skip doing anything to i = operandIndex as that
            // will have same count values finalll
            for (int i = operatorIndex; i < operandIndex; ++i)
            {
                this->operatorCountVec[i]--;
                this->operandCountVec[i]++;
            }
        }
        return this->check_balloting_property(operandIndex) && this->check_balloting_property(operatorIndex);
    }
    return false;
}

/*
* Function to compute the area through the tree (post-ordered)
* @param currList: current expression
* @param moduleList: module details map
* @param generatePlotData: flag to indicate whether to generate plotting relevant data
* @return data struct of width, height, name (if return from leaf node)
* 
* NOTE: name will be used to create placement data for graph
* 
* Logic: Using stack based approach to ensure that logic follow bottom left to top right logic
* which requires iterating from left to right.
* Recursion based approach goes from right to left -> reverse => placement computation will be complicated
*/
float compute_area_wrapper(std::vector<std::string>& currList,
    std::unordered_map<std::string, cirModule_t>& moduleList, bool generatePlotData)
{
    int roomCount = 1;
    std::string roomName = "room";
    std::unordered_map<std::string, cirModule_t> room_map;
    std::stack<cirModule_t> moduleStack;
    int index = 0;
    while (index < currList.size())
    {
        std::string currElement = currList[index];
        if (is_operator(currElement))
        {
            // element is operator
            // Pop two elements
            cirModule_t module2 = moduleStack.top();
            moduleStack.pop();
            cirModule_t module1 = moduleStack.top();
            moduleStack.pop();
            // Create room module type to insert
            cirModule_t roomModule;

            if (is_vertical_partition(currElement))
            {
                // partition type is V
                roomModule.width = module1.width + module2.width;
                roomModule.height = std::max(module1.height, module2.height);
            }
            else // H_t
            {
                // partition type is H
                roomModule.width = std::max(module1.width, module2.width);
                roomModule.height = module1.height + module2.height;
            }
            
            roomModule.isModule = false;
            roomModule.name = roomName + std::to_string(roomCount);
            // Add the inner module data
            roomModule.module1 = module1.name;
            roomModule.isModule1mod = module1.isModule;
            roomModule.module2 = module2.name;
            roomModule.isModule2mod = module2.isModule;
            roomModule.partitionType = currElement;
            moduleStack.push(roomModule);
            room_map[roomModule.name] = roomModule;
            ++roomCount;

        }
        else
        {
            // element is operand => add to stack
            moduleStack.push(moduleList[currElement]);
        }
        ++index;
    }
    cirModule_t topRoom = moduleStack.top();
    float totalArea = topRoom.width * topRoom.height;

    if (generatePlotData)
    {
        // Build the graph plotting data from the room_map
        // to generate required data for plotting through python
        // in matplotlib
        
        // Clear the past placement data if any
        for (auto x : moduleList)
        {
            moduleList[x.first].placement = std::make_pair(0, 0);
        }
        // Room related data already clear

        // Re-using the moduleStack
        while (not moduleStack.empty())
        {
            cirModule_t currentRoom = moduleStack.top();
            moduleStack.pop();
            if (currentRoom.isModule)
            {
                // Update the reference in moduleList
                moduleList[currentRoom.name].placement = currentRoom.placement;
                continue;
            }
            cirModule_t module1, module2;
            // Get room1
            if (currentRoom.isModule1mod == false)
            {
                // module 1 is room
                module1 = room_map[currentRoom.module1];
            }
            else
            {
                // module 1 is module
                module1 = moduleList[currentRoom.module1];
            }
            // Get room2
            if (currentRoom.isModule2mod == false)
            {
                // module 2 is room
                module2 = room_map[currentRoom.module2];
            }
            else
            {
                // module 2 is module
                module2 = moduleList[currentRoom.module2];
            }
            // Module 1 is either the left or bottom irrespective of partition
            // so, its x,y will be same as currentRoom
            module1.placement = currentRoom.placement;
            // Add module1 to stack
            moduleStack.push(module1);

            // Calculate the x,y for module1
            if (is_horizontal_partition(currentRoom.partitionType))
            {
                // H_t
                module2.placement = std::make_pair(currentRoom.placement.first, currentRoom.placement.second + module1.height);
            }
            else // V_t
            {
                module2.placement = std::make_pair(currentRoom.placement.first + module1.width, currentRoom.placement.second);
            }
            // Add module2 to stack
            moduleStack.push(module2);
        }
    }

    return (totalArea);
}

/*
* Function to compute area
* @param generatePlotData: if plot data needs to be generated for python script
* @return float of area value
* 
* Logic 1: Recursion through tree (right to left)
* Logic 2: Add the elements to stack and pop-update when operator seen (left to right)
*/
float PolishExpression::compute_area(bool generatePlotData)
{
    this->clear_module_placement();
    return compute_area_wrapper(this->currExp, this->moduleList, generatePlotData);
}

/*
* Function to find random operator
* @return index of operator
*/
int PolishExpression::find_element(bool findOperator)
{
    int indexToCheck;
    do
    {
        indexToCheck = randDistribution(randGenerator) % this->currExp.size();
    } while (is_operator(this->currExp[indexToCheck]) != findOperator);
    return indexToCheck;
}

/*
* Function to perform move M1 operand swap
* @return bool -> if move successful
* 
* NOTE: No change in operator and operand count
*  => always skewed
*/
bool PolishExpression::moveM1()
{
    int index1 = this->find_element(false);
    int index2;
    do
    {
        index2 = this->find_element(false);
    } while (index1 == index2);
    // Safe to swap index1 and index2
    this->op_swap(index1, index2);
    return true;
}

/*
* Function to perform move M2 chain invert
* @return bool -> if move successful
* 
* Chain is the collection of H/V at a location
* If the chain length is 1, then flip only that
* If the chain is longer, then file the whole chain
* NOTE: No change in operator and operand count
*  => always skewed
*/
bool PolishExpression::moveM2()
{
    int index = this->find_element(true);
    int mainIndex = index;
    // Invert the partition type
    currExp[index] = invert_partition(currExp[index]);
    // Look for partition chain prior to this index
    while (index!= 0 && is_operator(currExp[index - 1]))
    {
        --index;
        // Invert the partition type
        currExp[index] = invert_partition(currExp[index]);
    }
    // Look for partition chain after the main index
    while (mainIndex != (this->currExp.size() - 1) && is_operator(currExp[mainIndex + 1]))
    {
        ++mainIndex;
        // Invert the partition type
        currExp[mainIndex] = invert_partition(currExp[mainIndex]);
    }
    return true;
}

/*
* Function to perform move M3 operator/operand swap
* @return bool -> if move successful
* 
* NOTE: Change in operand and operator count required
* NOTE: This allows only adjacent swaps
*/
bool PolishExpression::moveM3()
{
    bool moveSuccess = false;
    int triesLeft = M3TIMEOUT;
    //this->print_expression();
    while (moveSuccess == false && triesLeft > 0)
    {
        int operandIndex, operatorIndex;
        operandIndex = this->find_element(false);
        //int operatorIndex = this->find_element(true);

        // Supports only adjacent swaps
        // Check on index-1
        if (moveSuccess == false &&
            operandIndex != 0 &&
            is_operator(this->currExp[operandIndex - 1]) &&
            (2*this->operatorCountVec[operandIndex - 1] < (operandIndex+1)) &&
            // Check operand + 1 is not of the same type to make VV or HH post swap
            (
                (
                    is_horizontal_partition(this->currExp[operandIndex-1]) &&
                    not is_horizontal_partition(this->currExp[operandIndex+1])
                ) ||
                (
                    is_vertical_partition(this->currExp[operandIndex-1]) &&
                    not is_vertical_partition(this->currExp[operandIndex+1])
                )
            )
           )
        {
            if (
                this->op_swap(operandIndex, operandIndex - 1, true) == false)
            {
                // revert back as swap not possible
                this->op_swap(operandIndex - 1, operandIndex, true);
            }
            else
            {
                moveSuccess = true;
            }
        }
        // Check on index+1
        else if (moveSuccess == false &&
            operandIndex != (this->currExp.size() - 1) &&
            is_operator(this->currExp[operandIndex + 1]) &&
            (2*this->operatorCountVec[operandIndex + 1] < operandIndex) &&
            // Check operand - 1 is not of the same type to make VV or HH post swap
            (
                (
                    is_horizontal_partition(this->currExp[operandIndex + 1]) &&
                    not is_horizontal_partition(this->currExp[operandIndex - 1])
                    ) ||
                (
                    is_vertical_partition(this->currExp[operandIndex + 1]) &&
                    not is_vertical_partition(this->currExp[operandIndex - 1])
                    )
                )
            )
        {
            if (this->op_swap(operandIndex, operandIndex + 1, true) == false)
            {
                // revert back as swap not possible
                this->op_swap(operandIndex + 1, operandIndex, true);
            }
            else
            {
                moveSuccess = true;
            }
        }
        --triesLeft;
    }
    return moveSuccess;
}

/*
* Print polish expression
*/
void PolishExpression::print_expression(bool debug)
{
    print_vector(this->currExp);
    if (debug)
    {
        for (int i = 0; i < this->currExp.size(); ++i)
        {
            std::cout << i << " ";
        }
        std::cout << "\n";
        print_vector(this->operandCountVec);
        print_vector(this->operatorCountVec);
        print_vector(this->operandCountVec);
        for (int i = 0; i < this->currExp.size(); ++i)
        {
            std::cout << this->check_balloting_property(i) << " ";
        }
        std::cout << "\n";
    }

}

/*
* Print modules list
*/
void PolishExpression::print_modules()
{
    std::cout << "Name\tWidth\tHeight\tX\tY\n";
    for (auto x : this->moduleList)
    {
        std::cout << x.first << "\t" << x.second.width << "\t" << x.second.height << "\t" << x.second.placement.first << "\t" << x.second.placement.second << "\n";
    }
}

/*
* Generate plot file for python script
*/
void PolishExpression::generate_plot_file()
{
    std::ofstream OUTFH("plot_data.txt");
    if (!OUTFH.is_open())
    {
        std::cerr << "Unable to open the output plot data file: plot_data.txt\n";
        return;
    }
    OUTFH << "Name\tWidth\tHeight\tX\tY\n";
    for (auto x : this->moduleList)
    {
        OUTFH << x.first << " " << x.second.width << " " << x.second.height << " " << x.second.placement.first << " " << x.second.placement.second << "\n";
    }
}

/*
* Destructor for the class
*/
PolishExpression::~PolishExpression() {}

/*
* Function to check if element is operator
* @param inStr -> string to check
* @return bool if the index is an operator
*/
bool is_operator(std::string inStr)
{
    return (is_vertical_partition(inStr) || is_horizontal_partition(inStr));
}

/*
* Function to check if operator is vertical partition
* @param inStr -> string to check
* @return bool if string in vertical partition
*/
bool is_vertical_partition(std::string inStr)
{
    return inStr.compare(V_t) == 0;
}

/*
* Function to check if operator is horizontal partition
* @param inStr -> string to check
* @return bool if string in horizontal partition
*/
bool is_horizontal_partition(std::string inStr)
{
    return inStr.compare(H_t) == 0;
}

/*
* Function to select a move based on temperature
* @param inTemp -> temperature
* @param maxTemp -> maximum temperature
* @return int of move type to run
*/
int select_move(float inTemp, float maxTemp)
{
    // Make bigger moves at high temp
    int moveArr[100];
    float tempRatio = inTemp / maxTemp;
    if (tempRatio >= 0.75)
    {
        // Great chance of bigger moves
        int i = 0;
        for (; i < 25; ++i)
        {
            moveArr[i] = 1;
        }
        for (; i < (int)(25 + 75 / 2); ++i)
        {
            moveArr[i] = 2;
        }
        for (; i < 100; ++i)
        {
            moveArr[i] = 3;
        }
    }
    else if (tempRatio >= 0.25 && tempRatio < 0.75)
    {
        // Equal chance of all moves
        int i = 0;
        for (; i < (int)(100/3.0f); ++i)
        {
            moveArr[i] = 1;
        }
        for (; i < (int)(100*2/3.0f); ++i)
        {
            moveArr[i] = 2;
        }
        for (; i < 100; ++i)
        {
            moveArr[i] = 3;
        }
    }
    else // tempRatio < 0.25
    {
        // Great chance of smaller moves
        int i = 0;
        for (; i < (int)(75 / 2.0f); ++i)
        {
            moveArr[i] = 1;
        }
        for (; i < 75; ++i)
        {
            moveArr[i] = 2;
        }
        for (; i < 100; ++i)
        {
            moveArr[i] = 3;
        }
    }
    // Randomly select any move irrespective of temperature
    return moveArr[randDistribution(randGenerator)];
}

/*
* Function to flip the partition
* @param inPartition -> partition type
*/
std::string invert_partition(std::string inPartition)
{
    if (inPartition.compare(H_t) == 0)
    {
        return V_t;
    }
    else if (inPartition.compare(V_t) == 0)
    {
        return H_t;
    }
    std::cerr << "Invalid partition type passed\n";
    return "";
}