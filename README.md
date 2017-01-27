# Systems2Shell
Shell Script Program

This program was written for a systems lab. It's a unix shell that performs basic unix commands. It keeps track of the hisotry of commands entered via a text file. If "h" or "history" command is given it displays the last 12 commands that were entered. Entering the command "rr" will run the last command entered without adding it to the hisotry and the command "r " followed by number in the hisotry will run that command without adding it to the history.

To compile the file, type 'gcc -o shell2.c myshell' and to run program, type 'myshell'
