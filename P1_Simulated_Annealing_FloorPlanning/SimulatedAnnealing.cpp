/*
* Description:
*   Top file for simulated annealing based floor planning
* 
* Input file format:
*   <module_name> <area> <aspect_ratio>
*/

#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <iostream>
#include <string>
#include <random>
#include <cmath>

#include "HelperFuncs.h"
#include "PolishExpression.h"

int main(int argc, char** argv)
{
    PolishExpression currPolishExpression;
    int modulesCount = 0;
    if (argc == 1)
    {
        std::cerr << "Provide input module file as input with format: <module_name> <area> <aspect_ratio>\n";
        return 1;
    }
    std::string inputFile(argv[1]);
    std::ifstream FH(inputFile);
    if (!FH.is_open())
    {
        std::cerr << "Unable to open the input file " << inputFile << "\n";
        return 1;
    }
    std::string currentLine;
    while (std::getline(FH, currentLine))
    {
        // Remove extra whitespaces
        remove_newline(currentLine);
        remove_tabs(currentLine);
        remove_multiple_spaces(currentLine);

        // Split the input file based on spaces
        std::vector<std::string> currentLineVec = split_str(currentLine);
        if (currentLineVec.size() != 3)
        {
            std::cerr << "Incorrect format for input file";
            FH.close();
            return 1;
        }
        cirModule_t currModule;
        // Area = h*w, Aspect ratio = w/h
        currModule.name = currentLineVec[0];
        currModule.area = std::stof(currentLineVec[1]);
        currModule.aspectRatio = std::stof(currentLineVec[2]);
        currModule.height = std::sqrt(currModule.area / currModule.aspectRatio);
        currModule.width = std::sqrt(currModule.area * currModule.aspectRatio);
        currPolishExpression.add_module(currModule.name, currModule);
        ++modulesCount;
    }
    FH.close();

    // Simulated Annealing
    // Create random polish expression
    currPolishExpression.create_random_expression();
    // Inputs
    float tempScaling = 0.9f; float tempConstraint = 10.0f; float timeOut = 5;
    int runMultiplier = 5000; // k in the pseudo code
    // Init variables
    std::vector<std::string> best = currPolishExpression.get_polish_expression();
    int movesTried = 0, uphill = 0, reject = 0, maxRuns = runMultiplier * modulesCount, attempt = 0;
    float temperature, maxTemperature;
    temperature = maxTemperature = 1000;
    float bestCost = currPolishExpression.compute_area();
    // Create random number generator
    std::random_device rd;
    std::default_random_engine randGenerator{ rd() };
    // Range of distribution is 0->9 as grid positions are 2 columns * 5 rows = 10
    std::uniform_int_distribution<int> randDistribution(0, 100);
    // Init time
    std::chrono::system_clock::time_point startTime, currentTime;
    std::chrono::minutes runTime;
    startTime = std::chrono::high_resolution_clock::now();

    std::cout << "Initial random solution area: " << bestCost << "\n";    

    // SA loop
    do
    {
        movesTried = 0;
        uphill = 0;
        reject = 0;
        do
        {
            // Store current cost
            float oldCost = currPolishExpression.compute_area();
            std::vector<std::string> oldPolishExp = currPolishExpression.get_polish_expression();
            int moveType = select_move(temperature, maxTemperature);
            bool moveSuccess = false;
            switch (moveType)
            {
            case 1:
                moveSuccess = currPolishExpression.moveM1();
                break;
            case 2:
                moveSuccess = currPolishExpression.moveM2();
                break;
            case 3:
                moveSuccess = currPolishExpression.moveM3();
                break;
            default:
                break;
            }
            // if move attempt failed
            if (moveSuccess == false)
            {
                continue; // re-attempt move
            }
            ++movesTried;
            // Compute change in cost
            float newCost = currPolishExpression.compute_area();
            float delCost = newCost - oldCost;

            if ((delCost <= 0) || (randDistribution(randGenerator)/100.0f < std::exp((-1*delCost)/temperature)))
            {
                if (delCost > 0)
                {
                    ++uphill;
                }
                // E <- NE can be skipped my logic updates the current structure

                // Check if the solution is global best one so far
                if (newCost < bestCost)
                {
                    best = currPolishExpression.get_polish_expression();
                    bestCost = newCost;

                }
            }
            else
            {
                // Reject move
                ++reject;
                // Reset the polish expression
                currPolishExpression.update_expression(oldPolishExp);
            }
        } while ((uphill < maxRuns) && (movesTried < 2*maxRuns));

        // Update temperature
        temperature = tempScaling * temperature;

        // Calcuate runtime for time out check
        currentTime = std::chrono::high_resolution_clock::now();
        runTime = std::chrono::duration_cast<std::chrono::minutes>(currentTime - startTime);

        ++attempt;
        std::cout << "Attempt #" << attempt << ": Cost Value = " << bestCost << "\n";

    } while (
        (reject/ movesTried < 0.95) &&
        (temperature > tempConstraint) &&
        ((int)runTime.count() < 5)
        );
    currPolishExpression.update_expression(best);
    currPolishExpression.clear_module_placement();
    currPolishExpression.compute_area(true);
    currPolishExpression.print_modules();
    std::cout << "Best polish expression found:\n";
    currPolishExpression.print_expression(false);
    std::cout << "Best area: " << bestCost << "\n";

    std::cout << "Generated plot data file to use in FP_plotter.py\n";
    currPolishExpression.generate_plot_file();
}
