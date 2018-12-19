#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAXLEN 10
#define MAXCHAR 100
#define DEL " "
#include <stdbool.h>

extern int bg =0;
bool cd =0;

// function to read the command line and parse it to fill the input args array
// input args 2 has the final values that we want

int read_parse_line(char inputline[],char* inputargs[],char* inputargs2[])
{
    readline(inputline);
    processline(inputline,inputargs,inputargs2);
    return 1;
}
// the main function which reads the line that we want to parse
// the function handles case of exit and cd

void readline(char inputline[])
{
    char* ret = fgets(inputline,MAXCHAR,stdin);
    remove_endofline(inputline);
     if ((!strcmp(inputline,"exit")) || ret == NULL)
    {
        printf("%s", "\nGoodbye ..\n");
        exit(0);
    }
}

// function that removes the end of the commannd line as the end will be a new line \n

void remove_endofline(char inputline[])
{
    int i=0;
    while (inputline[i] != '\n')
    {
        i++;
    }
    inputline[i]='\0';
}

// function which handles signals when each child is terminated
// the function handles the exit of each child and appends a message to the log file
void signalhandler (int sig)
{
    pid_t pid;
   // pid = wait(NULL);
    char buf [100];
    int pd = pid;
    sprintf(buf,"Child Process was terminated.\n");
    Log (buf);
}
//this function is responsible for filling the args arrays,  and handling the background processes represented by &

int processline(char line[],char* args[],char* args2[])
{
    int i =0;
    int u =1;
    args[i] = strtok(line,DEL);
    bg =0;
    if (args[i] == NULL)
    {
        printf("NO COMMAND\n");
        return 1;
    }
    else {
    args2[i] = args[i];
    }

    while (args[i] != NULL)
    {
        if (!strcmp(args[i],"&"))
        {
       // args[i] =NULL;
        bg =1;
      //  break;
        }
        else bg =0;
        i++;
        args[i] = strtok(NULL, DEL);
    }

    for (u=0;u<i;u++)
    {
    if (strcmp(args[u],"&"))
    {
        args2[u] = args[u];
    }
    else {
        args2[u] = NULL;
        break;
    };
    }

    return 1;
}
// boolean value to see whether the log file is created or not
bool LogCreated = false;

// Log function is responsible in creating the log file and appending the message to it
// message is " Child process terminated"
void Log (char *message)
{
	FILE *file;

	if (!LogCreated) {
		file = fopen("x.log", "w");
		LogCreated = true;
	}
	else
	file = fopen("x.log", "a");
	if (file == NULL) {
		if (LogCreated)
			LogCreated = false;
		return;
	}
	else
	{
		fputs(message, file);
		fclose(file);
	}
	if (file)
		fclose(file);
}

int main()
{
    char* args[MAXLEN]; // the array of all arguments in the shell
    char* args2[MAXLEN]; // the array of the arguments without ampersand
    char line[MAXCHAR]; // entered line in the shell command
    int status;
    int result = 0;
    signal(SIGCHLD, signalhandler); //To handle the signals and see which child exited first\

    printf(">>");

   while (read_parse_line(line,args,args2)) // while there is a line to read
    {

    if (bg == 1)  // if its a background process do the following
    {
    printf(">>");
    pid_t pid = fork();

    if (pid >0) { // in parent do nothing

    }

   else  if (pid == 0)
    {
    setpgid(0,0);
    if (execvp(args2[0], args2) == -1) {
    perror("lsh");
    }
        //    exit(0);
    }

    else  {
        printf("\nFailed forking child..");
        return;
    }
    }

    else  //bg =0 not a background process
    {

    printf(">>");
  // pid is a variable of type pid_t to determine the process id (whether in parent or child)
    pid_t  wpid;
    pid_t pid = fork();

    if (pid > 0)
    {
        //parent process
       // waitpid(pid,NULL,0);
        do {
      wpid = waitpid(pid, NULL, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    }

    else if (pid == 0)
    {
    //the child process
    if (execvp(args2[0], args2)== -1) {
      perror("lsh");
    }

   // exit(0);

    }

     else  {
        printf("\nFailed forking child..");
        return;
    }
    }
}

    return 0;
}
