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
#include <fstream>
#include <sys/sendfile.h>
using namespace std;

int SO;

int SI;

void user()
{
	char* user_name = getlogin();
	if ( user_name  == NULL)
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

vector<command> parsing_pipe (char** b)
{
	int i = 0;	
	string a;
	//int j;
	while ( b[i] != NULL )
	{
		a = a + ' '+b[i];
		i++;
	}
	//cout << "after white space:" << a << endl;

	vector <command> re;
	

	vector <size_t> places;

	for (size_t it=0; it < a.length(); it++)
	{
		if ( a.at(it)=='>' || a.at(it)=='<' || a.at(it)=='|' )
		{
			places.push_back(it);
		}
	}

	for (size_t it=0; it < places.size();it++)
	{
		//cout << "it:" << places.at(it);

		command ree;
		
		string haha;
		if (it == 0)
		{
			haha = a.substr( 0, (places.at(it)));
		}
		else
		{
			//cout << "mid" << endl;
			haha = a.substr ( (places.at(it-1)+1), (places.at(it) - places.at(it-1))-1);
		}
		
		//cout << haha << endl;
		char symbol = a.at(places.at(it));

		ree.set_call(haha, symbol);
		
		vector <string> cpy;
		
		char* whole = (char*) haha.c_str();
		char* get2;

		get2 = strtok (whole, " ");
		if(get2 != NULL)
		{
			cpy.push_back(get2);
		}

		while (get2 != NULL)
		{
			get2 = strtok (NULL, " ");
			if (get2 != NULL)
			{
				cpy.push_back(get2);
			}
		}

		ree.set_copy(cpy);
		ree.reset();
		
		re.push_back(ree);
		
		if (it == (places.size()-1))
		{
			
			if (it != a.length()-1)
			{

				command ree2;
		
				haha = a.substr ((places.at(it)+1), a.length());
				symbol = '.';
				
				vector <string> cpy2;
		
				char* whole = (char*) haha.c_str();
				char* get2;

				get2 = strtok (whole, " ");
				if(get2 != NULL)
				{
					cpy2.push_back(get2);
				}

				while (get2 != NULL)
				{
					get2 = strtok (NULL, " ");
					if (get2 != NULL)
					{
						cpy2.push_back(get2);
					}
				}
				
				ree2.set_call(haha, symbol);
				ree2.set_copy(cpy2);
				//cout << "c_size:" << cpy2.size()<< endl;
				ree2.reset();
				re.push_back(ree2);
			}
		}
		//cout << "parsing1" << endl;
	}
	//cout << "parsing2" << endl;
	return re;
}

void pipe_in(command a, command b, string& output, bool follow , int fd_0, int fd_1)
{
	// a < b;

	int fdb = open (b.get_arr()[0], O_RDONLY);
	if (fdb == -1)
	{
		perror ("open file");
		if (b.get_arr()[0] == NULL)
		{
			cout << "missing input file." << endl;
		}
	}

	if (follow)
	{
		//cout << "in" << endl;
		//close(0);
		//dup(fdb);
		
		if ( close (1) == -1 )
		{
			perror ("close");
			exit(1);
		}

		if ( dup(fd_1) == -1 )
		{
			perror ("dup");
			exit(1);
		}

		
		int pid = fork();
		if (pid == -1)
		{
			perror ("fork");
		}
		if (pid == 0)
		{
			if ( close (0) == -1 )
			{
				perror ("close");
				exit(1);
			}
			if ( dup (fd_0) == -1)
			{
				perror ("dup");
				exit(1);
			}

			if (execute(a.get_arr()) == false )
			{
				//perror ("execute");
			}
			
			if ( close (fd_0) == -1 )
			{
				perror ("close");
				exit(1);
			}
			if ( dup2 (SI,0) == -1 )
			{
				perror ("dup2");
				exit(1);
			}
		}
		if(pid > 0)
		{
			if ( wait(0) ==-1)
			{
				perror ("wait");
			}
		}

		if ( close (fd_1) == -1 )
		{
			perror ("close");
			exit(1);
		}

		if ( dup2 (SO,1) == -1) 
		{ 
			perror("dup2"); 
		  	exit(1);
		}
		//dup2 (fd_1,SO);
		

		string wew;

		int size;

		char c[1];
		size = read (fd_1, (char*)wew.c_str(), sizeof(c));
		if (size == -1)
		{
			perror ("read");
		}

		//cout << "wew:" << wew << endl;
		while(size>0)
		{
			//cout << "111" << endl;
			wew.push_back(*c);
			size = read (fd_1,c ,sizeof(c));
			if (size == -1)
			{
				perror ("read");
			}
		}
		//read (fd_1, output.c_str , )
		//close (fdb);
		//dup2 (SI,0);

		output = wew;
	}
	else
	{
		//cout << "!in" << endl;
		if ( close(0) == -1 )
		{
			perror ("close");
			exit(1);
		}
		if  ( dup(fdb) == -1 )
		{
			perror ("dup");
			exit(1);
		}

		if (execute(a.get_arr())== false)
		{
			perror ("execute");
		}
	
		if ( close (fdb) == -1)
		{
			perror ("close");
			exit(1);
		}
		if ( dup2(SI,0) == -1 )
		{
			perror ("dup");
			//exit(1);
		}

	}
}

void pipe_out (command a, command b, string& output, bool has_prev, bool has_follow, char** s, int& fd_0, int& fd_1)
{
	//cout << a.get_call() << endl;
	//cout << b.get_call() << endl;
	if (has_prev)
	{
		int fdb2;
		if (b.get_arr()[0][0] == '>')
		{
			fdb2 = open (b.get_arr()[0], O_WRONLY | O_CREAT | O_APPEND, 006644);
			if (fdb2 == -1)
			{
				perror ("open file");
				if (b.get_arr()[0] == NULL)
				{
					cout << "missing output file." << endl;
				}
			}
		}
		else
		{
			fdb2 = open (b.get_arr()[0], O_WRONLY | O_CREAT | O_TRUNC, 006644);
			if (fdb2 == -1)
			{
				perror ("open file");
				if (b.get_arr()[0] == NULL)
				{
					cout << "missing output file." << endl;
				}
			}
		}
		
		if ( read (fd_0,(char*)output.c_str(), BUFSIZ) == -1 )
		{
			perror ("read");
		}
		//cout << "wait" << endl;
		if ( close (1) == -1 )
		{
			perror ("close");
			exit(1);
		}
		if ( dup (fdb2)== -1)
		{
			perror ("dup");
			exit(1);
		}
		
		if ( close (fd_0) == -1)
		{
			perror ("close");
			exit(1);
		}

		cout << output;

		if ( dup2 (fd_0,0) == -1 )
		{
			perror ("dup");
		}

		if ( close (fdb2) == -1 )
		{
			perror ("close");
			exit(1);
		}
		if ( dup2 (SO,1) == -1 )
		{
			perror ("dup");
		};
		
		char buf[BUFSIZ];
		if ( read (fd_0, buf, BUFSIZ) == -1)
		{
			perror ("read");
		}

		if (has_follow)
		{
		}
	}
	else
	{
		// a > b;
	
		int fdb = open ( b.get_arr()[0], O_WRONLY | O_CREAT |O_TRUNC, 00664);
		if (fdb == -1)
		{
			perror ("open file");
			if (b.get_arr()[0] == NULL)
			{
				cout << "missing output file." << endl;
			} 
		}
	
		if ( close(1) == -1 )
		{
			perror ("close");
			exit(1);
		}
		if ( dup(fdb) == -1)
		{
			perror ("dup");
			exit(1);
		}

		//cout << "arr: " << a.get_arr()[3] << endl;
		if (execute(a.get_arr()) == false)
		{
		//
		}

		//output = buf;
		//wait(0);
		if ( close (fdb) == -1 )
		{
			perror ("close");
			exit(1);
		}

		if ( dup2(SO,1) == -1 )
		{
			perror ("dup");
		}
	
		//fdb = open ( b.get_arr()[0], O_WRONLY | O_CREAT |O_TRUNC, 00664);
		//if (fdb == -1)
		//{
		//	perror ("open file");
		//	if (b.get_arr()[0] == NULL)
		//	{
		//		cout << "missing output file." << endl;
		//	} 
		//}
		
		int fb = open ( b.get_arr()[0], O_RDONLY);
		if (fb == -1)
		{
			perror ("open file");
			if (b.get_arr()[0] == NULL)
			{
				cout << "missing output file." << endl;
			} 
		}
		
		char te;	
		string word;

		ifstream file1;
		file1.open(b.get_arr()[0]);

		while (file1.get(te))
		{
			word.push_back(te);
		}
	
		//cout << "word"<< word << endl;

		output = word;
		fdb = open ( b.get_arr()[0], O_RDONLY);
		if (fdb == -1)
		{
			perror ("open file");
			if (b.get_arr()[0] == NULL)
			{
				cout << "missing output file." << endl;
			} 
		}
/*		close (0);
		dup (fb);

		//char qq[BUFSIZ];

		//read (fb, (char*)word.c_str(), BUFSIZ);		
		getline(cin,te);

		close (fb);
		dup2 (SI,0);

		//string te = qq;
		cout << "te: " << te << endl;
*/
		if ( close (1) == -1 )
		{
			perror ("close");
			exit(1);
		}
		if ( dup (fd_1) == -1 )
		{
			perror ("dup");
			exit(1);
		}

		cout << word;

		if ( close (fd_1) == -1 )
		{
			perror ("close");
			exit(1);
		}
		if ( dup2 (SO,1) == -1)
		{
			perror ("dup");
		}
	}

}


void pipe_pipe (command a, command b, bool in, bool follow, int *fd, string& output)
{
	int pi[2];
	if (-1 == pipe(pi))
	{
		perror ("pipe");
	}

	if (in)
	{
		if (follow)
		{
			size_t pid = fork();
			if (pid == 0)
				{

				if ( close (1)== -1)
				{
					perror ("close");
					exit(1);
				}
				if ( dup (fd[1]) == -1)
				{
					perror ("dup");
					exit(1);
				}
			
				if ( close (fd[0]) == -1 )
				{
					perror ("close");
					exit(1);
				}
	
				if(execute(b.get_arr()) == false)
				{
				}
			
				
				}
			if (pid > 0)
			{
				if ( wait(0) == -1 )
				{
					perror ("wait");
				}

				//close (0);
				if ( dup2(fd[0], 0) == -1 )
				{
					perror ("dup");
				}
			
				if ( close (fd[1]) == -1)
				{
					perror ("close");
					exit(1);
				}
				//dup2(SO,1);
			}
			//cout << "wow "<<wow << endl;
		}
		else
		{
			//write (fd_0, (char*)output.c_str(), BUFSIZ);
			size_t pid1 = fork();
			if (pid1 == -1)
			{
				perror("fork");
			}
			if (pid1 == 0)
			{
				//close (0);
				//close(fd[0]);
				if ( dup2 (fd[0],0) == -1)
				{
					perror ("dup");
				}
				if (execute(b.get_arr())== false)
				{
				}
			}
			if (pid1 > 0)
			{
				if ( wait(0) == -1 )
				{
					perror ("wait");
				}
				if ( dup2 (fd[0],0) == -1)
				{
					perror ("dup");
				}
			}
			//cout << "done" << endl;
		}
	}
	else
	{
		if (follow)
		{
			size_t pid = fork();
			if (pid == -1)
			{
				perror ("fork");
			}
			if (pid == 0)
				{

				if ( close (1) == -1 )
				{
					perror ("close");
					exit(1);
				}
				if ( dup (fd[1]) == -1)
				{
					perror ("dup");
					exit(1);
				}
			
				if ( close (fd[0]) == -1)
				{
					perror ("close");
					exit(1);
				}
	
				if(execute(b.get_arr()) == false)
				{
				}
			
				
				}
			if (pid > 0)
			{
				if ( wait(0) == -1 )
				{
					perror ("wait");
				}

				//close (0);
				if ( dup2(fd[0], 0) == -1 )
				{
					perror ("dup2");
				}
			
				if ( close (fd[1]) == -1 )
				{
					perror ("close");
					exit(1);
				}
				//dup2(SO,1);
			}	
			
			/*int pi[2];
			if (-1 == pipe(pi))
			{
				perror ("pipe");
			}

			close (0);
			dup(fd[0]);

			close (1);
			dup(fd[1]);

			if (execute(b.get_arr())==false)
			{}
						
			close (fd[1]);
			dup2 (SO,1.);
	
			close (fd[0]);
			dup2 (SI,0);
			
			close (1);
			dup(fd[1]);
			*/
			//read (pi[1], fd_1, BUFSIZ);
		}
		else
		{
		if ( close (1) == -1)
		{
			perror ("close ");
			exit(1);
		}
		if ( dup(fd[1]) == -1 )
		{
			perror ("dup");
			exit(1);
		}
		
		if (execute(a.get_arr())==false)
		{
		}
		
		if ( close (fd[1]) == -1 )
		{
			perror ("close");
			exit(1);
		}
		if ( dup2(SO,1) == -1 )
		{
			perror ("dup");
		}

			if ( close (0) == -1 )
			{
				perror ("close");
				exit(1);
			}
			if ( dup (fd[0]) == -1)
			{
				perror ("dup");
				exit(1);
			}
			
			if(execute(b.get_arr())==false)
			{}
		
			if ( close (fd[0]) == -1 )
			{
				perror ("close");
				exit(1);
			}
			if ( dup2 (SI,0) == -1 )
			{
				perror ("dup");
			}
		}

	}
}


void run_pipe (command a)
{
	//cout << "a call: " <<  a.get_call()<< endl;
	vector<command> list = parsing_pipe (a.get_arr());	
	
	string temp;
	bool has_fd = false;
	//cout << "listsize: " << list.size()<< endl;


	
	
	//int file0 = open ()
	
	char** store;

	for (unsigned int i =0; i < list.size(); i++)
	{
		int fd[2];
	if (-1 == pipe2(fd,O_CLOEXEC))
	{
		perror ("pipe");
	}

		bool for_in = false;
		if (i != list.size()-1)
		{
			if (list.at(i+1).get_sperator() == '>' || list.at(i+1).get_sperator() == '|')
			{
				store = list.at(i).get_arr();
				for_in = true;
			}
		}

		//cout << "S: " << list.at(i).get_sperator() << endl;
		if (list.at(i).get_sperator() == '<')
		{
			pipe_in (list.at(i), list.at(i+1), temp, for_in, fd[0], fd[1]);
			has_fd = true;
		}
		else if (list.at(i).get_sperator() == '>')
		{
			pipe_out (list.at(i), list.at(i+1), temp, has_fd,for_in, store, fd[0],fd[1]);
			has_fd = true;
		}
		else if (list.at(i).get_sperator() == '|')
		{
			pipe_pipe (list.at(i), list.at(i+1), has_fd, for_in, fd, temp);
		}
		else
		{
			//last one, do nothing;
		}
	}
	//close (fd[0]);
	//close (fd[1]);
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
 
SO = dup(1);
if (SO == -1)
{
	perror("dup");
	exit(1);
}

SI = dup(0);
if (SI == -1)
{
	perror("dup");
	exit(1);
}
while(1)
{
	int uid = fork();
	if (uid == 0)
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
					////cout << "running piping....." <<endl;
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
	exit(0);
}
else if (uid > 0)
	{
		if ( wait(0) == -1 )
		{
			perror ("wait");
		}
	}		
}

}

