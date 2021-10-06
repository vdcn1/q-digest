#!/usr/bin/env ipython

import random
import csv

EPS=[0.2 ,0.1, 0.05, 0.01]
TARGET= [0, 3, 4, 5, 6, 7]
QUERY = ["rank"]
MAX_ITEM = {
    0: 1522916,
    3: 17,
    4: 453190,
    5: 5422196,
    6: 678023588,
    7: 1345795830,
}
TOTAL_MASS = 3577297

for t in TARGET:
    queries = []
    for i in range(100):
        queries.append([random.randint(0, MAX_ITEM[t])])

    with open(f"../data/INPUT_query_rank_t_{t}.csv",'w') as out:
        assert len(queries) == 100
        csv_out = csv.writer(out)
        for row in queries:
            csv_out.writerow(row)


executions = [(t, e, q, MAX_ITEM[t], f"../data/INPUT_query_{q}_t_{t}.csv")
              for t in TARGET
              for e in EPS
              for q in QUERY]

random.shuffle(executions)

with open('../data/executions.csv','w') as out:
    csv_out=csv.writer(out)
    for row in executions:
        csv_out.writerow(row)
