// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports five arguments
//struct will be used in order to store commands and pid from processes
//in order to implement history command and pidhostory
struct processInfo
{
	char *command[15];
	int PIDS[10];
};
//initialization of structure that will hold pids and processes to implement
//history command
struct processInfo PIDANDCOMMAND;
//tryExecuteCommand is the  function where everything happens. This function contains
//the bulk of the code. This function takes a pointer to the tokens of input
//as well as the number of tokens in order to implement the commands
int tryExecuteCommand(char *token[], int token_count)
{
	//if the token given is empty(NULL), we just return and get out of the
	//function without doing anything.
	if(token[0] == NULL)
		return 0;
	//if the first token is the word quit or the word exit, we exit out of
	//the shell successfully(with 0) otherwise, check the next statement
	else if(strcmp(token[0],"quit") == 0 || strcmp(token[0],"exit") == 0)
		exit(EXIT_SUCCESS);
	//if the first token is not empty, we go in and start checking for commands
	//in our child and parent processes
	else if(token[0] != NULL)
	{
		pid_t child_pid = fork();
		//paths for us to check for program implementatations. all bins and
		//current directories
		char *paths[] = {"./","/usr/local/bin","/usr/bin","/bin"};
		if( child_pid == 0 )
		{
			//child process with reactivate SIGINT and SIGTSTP so that our shell
			//gains the ability to stop processes within it, without having the
			//shell stop as well.
			if(SIGINT)
				signal(SIGINT, SIG_DFL); 
			else if(SIGTSTP)
				signal(SIGTSTP, SIG_DFL);
			int x;
			// loop user in order to check every path for commands for us to implement
			// starting from the current directory all the way down to the /bin directory
			for(x = 0; x < sizeof(paths)/sizeof(paths[0]);x++)
			{
				//cration of path for us to check for command
				char path[MAX_COMMAND_SIZE];
				strcpy(path, paths[x]);
				strcat(path,"/");
				strcat(path,token[0]);
				//our shell is taking in the path as well as ten other possible pieces
				//of input.
				execl(path,token[0],token[1],token[2],token[3],token[4],
				token[5],token[6],token[7],token[8],token[9],token[10],NULL );
			}
			//if a command is not found and it is not cd, we shall print command
			//not found. otherwise, we just move other to the parent process
			//quietly without printing anything
			if(strcmp(token[0],"cd") != 0)
			{
				printf("%s: Command not found.\n\n", token[0]);
			}
			exit( EXIT_SUCCESS );
		}
		else
		{
			int status;
			//wait for the child process to finish executing
			waitpid( child_pid, &status, 0 );
			//after the child process is done and we have the first token with the string
			//cd and nothig else in the next token, we just move to the previous directory
			if(strcmp(token[0],"cd")==0 && !token[1])
			{
				chdir("..");
			}
			//after the child is done and we have something in the first two tokens, with
			//the second thing being a path, we change directory into that one other path
			//after cd
			else if(strcmp(token[0],"cd")==0 && token[1])
			{
				chdir(token[1]);
			}
			return 0;
		}
		return 0;
	}
return 0; 
} 

int main()
{
	//we tell the program to ignore signals SIGINT and SIGTSTP in order to
	//prevent our shell from stopping when it gets these signals
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

	while( 1 )
	{
		// Print out the msh prompt
		printf ("msh> ");

		// Read the command from the commandline.  The
		// maximum command that will be read is MAX_COMMAND_SIZE
		// This while command will wait here until the user
		// inputs something since fgets returns NULL when there
		// is no input
		while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
		/* Parse input */
		char *token[MAX_NUM_ARGUMENTS];
		int   token_count = 0;
		// Pointer to point to the token
		// parsed by strsep
		char *arg_ptr;
		char *working_str  = strdup( cmd_str );
		// we are going to move the working_str pointer so
		// keep track of its original value so we can deallocate
		// the correct amount at the end
		char *working_root = working_str;
		// Tokenize the input stringswith whitespace used as the delimiter
		while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
				(token_count<MAX_NUM_ARGUMENTS))
		{
			token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
			if( strlen( token[token_count] ) == 0 )
			{
				token[token_count] = NULL;
			}
			token_count++;
		}
		// 
		//TryExecuteCommand function is used here in order to do all the
		//functionality of my shell taking in the tokens and token count given
		//by the code above provided by the teacher
		tryExecuteCommand(token,token_count);
		free( working_root );
	}
	return 0;
}
