#include <iostream>
#include <cstring>
using namespace std;

class command
{
	private:
		string call;
		bool next;
		char sperator;
		char* arg[100];
	public:
		void reset(char*, char* []);
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

char** command :: get_arr()
{
	return arg;
}

char command :: get_sperator()
{
	return sperator;
}
void command :: reset(char* C, char* aa[])
{
	int t = 0;

	while (aa[t] != NULL)
	{
		arg[t] = aa[t];
		t++;
	}
	call = C;	
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
