# File-Analyzer #

Authors: Vincent Phung, William McFarland

### Description ###
Multithreaded program that analyzes a set of files and reports their similarities based on word frequencies.\
Computes the Jensen-Shannon distance between the word frequencies of pairs of files.


### Arguments ###
The program takes in the names of one or more files and directories, and up to four optional arguments specifying program parameters.\
Optional Parameters: N is a positive integer, S is a string.\
The default option for the threads is 1, and the default suffix is ".txt"
* Directory threads -dN
* File threads -fN
* Analysis threads -aN
* File name suffix -sS

Example: ./compare file1 directory1 -d3 -f1 -a2 -s.txt


