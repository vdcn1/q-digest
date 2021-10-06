import random
import csv

MAX_ITEM = {
    0: 1522916,
    3: 17,
    4: 453190,
    5: 5422196,
    6: 678023588,
    7: 1345795830,
}
TARGET= [0, 3,4,5,6,7]
TOTAL_MASS = 3577297

for t in TARGET:
    with open("../data/INPUT_query_quant_t_" + str(t) + ".csv", "x") as file:
        for i in range(100):
            value = random.randrange(TOTAL_MASS)
            file.write(str(value) + '\n')