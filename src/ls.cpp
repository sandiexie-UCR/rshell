#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

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

bool comparedir (string a, string b)
{
	return a.compare(b) <0;
}

void sort_dir (vector<string>& f)
{
	sort(f.begin(), f.end(), comparedir);
}

void print_file (bool& do_all, bool& do_l, vector<string> filenames, string dir)
{
	if (!do_all)
	{
		//fix vector is it is not a
		while( filenames.at(0).at(0) == '.' )
		{
			filenames.erase(filenames.begin());
		}
	}
	
	if (! do_l )
	{
		// just print names
		for (unsigned int i =0; i < filenames.size();i++)
		{
			char *width = (char*)malloc(BUFSIZ);

			strcat (width,(char*) filenames.at(i).c_str());

			cout << filenames.at(i) << "  ";
		}
	}
	else
	{
		cout << "here"<< endl;
		//do l
		//print infos
		for (unsigned int i=0; i<filenames.size();i++)
		{
			string path = dir + '/' + filenames.at(i);

			cout << "::" << endl;
			struct stat f1;
			stat(path.c_str(), &f1);
			cout << "!!";	
			// access part:
			if (S_ISDIR(f1.st_mode))
			{
				cout << "rrrr";
				cout << 'd';
			}
			else if (S_ISLNK(f1.st_mode))
			{
				cout << 's';
			}
			else
			{
				cout << '-';
			}

			//check for each one:
			if (f1.st_mode & S_IRUSR)
				cout << 'r';
			else
				cout << '-';
			
			if (f1.st_mode & S_IWUSR)
				cout << 'w';
			else
				cout << '-';

			if (f1.st_mode & S_IXUSR)
				cout << 'x';
			else
				cout << '-';

			if (f1.st_mode & S_IRGRP)
				cout << 'r';
			else
				cout << '-';
			
			if (f1.st_mode & S_IWGRP)
				cout << 'w';
			else
				cout << '-';

			if (f1.st_mode & S_IXGRP)
				cout << 'x';
			else
				cout << '-';

			if (f1.st_mode & S_IROTH)
				cout << 'r';
			else
				cout << '-';
			
			if (f1.st_mode & S_IWOTH)
				cout << 'w';
			else
				cout << '-';

			if (f1.st_mode & S_IXOTH)
				cout << 'x';
			else
				cout << '-';

			cout << " ";

			struct passwd* name;
			struct group* group;

			string name_s;
			string group_s;

			string new1 = dir + '/' + filenames.at(i);
			
			name = getpwuid(f1.st_uid);

			group = getgrgid(f1.st_gid);
			
			name_s = name->pw_name;
			group_s = group->gr_name;

			string time;
			time = ctime(&f1.st_mtime);
			time = time.substr(4,12);

			cout.width(2); cout << right << f1.st_nlink;
			cout << " " << name_s;
			cout << " " << group_s;
			cout << " " << f1.st_size << " " << time << " ";

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
	cout << "R: " << has_R << endl;	
	//cout << directory.at(0)<< endl;
	//cout << directory.at(1);

	for (int i = 0; i < directory.size(); i++)
	{
		// loop for each directory

		vector <string> files;
			
		DIR *opened_dir = opendir((char* ) directory.at(i).c_str());
		
		cout << "222" << endl;

		if (opened_dir == NULL)
		{
			perror("open not existing directory.");
			exit(1);
		}

		dirent *a;
		while ( (a = readdir(opened_dir)) )
		{
			if (a == NULL)
			{
				perror ("read directory fail");
				exit(1);
			}

			string b = a-> d_name;
			files.push_back (b);
		}

		sort_dir (files);

		// finish getting files from directory----------------------------------------


		for (int y =0; y<files.size();y++)
		{
			cout << files.at(y)<< endl;
		}

		print_file (has_a, has_l, files, directory.at(i));
	}
	return 0;
}
