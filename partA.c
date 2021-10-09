/*

		Compile with -std=c99 flag

		This program uses wait() and fork(), and implements the Collatz conjecture, while providing the necessary error checking.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

bool isint(char *str);     //return True if int and False if not int. Also eliminates negatives.


int main()
{
	//pid_t cpid;
	char inpstr[11];
	printf("Enter a positive integer: ");
	scanf("%s", inpstr);         //recieve input from user
	if(!isint(inpstr)){    //removes negatives and strings and floats
		printf("Error, wrong input.\n");
		return 1;
	}
	long int n = atoi(inpstr);
	if(n <= 0){     //removes 0
		printf("Error, wrong input.\n");
		return 1;
	}
	pid_t child_pid = fork();
	if(child_pid == 0){   //in child process
	  while (1) {  //simple algorithm for collatz conjecture.
		  printf("%d ", n);
		  if (n == 1)   //exit case.
			  break;
		  if (n % 2 == 0) { //even
		  	n = n / 2;
		  }
	 	  else {  //odd
	 	 	  n = (3 * n) + 1;
 		  }
	  }
		printf("\n");
		exit(0);
  }

  else if (child_pid > 0){  //in parent process
	  wait(NULL);
 }
 else{ //fork fail
	 printf("Error fork Failed!!!\n");
	 exit(1);
 }
 return 0;
}

bool isint(char *str){     //return True if int and False if not int.Also eliminates negatives.
	int len = strlen(str);
	if(len>10){
		return false;  //greater than largest int.
	}
	for (int i=0; i<len; i++){
		if(isdigit(str[i]))  //is a digit, move on.
			continue;
		else
			return false;    //won't work
	}
	return true;
}
