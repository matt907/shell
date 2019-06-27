#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 80s

void setup(char inputBuffer[], char *args[], int *flag){
	int length, //number of chars in command line
	    i,      //loop index for accessing inputBuffer array
	    start,  //index where beginning of next command param is
	    ct;     //index of where to place the next param into args[]
	ct = 0;
	length = read(0,inputBuffer,MAX_LINE);
   strcpy(historyBuffer, inputBuffer);
	/*0 is the system predefined file descriptor for stdin, which is the 
	  user's screen in this case. inputBuffer by itself is the same as 
	  &inputBuffer[0], i.e. the starting address of where to store the 
	  command thatis read, and length hols the number of characters read in.
          inputBuffer is not a null terminated C-string*/
	
	start = -1;
	if(length == 0)
	   exit(0);  // ^d was entered, end of commands
	if(length < 0){
	   perror("error reading the command");
	   exit(-1); //terminate with error code of -1
	}

	for(i=0;i<length;i++){  //examine every character in the inputBuffer
		switch(inputBuffer[i]){
			case ' ':
			case '\t': //argument separtors
			if(start != -1){
			   args[ct] = &inputBuffer[start]; //set up pointer
			   ct++;
			}
			inputBuffer[i] = '\0'; //put in a null to end the argument
			start = -1;
			break;

			case '\n': 
			if(start != -1){
			   args[ct] = &inputBuffer[start];
			   ct++;
			}
			inputBuffer[i] = '\0';
			start -1;
			args[ct] = NULL; //no more arguments to this command
		break;

		default:  //some other character
			if(start == -1)
			   start = i;
			if(inputBuffer[i] == '&'){
			   *flag = 1;
			   inputBuffer[i] = '\0';
			}
		}//end of switch
	}//end of for
	args[ct] = NULL;
}//end of setup routine

void shellHandler(){ //^C exception
   

   printf(" COMMAND->\n");
}//end shellHandler

int main(void){
	int flag;                  //equals 1 if a command is followed by '&'
	char *args[MAX_LINE/2+1];  //command line (of 80) must have <40 arguments
	int child,    		   //process id of the child process
	    status;		   //result from execvp system call
	char inputBuffer[MAX_LINE];//buffer to hold the command entered
   char historyBuffer[10][MAX_LINE]; //create a history buffer that holds 10 commands
   
   signal(SIGINT, shellHandler); //called when ^C is pressed

	while(1){	//program terminates normally inside setup
	    flag = 0;
 	    printf(" COMMAND->\n");
	    setup(inputBuffer,args,&flag); //get next command
	    child = fork();   //creates a duplicate process
	    switch(child){
		case -1:
		    perror("Could not fork the process");
		    break;  /* perror is a library routine that displays a system
			       error message, according to the value of the system
			       vaiable "errno" which will be set during a function
			       (like fork) that was unable to successfully 
			       complete its task */
		case 0: //here is the child process
		    status = execvp(args[0], args);
		    if(status !=0){
			perror("Error in execvp");
			exit(-2); //terminate this process with error code -2
		    }
		    break;
		default:
		    if(flag==0) //handle parent, wait for child
			while(child != wait((int *) 0));
	    }//end switch
	}//end while
}//end main
