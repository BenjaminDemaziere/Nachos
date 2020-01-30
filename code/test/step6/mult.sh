#!/bin/bash

# Met un sleep entre chaque commande pour laisser le temps de se connecter
cd ../../build
exec="nachos-mynetwork"
./$exec -rs -m 0 -x ./step6/multServeur -l 0.9 & 
sleep 0.1
./$exec -m 1 -x ./step6/multClient1 -l 0.9 &
sleep 0.1
./$exec -m 2 -x ./step6/multClient2 -l 0.9 &
sleep 0.1
./$exec -m 3 -x ./step6/multClient3 -l 0.9