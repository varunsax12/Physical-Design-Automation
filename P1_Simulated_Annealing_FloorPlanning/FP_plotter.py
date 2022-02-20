#
# Script to convert the graph data dump from sa (*.cpp)
# into plots for viewing the floorplan
#

import random

if __name__ == "__main__":
    import matplotlib.pyplot as plt
    plt.axes()

    with open("plot_data.txt") as FH:
        lines = FH.readlines()
        for line in lines:
            line = line.strip()
            line = line.split(" ")
            name = line[0]
            if len(line) == 5:
                width = float(line[1])
                heigth = float(line[2])
                xPos = float(line[3])
                yPos = float(line[4])
                color = (random.random(), random.random(), random.random())
                rectangle = plt.Rectangle((xPos,yPos), width, heigth, fc=color,ec="black")
                plt.gca().add_patch(rectangle)
    plt.axis('scaled')
    plt.show()