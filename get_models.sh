#!/bin/bash

for FILE in $(grep -v "^\#\|^$" models_links.txt); do
wget -P models $FILE
done
