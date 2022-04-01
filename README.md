# CS214-WordWrap
Reiya Dave: rrd74
Meghana Kolanu: mrk169

PART 1: We produced different .txt files within the same directory as the ww.c file to test Part 1 of our code. We inserted lines after sentences, blank lines, tabs between words, and extra spaces between words to our text files to provide variations. We used a variety of these and modified the placement of our words in the text file. We also tested the application for different column lengths while making different permutations of the text files. By checking the right character length on each line, we were able to determine the accuracy of our outputs as well as hand-checking on a piece of paper.

PART 2: We built different directories within the same directory of ww.c to test our code for Part 2. We began by just generating one directory that included only one .txt file after extending the code from Part 1. We then added more .txt files to the directory to confirm that our program could wrap multiple files within a directory. To add on, we tested if our application could disregard all other files that were not .txt files.

In order to further ensure that our program ran properly, we ran comparison statements through the terminal to check that a wrapped file and rewrapped file contained the same number of bytes. Our checking included making sure the command 'cmp filename1 filename2' (where filename1 and filename2 are .txt files with the same words and paragraphs) printed nothing, meaning that the two files have the same amount of bytes and were wrapped correctly. 