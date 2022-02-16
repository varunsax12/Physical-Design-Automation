# Simulated Annealing Based Flooplanning

Steps to run:
1. make
2. ./sa input_file.txt
3. python FP_plotter.py

Tuning variables:
1. TempScaling: cool down rate when generating bad moves (to make runs more conservative)
2. tempConstraint: The lowest temperature to anneal till
3. timeOut: Time out for annealing
4. runMultiplier: iteration scaling per run

References:
[1] https://limsk.ece.gatech.edu/course/ece6133/slides/floorplanning.pdf
