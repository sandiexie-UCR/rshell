#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include "command.h"
#include <string>
#include <signal.h>
#include <errno.h>
using namespace std;

void user()
{
	char* user_name = getlogin();
	if (!getlogin())
	{
		perror("getlogin");
	}

	char name [BUFSIZ];
	if ( gethostname(name, BUFSIZ) == -1 )
	{
		perror("gethostname");
	}
	
	char dir_name [BUFSIZ];
	if ( getcwd (dir_name, BUFSIZ) == NULL)
	{
		perror("getcwd");
	}

	char* home_name;
	home_name = getenv("HOME");
	if (home_name == NULL)
	{
		perror ("getenv");
	}

	string home_str = home_name;
	
	string dir_name_new = dir_name;
	
	if (home_str.length() <= dir_name_new.length())
	{
		dir_name_new = dir_name_new.substr(home_str.size());

		cout << user_name << "@" << name << ":" <<"~"<< dir_name_new <<  "$ ";
	}
	else
	{
		cout << user_name << "@" << name << ": " << dir_name << "$ ";
	}
	
	
}

bool execute ( char*argu[])
{
	int pass = 0;

	if( *argu == NULL )
	{
		return false;
	}

	pid_t pid = fork();

	if (pid == -1)
	{
		perror ("fork");
		exit (1);
		// iperror 
	}

	if (pid == 0)
	{
		pass = execvp (argu[0], argu);

		if( pass < 0)
		{
			perror ("exec");
			exit(1);
		} 
		// in child
	}
	if (pid > 0)
	{
		pid_t wait;
		do
		{
			wait = waitpid(pid, &pass, 0);
		} while (wait ==  -1 && errno == EINTR );
		if (wait < 0)
		{
			perror("wait");
		}

		if (pass > 0)
		{
			return false;
		}
		else 
		{
			return true;
		}
		//in parent
	}

	return true;
}

void run_cd( command a )
{
	if (a.get_arr()[1] != NULL)
	{
		string first = a.get_arr()[1];

		if ( first == "-")
		{
			// "cd -"
			//cout << "-" << endl;
			char* current;
			current = getenv("PWD");
			if (current == NULL)
			{
				perror ("getenv");
			}

			char* prev;
			prev = getenv("OLDPWD");
			if (current == NULL)
			{
				perror ("getenv");
			}
			
			if ( chdir(prev) == -1)
			{	perror ("chdir");	}


			if (setenv("OLDPWD", current,1) == -1)
			{	perror ("setenv");	}
		
			if ( setenv("PWD", prev ,1)  == -1)
			{	perror ("setenv");	}

			string print = prev;
			cout << print << endl;
		}
		else if ( first == "~")
		{
			char* current;
			current = getenv("PWD");
			if (current == NULL)
			{
				perror ("getenv");
			}

			char* home;
			home = getenv("HOME");
			if (current == NULL)
			{	perror ("getenv");	}

			if ( chdir(home) == -1)
			{	perror ("chdir");	}
		
			if (setenv("OLDPWD", current,1) == -1)
			{	perror ("setenv");	}
		
			if ( setenv("PWD", home ,1)  == -1)
			{	perror ("setenv");	}	
			// "cd ~"
			//cout << "~" << endl;
		}
		else 
		{
			char* current;
			current = getenv("PWD");
			if (current == NULL)
			{
				perror ("getenv");
			}

			if ( chdir (a.get_arr()[1]) == -1)
			{
				perror ("chdir");
			}
			
			if ( setenv ("OLDPWD", current, 1) == -1 )
			{
				perror ("setenv");
			}

			char aa[BUFSIZ];
			if ( getcwd (aa, BUFSIZ) ==NULL)
			{
				perror ("getcwd");
			}				

			if ( setenv ("PWD", aa, 1) == -1 )
			{
				perror ("setenv");
			}
	
			// "cd <PATH>"
			//cout << a.get_arr()[1] << endl;
		}
	}
	else if (a.get_arr()[1] == NULL)
	{
		// "cd"
		//cout << "cd" << endl;
		char* current;
		current = getenv("PWD");
		if (current == NULL)
		{
			perror ("getenv");
		}

		char* home;
		home = getenv("HOME");
		if (current == NULL)
		{	perror ("getenv");	}

		if ( chdir(home) == -1)
		{	perror ("chdir");	}
		
		if (setenv("OLDPWD", current,1) == -1)
		{	perror ("setenv");	}
		
		if ( setenv("PWD", home ,1)  == -1)
		{	perror ("setenv");	}
	}
	return;
}


void signal(int signum)
{
	if (signum == SIGINT)
	{
		// do nothing for Ctrl+C;
		cout <<endl;
	}
	else if (signum == SIGTSTP)
	{
		// ctrl+z;
		raise(SIGSTOP);
	}
}

struct sigaction act1, act2;

int main(int argc, char* argv[])
{
	act1.sa_handler = signal;
	//sigemptyset (&act1.sa_mask);
	//act1.sa_flags = 0;

	sigaction (SIGINT,NULL, &act2);
	if (act2.sa_handler != SIG_IGN)
	{
		sigaction (SIGINT,&act1, NULL);
	}
	sigaction (SIGTSTP,NULL, &act2);
	if (act2.sa_handler != SIG_IGN)
	{
		sigaction (SIGTSTP,&act1, NULL);
	}

while(1)
{
	//cout << "o" << endl;
	cin.clear();
	user();
	string input;
	getline(cin,input);

	size_t found =-1;
	found = input.find("#");
	
	if (found > 0)
	{
		input = input.substr(0, found);
	}
	
	char* temp_input = (char*) input.c_str();
	char* token;
	
	vector < vector<command> > syscalls;
	vector <command> inner;

	token = strtok (temp_input,";");

	// sperate command by ;

	command holder;

	if (token!=NULL)
	{
		holder.set_call (token,';');
		inner.push_back(holder);

		syscalls.push_back(inner);
	}
	inner.clear();

	while (token != NULL)
	{	
		token = strtok (NULL, ";");
		
		if (token != NULL)
		{
			holder.set_call(token,';');
			inner.push_back(holder);
			syscalls.push_back (inner);
		}
		inner.clear();
	}
	
	// sperate command by &  ==============================================================
	
	if (syscalls.size() > 0)
	{
		command holder1;
		
		for (unsigned int i = 0; i < syscalls.size(); ++i )
		{
			
			string whole1 = syscalls.at(i).at(0).get_call();
			string wholecopy1 = whole1;
			
			size_t found1 = whole1.find("&&");
			string call1;
			
			while ( found1 != string::npos)
			{
				call1 = wholecopy1.substr(0,found1);
				
				holder1.set_call ( call1, '&' );
				syscalls.at(i).push_back(holder1);
				
				whole1 = whole1.substr(found1+2,whole1.size());
				wholecopy1 = whole1;
			
				found1 = whole1.find("&&");			
			}

				if(whole1.length() != 0)
				{
					holder1.set_call (whole1, ';');
					syscalls.at(i).push_back(holder1);
				}

		}
		
		for (unsigned int u=0; u<syscalls.size(); u++)
		{
			syscalls.at(u).erase(syscalls.at(u).begin());
		}
		//cout << "sperate by & success" <<endl;

		// sperate command by ||  ======================================================
		// run throught all commands, sperate them by ||

		command holder2;
		for (unsigned int i=0; i < syscalls.size(); ++i)
		{
			unsigned int size = syscalls.at(i).size();
			int newsize;
			for (unsigned int j=0; j < size; ++j)
			{
				string whole2 = syscalls.at(i).at(j).get_call();
				string wholecopy2 = whole2;

				size_t found2 = whole2.find("||");
				string call2;

				while (found2 != string::npos)
				{
					call2 = wholecopy2.substr(0,found2);
					
					if (call2.length() > 0)
					{
						holder2.set_call (call2, '|');
						syscalls.at(i).push_back(holder2);
					}
					whole2 = whole2.substr(found2+2, whole2.size());
					wholecopy2 = whole2;

					found2 = whole2.find("||");		
				}
				
				if(whole2.length() != 0)
				{
					holder2.set_call (whole2, '&');
					syscalls.at(i).push_back(holder2);
				}

				newsize = syscalls.at(i).size();
			}
			
			
			syscalls.at(i).at(newsize-1).set_call(syscalls.at(i).at(newsize-1).get_call(), ';');	
		
			for (unsigned int y =0 ; y < size; y++)
			{
				syscalls.at(i).erase( syscalls.at(i).begin() );
			}
		}

		//cout << "seperate by || success" <<endl;
	}


	// print parsing =======================================================================
	// create char** ======================================================================
if (syscalls.size()>0)
{
	for (unsigned int i=0; i<syscalls.size(); i++)
	{
		//char* arr[100];
		if ( syscalls.at(i).size() > 0 )
		{

		for (unsigned int j=0; j<syscalls.at(i).size(); j++)
		{
			vector<string> tempA;
		
			
			char* whole_cell = (char*) (syscalls.at(i).at(j).get_call()).c_str();
			char* command1;
			
			command1 = strtok(whole_cell, " \t");
			if (command1 != NULL)
			{
				tempA.push_back(command1);
			}		
	
			while(command1 != NULL)
			{
				command1 = strtok (NULL, " ");
				if (command1 != NULL)
				{
					tempA.push_back(command1);
				}
			}
			if (tempA.size() > 0)
			{
				
				syscalls.at(i).at(j).set_copy ( tempA );
				syscalls.at(i).at(j).reset();
			}
		}
		}
}
// codes to test whats in char** arr[]================================================
// executing ==========================================================================
	
if (syscalls.size()>0)
{			
	for (unsigned int i=0; i <= syscalls.size()-1; i++)
	{
		bool e_or = false;
		bool e_and = true;
		
		if (syscalls.at(i).size() >0)
		{
		for (unsigned int j=0; j <= syscalls.at(i).size()-1; j++)
		{
			//cout << j << " <- " <<  *(syscalls.at(i).at(0).get_arr( )) << endl;
			string cd_w = syscalls.at(i).at(j).get_arr()[0];
			
			if ( (string)syscalls.at(i).at(j).get_arr()[0] == "cd")
			{
				cout << "doing cd..." << endl;
				run_cd(syscalls.at(i).at(j));
			}
			else if ( j==0 )
			{
				// doing with the first one of each vector
				// will ALWAYS run exec
				//cout << "the first one: " <<  *(syscalls.at(i).at(0).get_arr( )) << endl;	
				string aaa = *(syscalls.at(i).at(0).get_arr());

				if ( aaa  == "exit" )
				{
					exit(1);
				}
				if ( execute(syscalls.at(i).at(0).get_arr()) == true )
				{
					e_or = true;
					e_and = true;
				}
				else
				{
					e_or = false;
					e_and = false;
				}
			}
			//-------------------------------------------------------------
			//============================================================
			else
			{
				string bbb = *(syscalls.at(i).at(j).get_arr());
				if ( bbb  == "exit" )
				{
					exit(1);
				}
				// not the first one
				if ( syscalls.at(i).at(j-1).get_sperator() == '&')
				{
					// if following a &&
					if ( e_and == true )
					{
						if ( execute(syscalls.at(i).at(j).get_arr()) == true )
						{
							e_and = true;
						}
						else
						{
							e_and = false;
						}	
					}
					else
					{
						// do nothing
					}

					//----------------------------------------------

					if (  syscalls.at(i).at(j).get_sperator() == '|' )
					{
						e_or = e_and;
					}
					else
					{
						// do nothing
					}

				}
			//==============================================================
				else if ( syscalls.at(i).at(j-1).get_sperator() == '|')
				{
					// if following a ||
					if ( e_or == false )
					{
						if ( execute(syscalls.at(i).at(j).get_arr()) == true ) 
						{	
							e_or = true;
						}
						else
						{
							e_or = false;
						}
					}
					else if ( e_or == true)
					{
						// do nothing
					}

					
					//-----------------------------------------------

					if ( syscalls.at(i).at(j).get_sperator() == '&' )
					{
						e_and = e_or ;
					}
					else
					{
						// do nothing
					}
				}
			}
			


			
		}
		}
		}
	}
}
	// codes to test whats in char** arr[]================================================
		
}

}





