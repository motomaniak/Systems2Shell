/* Aaron Jenkins 
To Compile: gcc shell2.c -o myshell
To Run: myshell
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */

int count = 0;
char *cd = NULL;
struct history{
  int val;
  char command[80];
  struct history *next;
};

struct history *head = NULL;
struct history *curr = NULL;

struct history* createHistory(int val, char inBuffer[]){
  struct history *ptr = (struct history*)malloc(sizeof(struct history));
  ptr->val = val;
  strcpy(ptr->command, inBuffer);
  ptr->next = NULL;

  head = curr = ptr;
  return ptr;
}

void printHistory(void){
    int i = 0;
    struct history *ptr = head;
    int p = 1;
    while(p){
        if(ptr == NULL || i == 12){
            p = 0;
        }else{
            printf("\n [%d] %s \n", ptr->val, ptr->command);
            ptr = ptr->next;
        }
        i++;
    }
    return;
}

struct history* findCommand(int val, struct history **prev){
    struct history *ptr = head;
    struct history *temp = NULL;
    int found = 0;
    while(ptr != NULL){
        if(ptr->val == val){
            found = 1;
            break;
        }else{
            temp = ptr;
            ptr = ptr->next;
        }
    }

    if(found == 1){
        if(prev)
            *prev = temp;
        return ptr;
    }else{
      return NULL;
    }

}

struct history* addToHistory(int val, char inBuffer[], int onLoad){
    if(onLoad == 0){
        FILE *fp = fopen("history.txt", "ab+");
        fprintf(fp, "%s", inBuffer);
        fclose(fp);
    }
  
    if(NULL == head){
        return (createHistory(val, inBuffer));
    }

    struct history *command = NULL;
    command = findCommand(val, NULL);
    if(command != NULL){
        free(command);
    }
    struct history *ptr = (struct history*)malloc(sizeof(struct history));
    ptr->val = val;
    strcpy(ptr->command, inBuffer);
    ptr->next = NULL;

    ptr->next = head;
    head = ptr;
    return ptr;
}



/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also 
modifies the args parameter so that it holds points to the null-terminated strings which 
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/


void setup(char inBuffer[], char *args[],int *bkgrnd)
{
    int length,  /* #  characters in the command line */
        start,   /* Beginning of next command parameter           */
        i,       /* Index for inBuffer arrray          */
        j;       /* Where to place the next parameter into args[] */
	

    /* Read what the user enters */
    length = read(STDIN_FILENO, inBuffer, MAXLINE);  

    start = -1;
    j = 0;
    char cmdNum[10];

    struct history *command = NULL;

    if(strncmp("cd", inBuffer, 2) == 0){ 
		cd = strtok(inBuffer, " ");	
		cd = strtok(NULL, " ");
    }

    if(strncmp("r", inBuffer, 1) != 0){
		addToHistory(count, inBuffer, 0);
    }

	if(strncmp("rr", inBuffer, 2) == 0){
       	int val;
       	val = count -1;
       	command = findCommand(val, NULL);
       	if(command == NULL){
			printf("\n [%d] [%s} \n", val, "Command not found");
	 		exit(0);
		}
		printf("\n [%s] \n", command->command);
       	strcpy(inBuffer, command->command);
		length = strlen(inBuffer);
    }

	if(strncmp("r ", inBuffer, 2) == 0){
		strcpy(cmdNum, &inBuffer[2]);
   		int val;
   		val = atoi(cmdNum);
   		command = findCommand(val, NULL);
   		if(command == NULL){
 			printf("\n [%d] [%s} \n", val, "Command not found");
 			exit(0);
   		}
   		printf("\n [%s] \n", command->command);
   		strcpy(inBuffer, command->command);
   		length = strlen(inBuffer);
	}

    

    if (length == 0)
        exit(0);            /* Cntrl-d was entered, end of user command stream */

    if (length < 0){
        perror("error reading command");
	exit(-1);           /* Terminate with error code of -1 */
    }
    
    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {
        switch (inBuffer[i]){
        case ' ':
        case '\t' :          /* Argument separators */

            if(start != -1){
                args[j] = &inBuffer[start];    /* Set up pointer */
                j++;
            }

            inBuffer[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
                args[j] = &inBuffer[start];     
                j++;
            }

            inBuffer[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *bkgrnd = 1;
            inBuffer[i] = '\0';
            break;
            
        default :             /* Some other character */
            if (start == -1)
                start = i;
	}
 
    }   
    count++;
    args[j] = NULL; /* Just in case the input line was > 80 */
} 

int main(void)
{
	struct history *ptr = NULL;
    char inBuffer[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    int bkgrnd;             /* Equals 1 if a command is followed by '&', else 0 */
    char line[82];
    

	FILE *fp = fopen("history.txt", "ab+");
    while(fgets(line,sizeof line, fp) != NULL){
        addToHistory(count, line, 1);
        count++; 	   
    }
    fclose(fp);
    

    while (1){            /* Program terminates normally inside setup */

        bkgrnd = 0;
        printf("SysIIShell--> ");  /* Shell prompt */
        fflush(0);
        memset(inBuffer, 0, sizeof(inBuffer));	
        setup(inBuffer, args, &bkgrnd);       /* Get next command */
	
        if(strcmp("history", inBuffer) == 0 || strcmp("h", inBuffer) == 0){
            printHistory();
        }else if(strcmp("cd", inBuffer) == 0){
            int ret = chdir(cd);
            if(ret == -1){
                printf("ERROR: no such directory\n");
            }
        }else{
            pid_t pid;
            pid = fork();
            if(pid < 0){
                fprintf(stderr, "Fork Failed");
                return 1;
            }else if(pid == 0){
                if(execvp(args[0], args) < 0){
                    printf("ERROR: exec failed\n");
                    exit(1);
                }
            }else{
                if(bkgrnd == 0)
                    while(wait(NULL) == pid);
            }
        }
    }
}
