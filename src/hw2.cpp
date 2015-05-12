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
#include <fcntl.h>
#include <sys/stat.h>
using namespace std;

int SI = dup(1);
int SO = dup(0);

void user()
{
	char* user_name = getlogin();
	if (!getlogin())
	{
		perror("getlogin");
	}

	char name [1000];
	if ( gethostname(name, 1000) == -1 )
	{
		perror("gethostname");
	}

	cout << user_name << "@" << name << ":" << "$";
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
		pid_t wait = waitpid(pid, &pass, 0);
		
		if (wait < 0 )
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


void by_and (vector< vector <command> > & a)
{
	command holder1;
		
	for (unsigned int i = 0; i < a.size(); ++i )
	{
			
		string whole1 = a.at(i).at(0).get_call();
		string wholecopy1 = whole1;
			
		size_t found1 = whole1.find("&&");
		string call1;
			
		while ( found1 != string::npos)
		{
			call1 = wholecopy1.substr(0,found1);
				
			holder1.set_call ( call1, '&' );
			a.at(i).push_back(holder1);
				
			whole1 = whole1.substr(found1+2,whole1.size());
			wholecopy1 = whole1;
			
			found1 = whole1.find("&&");			
		}

		if(whole1.length() != 0)
		{
			holder1.set_call (whole1, ';');
			a.at(i).push_back(holder1);
		}

	}
		
	for (unsigned int u=0; u<a.size(); u++)
	{
		a.at(u).erase(a.at(u).begin());
	}

}

void by_or (vector < vector <command> > &a)
{
	command holder2;
		for (unsigned int i=0; i < a.size(); ++i)
		{
			unsigned int size = a.at(i).size();
			int newsize;
			for (unsigned int j=0; j < size; ++j)
			{
				string whole2 = a.at(i).at(j).get_call();
				string wholecopy2 = whole2;

				size_t found2 = whole2.find("||");
				string call2;

				while (found2 != string::npos)
				{
					call2 = wholecopy2.substr(0,found2);
					
					if (call2.length() > 0)
					{
						holder2.set_call (call2, '|');
						a.at(i).push_back(holder2);
					}
					whole2 = whole2.substr(found2+2, whole2.size());
					wholecopy2 = whole2;

					found2 = whole2.find("||");		
				}
				
				if(whole2.length() != 0)
				{
					holder2.set_call (whole2, '&');
					a.at(i).push_back(holder2);
				}

				newsize = a.at(i).size();
			}
			
			
			a.at(i).at(newsize-1).set_call(a.at(i).at(newsize-1).get_call(), ';');	
		
			for (unsigned int y =0 ; y < size; y++)
			{
				a.at(i).erase( a.at(i).begin() );
			}
		}
}
//====================================PIPING======================================

bool is_pipe (command a)
{
	string b = a.get_call();

	size_t finding = b.find(">");
	if (finding != string::npos)
	{
		return true;
	}
	
	finding = b.find ("<");
	if (finding != string :: npos)
	{
		return true;
	}

	finding = b.find ("|");
	if (finding != string :: npos)
	{
		return true;
	}

	return false;
}

vector<command> parsing_pipe (string a)
{
	char* aa = (char*) a.c_str();
	char* get;

	string bb="";

	get = strtok(aa, " ");
	if (get!=NULL)
	{
		bb = bb+get.c_str();
	}

	while(get!=NULL)
	{
		get = strtok(NULL, " ");
		if (get != NULL)
		{
			bb = bb+get.c_str();
		}
	}

	a = bb;
	cout << "after white space:" << a << endl;

	vector <command> re;
	command ree;

	vector <size_t> places;

	for (size_t it=0; it < a.length(); it++)
	{
		if ( a.at(it)=='>' || a.at(it)=='<' || a.at(it)=='|' )
		{
			//cout << it << endl;
			places.push_back(it);
		}
	}

	for (size_t it=0; it<places.size();it++)
	{
		string haha;
		if (it==0)
		{
			//cout << it << ": " << places.at(it)<< endl;
			haha = a.substr( 0, (places.at(it)-1));
			//cout<< haha;
		}
		else
		{
			haha = a.substr((places.at(it-1)+1), (places.at(it)-1));
		}

		char symbol = a.at(places.at(it));

		ree.set_call(haha, symbol);
		re.push_back(ree);

		if (it==(places.size()-1))
		{
			if (it != a.length()-1)
			{
				haha = a.substr ((places.at(it)+1), a.length()-1);
				symbol = '.';

				ree.set_call (haha,symbol);
				ree.reset();
				re.push_back(ree);
			}
		}
	}
	return re;
}

void pipe_in(command a, command b, string& output)
{
	// a < b;
}

void pipe_out (command a, command b, string& output)
{
	cout << "here" << endl;
	// a > b;
	//cout <<"get_call: " << b.get_call() << endl;
	int fdb = open ( b.get_call().c_str(), O_WRONLY | O_CREAT |O_TRUNC, 00664);
	if (fdb == -1)
	{
		perror ("open file");
	}

	pid_t pid = fork();

	if (pid == -1)
	{
		perror ("fork");
	}
	else if (pid == 0)
	{
		//close(1);
		//dup(fdb);

		cout << "arr: " << a.get_arr() << endl;
		if (execute(a.get_arr()) == false)
		{
			perror("execute");
		}
	}
	else if (pid > 0)
	{
		cout << "p" << endl;
		wait(0);
		//close (fdb);
		//dup2(SI,1);
	}

}


void run_pipe (command a)
{
	vector<command> list = parsing_pipe (a.get_call());	
	
	string temp;

	cout << "listsize: " << list.size()<< endl;

	for (unsigned int i =0; i<list.size(); i++)
	{
		cout << "S: " << list.at(i).get_sperator() << endl;
		if (list.at(i).get_sperator() == '<')
		{
			//pipe_in (list.at(i), list.at(i+1), temp);
		}
		else if (list.at(i).get_sperator() == '>')
		{
			cout << "here" << endl;
			pipe_out (list.at(i), list.at(i+1), temp);
		}
		else if (list.at(i).get_sperator() == '|')
		{
			//pipe_pipe
		}
		else
		{
			//last one, do nothing;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
while(1)
{
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
	
	
	if (syscalls.size() > 0)
	{
		// seperate by & ==============================================================
		by_and (syscalls);

		// sperate command by ||  ======================================================
		by_or (syscalls);
	}


	// print parsing =======================================================================
	// create char** ======================================================================

if (syscalls.size()>0)
{
	// seperate by space and put it in the "copy" of command.
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
			if ( j==0 )
			{
				// doing with the first one of each vector
				// will ALWAYS run exec
				
				string aaa = *(syscalls.at(i).at(0).get_arr());

				if ( aaa  == "exit" )
				{
					exit(1);
				}
				
				//syscalls.at(i).at(0).printc();
				// check to do pipe or regular excution
				if ( is_pipe(syscalls.at(i).at(0)) )
				{
					cout << "running piping....." <<endl;
					run_pipe (syscalls.at(i).at(0));
				}
				else
				{
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
					//-----------------------------------------------
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
					// check next and set new bool
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
					//---------------------------------------------
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
					// check next and set new bool
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

