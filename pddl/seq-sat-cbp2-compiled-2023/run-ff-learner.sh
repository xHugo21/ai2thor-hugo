#! /bin/sh
echo "Tiempo limite" 

ulimit -t $5
# domaio problema algoritmo(14) Learner
system/ff-learner/ff-learner -p $1 -o $2 -f $3 -S 14 -L $4

 

