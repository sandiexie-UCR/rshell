#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


void case_dash(bool& aa, bool& ll, bool& RR, bool& gg, char* word)
{
	int i = 1;
	while (word[i] != '\0')
	{
		if (word[i] == 'a')
		{
			aa = true;
		}
		else if (word[i] == 'l')
		{
			ll = true;
		}
		else if (word[i] == 'R')
		{
			RR = true;
		}
		else 
		{
			gg = true;
		}
		i++;
	}
}

void get_directory (bool& a, bool& l, bool& R, bool& g, vector<string>& d, char* arg[], int num)
{
	for (unsigned int i=1; i < num; i++)
	{
		// loop for checking each arguments
		if (arg[i][0] == '-')
		{
			// is arguments
			case_dash(a, l, R, g, arg[i]);
		}
		else
		{
			// is directory
			d.push_back( std::string (arg[i]) );
		}

	}
}

int main (int argc, char * argv[])
{
	bool has_a = false;
	bool has_l = false;
	bool has_R = false;
	bool has_g = false;

	vector <string> directory;

	get_directory(has_a, has_l,has_R,has_g, directory, argv, argc);

	if (!(!has_g))
	{
		perror ("non-flag exists in arguments.");
		exit(1);
	}

	cout << "a: " << has_a << endl;
	cout << "l: " << has_l << endl;
	cout << "g: " << has_g << endl;
	
	cout << directory.at(0);
	return 0;
}
