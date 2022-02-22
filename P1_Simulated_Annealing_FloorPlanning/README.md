# Simulated Annealing Based Slicing Flooplanning

Steps to run:
1. make
2. ./sa <input_file>
3. python FP_plotter.py

Input file format:
<module_name> <area> <aspect_ratio>

Tuning variables:
1. TempScaling: cool down rate when generating bad moves (to make runs more conservative)
2. tempConstraint: The lowest temperature to anneal till
3. timeOut: Time out for annealing
4. runMultiplier: iteration scaling per run

Results:
NOTE: Generated for the input_file.txt in the repo.

Initial random solution view: </br>
Area = 500 </br>
![image](https://user-images.githubusercontent.com/79747613/154202488-775a5e17-f03b-4706-92a0-3461d9b28ce9.png)

Final results:</br>
Area = 391</br>
![Capture](https://user-images.githubusercontent.com/79747613/155136482-8b53258f-9ace-423f-86a9-3a60145e995d.PNG)

NOTE: Fine tuning the input variables can yield better results

References:
[1] https://limsk.ece.gatech.edu/course/ece6133/slides/floorplanning.pdf
