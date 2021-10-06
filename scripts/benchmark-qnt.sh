#!/bin/bash

echo "target,epsilon,query,tempo_inicio,tempo_fim" > ../data/q-digest_profile_trace.csv
while IFS=, read -r t e q m f0;
do
    f=`echo $f0 | sed 's/\\r//g'`
    t1=`(date --rfc-3339=ns)`
    ../src/q-digest --val $t --eps $e --univ 5422196 --in "../data/INPUT_query_rank_t_$t.csv" "../data/network_flows.csv" "quant" >> "../data/QDIGEST_QUANT_T_$t-E_$e.csv"
    t2=`(date --rfc-3339=ns)`    
    echo "$t,$e,$q,$t1,$t2" >> ../data/q-digest_profile_trace.csv
    sleep 1
done < ../data/executions.csv
