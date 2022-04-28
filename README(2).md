Reiya Dave:
Meghana Kolanu: mrk169




To begin with, we made the word wrap function that takes multiple inputs of data and normalizes to a specific width using both C/Linux libraries. 
In terms of the test cases, we have empty and non-empty files and when normalized, the empty files consist of no bytes and the original file would empty or have white spaces. 
In the non empty files, they will be words which have multiple lines in length and have conditions that need to be justified. 
The conditions are that every line must fit a column width, a sequence of white spaces between characters with non whitespace, for that exceeds the column width, and have the last character as “/n” for non-empty files. 


We created two functions known as the enqueue and dequeue without using threads, using one thread for enqueue and one thread for dequeue, and have both threads use both functions. 

We made the word wrap file recursively by opening the file word wrap. We also two queues which are the file work queue and the directory work queue. 
We call the function word wrap and putting the path in the file work queue by using fileEnqueue and made a thread called file_worker. 
We also changed the basePath name to a new subdirectory and was able to call the function again recursively. 
We made a struct to put the arguments in and put the path into the directory work queue by using dirEnqueue and made a thread called dir_worker. 
The purpose of the dir_worker is to run wrapFilesRecursively using struct and dequeue the subdirectory once done with the thread. We had cases where we have a single thread or multiple threads traverses the directories and printed the file names and the directories as well as enqueue the file names into the dir_worker queue. 
The purpose of the file_worker is to run the word wrap function using struct and dequeue the file path once done. We had cases where we have a thread or multiple threads that enqueues and another single thread or multiple threads which dequeues the files that are wrapped. In terms of the threading, the worker functions are dequeueing.

