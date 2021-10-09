/*
      Compile with flag -std=gnu99

      Implements a shell using fork(), wait() and execvp(), while adding history features.
*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#define MAX_LINE 80 /* The maximum length command */

char *lineReader(void);   //function that reads user's input.
char **split_line(char *line); //tokenizes the input.
int countWords(char *str);     //counts the number of arguments passed.
int returnIndex(char *str, int size);    //returns index from !N command if valid and -1 if invalid.
char *rtrim(char *str, const char *seps);   //trims space at the right
char *ltrim(char *str, const char *seps);   //trims space at the left
char *trim(char *str, const char *seps);  //uses rtim and ltrim to trim all whitespace from command
char *toLower(char *str);             //convert command to lower.

int main(){

  //char *args[MAX_LINE/2 + 1]; /* command line arguments */
  int should_run = 1; /* flag to determine when to exit program */
  char **history = NULL;     //stores history.
  int numC = 0;       //keeps track of total number of commands run since begininng.
  while (should_run) {

    bool isexc = false;
    printf("osh>");
    fflush(stdout);
    bool no_wait = false;
    char *line = lineReader();     //stores the line that is entered.

    char *sLine = strdup(line);


    if(strcmp(trim(sLine,NULL) ,"")==0){     //no input
      continue;
    }

    if(strcmp(toLower(trim(sLine, NULL)), "exit")==0){ //exit condition
      free(line);
      for(int i = 0; i < numC; i++)
        free(history[i]);
      free(history);
      return 0;
    }
    if(strcmp(trim(sLine, NULL), "!!")==0){     //set line to previous command
      if(numC==0){ //no commands yet
        printf("No commands in history\n");
        continue;
      }
      else{
        free(line);
        line = strdup(history[numC-1]);
        isexc = true;
        printf("%s",line);
      }
    }

    if((trim(sLine,NULL)[0]=='!') && (isexc == false)){  //!N case
      int index = returnIndex(sLine, numC);    //We have index of !N
      if(index==-1){
        printf("No such command in history\n");
        continue;
      }
      free(line);
      line = strdup(history[index-1]);
      printf("%s",line);
    }

    if(strcmp(toLower(trim(sLine, NULL)), "history")==0){ //print history
      if(numC==0){
        printf("No commands yet\n");
      }
      else{
        for(int i = numC - 1; i >= 0; i--){
          if(i == numC-11)
           break;
          printf("%d %s", i+1, history[i]);
        }
      }
      continue;
    }

      //add new line to history.
      history = (char **)realloc(history, (numC + 1) * sizeof(char *));
      history[numC++] = strdup(line);

    int argc = countWords(line);          //Stores the number of arguments.
    char **args = split_line(line);          //stores the arguments

    if(strcmp(args[argc-1],"&") == 0){           //checks for & command
      no_wait = true;
   }
   pid_t child_pid = fork();
   if(child_pid==0){  //child process
     if(no_wait==true){
       args[argc-1] = NULL; //removes trailing &
     }
     execvp(args[0], args);
     exit(0);      //prevents creation of extra process.
   }
   else if (child_pid > 0){  //parent process

     if (no_wait==true){    //handles cases with &
       continue;
     }
     else
      wait(NULL);
   }
   else{        //no fork
     printf("Error fork failed!!!\n");
     exit(1);
   }
  //free all memory that needs to be.
  free(args);
  free(line);
  free(sLine);
  }
  for(int i = 0; i < numC; i++)
    free(history[i]);
  free(history);
  return 0;
}

char *lineReader(){
  size_t bufSize  = MAX_LINE;
  char *line = NULL;
  line = (char *)malloc(bufSize * sizeof(char));
  if(line == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }
  getline(&line,&bufSize,stdin);
  return line;
}

#define OSH_TOK_DELIM " \n"
char **split_line(char *line)
{
  int bufsize = MAX_LINE*sizeof(char), position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, OSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += MAX_LINE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, OSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

#define OUT    0
#define IN    1

// returns number of words in str
int countWords(char *str)
{
    int state = OUT;
    int wc = 0;  // word count

    // Scan all characters one by one
    while (*str)
    {
        // If next character is a separator, set the
        // state as OUT
        if (*str == ' ' || *str == '\n' || *str == '\t')
            state = OUT;

        // If next character is not a word separator and
        // state is OUT, then set the state as IN and
        // increment word count
        else if (state == OUT)
        {
            state = IN;
            ++wc;
        }

        // Move to next character
        ++str;
    }

    return wc;
}

int returnIndex(char *line, int numC){      //-1 implies error, any integer implies valid answer.
  //line starts with !, must remove.
  line = trim (line,NULL);
  int size = strlen(line);    //total number of characters in line.
  if(size==1){  //error, no int.
    return -1;
  }
  //Must remove first(!) character
  int j = 0;
  char str[size];
   for(int i=1;i<size;i++){    //removes first and last letters
      str[j++]=line[i];
   }
   size = size-1;
   //remove trailing spaces....?

   //determine that all values in remaining string are digits.
   for(int i=0; i < size; i++){
     if(isdigit(str[i]))     //is a digit
      continue;
     else                   //not a digit.
      return -1;
   }
   //now the string is a valid integer.
   int n = atoi(str); //converts string to number.
   if((n>numC)||(n < (numC - 9))){    //invalid
     return -1;
   }
   return n;  //Valid input return the int.
}

char *rtrim(char *str, const char *seps)
{
    int i;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    i = strlen(str) - 1;
    while (i >= 0 && strchr(seps, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    return str;
}
char *ltrim(char *str, const char *seps)
{
    size_t totrim;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    totrim = strspn(str, seps);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) {
            str[0] = '\0';
        }
        else {
            memmove(str, str + totrim, len + 1 - totrim);
        }
    }
    return str;
}
char *trim(char *str, const char *seps)
{
    return ltrim(rtrim(str, seps), seps);
}
char *toLower(char *s){

  char *str = strdup(s);
  for(int i=0; str[i]!='\0'; i++)
    {
        if(str[i]>='A' && str[i]<='Z')
        {
            str[i] = str[i] + 32;
        }
    }
    return str;
}
