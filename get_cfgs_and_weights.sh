#!/bin/bash

for FILE in $(grep -v "^\#\|^$" cfgs_and_weights.txt); do
wget -P models $FILE
done
