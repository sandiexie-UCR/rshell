#include <iostream>
#include <cstring>
#include <vector>
using namespace std;

class command
{
	private:
		string call;
		bool next;
		char sperator;
		char* arg[100];
		vector<string> copy;
	public:
		void set_copy( vector<string> );
		void reset();
		command();
		command(string,char);
		void set_next(bool);
		void set_sperator(char);
		void printc();
		void set_call(string, char);
		char get_sperator();
		string get_call();
		char** get_arr();
};

void command :: set_copy( vector<string> a )
{
	for (unsigned int e=0; e < a.size(); e++ )
	{
		copy.push_back(a.at(e));
	}
}
	
char** command :: get_arr()
{
	return arg;
}

char command :: get_sperator()
{
	return sperator;
}
void command :: reset()
{
	unsigned int keep;
	for (unsigned int t=0; t<copy.size(); t++)
	{
		arg[t] = (char*) copy.at(t).c_str() ;
		keep = t;
	}


	arg[keep+1] = NULL;	
		
	call = copy.at(0);	
}

command :: command ()
{
	next = 1;
	//arg[0] = NULL;
}
string command :: get_call()
{
	return call;
}


command :: command (string a, char d)
{
	sperator = d;
	call = a;
	next = 1;
	//arg[0] = NULL;
}

void command :: set_call (string a,char d)
{
	sperator = d;
	call = a;
	next = 1;
	//arg[0] = NULL;
}
void command :: set_next (bool b)
{
	next = b;
}

void command :: set_sperator(char c)
{
	sperator = c;
} 

void command :: printc ()
{
	cout << call<<"("<<sperator<<")";
}
