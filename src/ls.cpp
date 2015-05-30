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
#include <sys/ioctl.h>

using namespace std;

char blue[] = {"\033[1;34m"};
char white[] = {"\033[0;00m"};
char green[] = {"\033[1;32m"};
char gray[] = {"\033[1;47;34m"};
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

void get_directory (bool& a, bool& l, bool& R, bool& g, vector<string>& d, char* arg[], size_t num)
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


void dir_size( bool all, vector<string> a , string currdir)
{
	unsigned int size = 0;
	
	struct stat sss;
	
	unsigned int i =0 ;
	for (i =0 ; i<a.size(); i++)
	{
		string path = currdir + '/' + a.at(i);
		if ( stat(path.c_str(), &sss) == -1)
		{
			perror ("stat");
		}

		if ( !(all) && (a.at(i).at(0) == '.'))
		{
			size = size;
		}
		else
		{
			size = size + sss.st_blocks;
		}
	}
	
	cout << "total: " << size/2 << endl;

	return;
}

int get_max_size (vector <string> a)
{
	unsigned int max =0;
	for (unsigned t =0; t <a.size(); t++)
	{
		if (a.at(t).size() > max)
		{
			max = a.at(t).size();
		}
	}
	return max;
}
void print_file (bool& do_all, bool& do_l, vector<string> filenames, string dir, bool do_R, bool more_dir)
{
	//cout << endl;
	if (do_R || more_dir)
	{
		cout << dir << ": " << endl;
	}
	
	if (do_l)
	{
		dir_size(do_all, filenames, dir);
	}

	if (!do_all)
	{
		//fix vector is it is not a
			while( filenames.size()>0 && filenames.at(0).at(0) == '.' )
			{
				filenames.erase(filenames.begin());
			}	
	}
	
	//cout << "!!!!" << endl;	
	//cout <<"size: " << filenames.size() << endl;
	if (! do_l )
	{
		struct winsize tersiz;
		if ( ioctl (STDOUT_FILENO, TIOCGWINSZ, &tersiz) == -1 )
		{
			perror ("ioctl");
		}
		
		//cout <<  "size" << tersiz.ws_col <<endl;
		unsigned int terminalsize = tersiz.ws_col -3;
		
		unsigned int maxsize = get_max_size (filenames) + 2;
		
		//bool atmax = false;
		unsigned int current_size = 0;

		// just print names
		for (unsigned int i =0; i < filenames.size();i++)
		{
			//string width = (char*)malloc(BUFSIZ);

			//strcat ((char*)width.c_str(), (char*) filenames.at(i).c_str());
			
			string path = dir + '/' + filenames.at(i);
			struct stat ss;
			if ( stat(path.c_str(), &ss) == -1 )
			{
				perror ("stat");
			}
			
			if (current_size + maxsize +3 >  terminalsize )
			{
				cout << endl;
				current_size = 0;
			}
			else
			{
				current_size = current_size + maxsize + 3;

			}

			if (S_ISDIR(ss.st_mode))
			{
				if (filenames.at(i).at(0) == '.')
				{
					cout << gray << filenames.at(i) << white;
					for (unsigned int y = 0; y < (maxsize - filenames.at(i).size()) ; y++)
					{
						cout << " ";
					}
				}
				else
				{
					cout << blue << filenames.at(i) << white;
					for (unsigned int y = 0; y < (maxsize - filenames.at(i).size()) ; y++)
					{
						cout << " ";
					}
				}
			}
			else if (S_IXUSR & ss.st_mode)
			{
				cout << green << filenames.at(i) << white;
				for (unsigned int y = 0; y < (maxsize - filenames.at(i).size()) ; y++)
					{
						cout << " ";
					}
			}
			else
			{
				cout << filenames.at(i);
				for (unsigned int y = 0; y < (maxsize - filenames.at(i).size()) ; y++)
					{
						cout << " ";
					}
			}
		}
	}
	else
	{
		//cout << "here"<< endl;
		//do l
		//print infos
		for (unsigned int i=0; i<filenames.size();i++)
		{
			string path;
			path = dir + '/' + filenames.at(i);

			struct stat f1;
			if ( stat(path.c_str(), &f1) == -1 )
			{
				perror ("stat");
			}
			//cout << "1" << endl;	
			// access part:
			if (S_ISDIR(f1.st_mode))
			{
				//cout << "rrrr";
				cout << 'd';
			}
			else if (S_ISLNK(f1.st_mode))
			{
				cout << 's';
			}
			//cout << "3" << endl;
			else
			{
				cout << '-';
			}
			//cout << "2" << endl;
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

			//cout << " ";
			//cout <<  " 11 " << endl;
			struct passwd* name;
			struct group* group;
			
			string name_s;
			string group_s;
			//cout << "22" << endl;
			//string new1 = dir + '/' + filenames.at(i);
			
			name = getpwuid(f1.st_uid);
			if(name == NULL)
				perror("name");

			//struct stat f2;
			//stat(filenames.at(i).c_str(), &f2);
			

			group = getgrgid(f1.st_gid);
			if (group == NULL)
			{
				perror ("getgrgid");
			}
			if (group == NULL)
			{
				group_s = group -> gr_name; 
			}
				//perror("group");


			//cout << "33" << endl;
			name_s = name -> pw_name;
			group_s = group -> gr_name;

			//cout << "44" << endl;
			string time;
			time = ctime(&f1.st_mtime);
			time = time.substr(4,12);

			//cout << "55" << endl;
			cout.width(2); cout << right << f1.st_nlink;
			cout << " " << name_s;
			cout << " " << group_s;

			cout.width(7);
			cout << right << f1.st_size;
			cout << " " << time << " ";
			//cout <<filenames.at(i)<< endl;
		
			//struct stat ss;
			//stat(filenames.at(i).c_str(), &ss);
			
			if (S_ISDIR(f1.st_mode))
			{
				if (filenames.at(i).at(0) == '.')
				{
					cout << gray << filenames.at(i) << white << endl;
				}
				else
				{
					cout << blue << filenames.at(i) << white << endl;
				}
			}
			else if (S_IXUSR & f1.st_mode)
			{
				cout << green << filenames.at(i) << white << endl;
			}
			else
			{
				cout << filenames.at(i) <<  endl;
			}
			
		}
	}
	if (!do_l)
	{
		cout << endl;
	}
	cout << endl;
}


bool notcontain_slash (string a)
{
	int found = -1;
	found =  a.find("/");
	if (found > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool contain_dotdot (string a)
{
	int found = -1;
	found =  a.find("..");
	if (found >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void do_R (bool aaa, bool lll, vector<string> filesforR, string curr_dir, bool first)
{
	vector < vector<string> > needrecur;
	vector <string> needname;

	//bool dot = false;

	int do_from =0;
	
	if (aaa)
	{
		do_from = 2;
	}
	else
	{
		for (unsigned y  =0; y < filesforR.size();y++)
		{
			if (filesforR.at(y).at(0) == '.')
			{
				do_from ++;
			}
		}
	}
	
	for (unsigned int i = do_from; i < filesforR.size(); i++)
	{
		if (filesforR.at(i).size() >0)
		{
		//cout << filesforR.at(i) << "contain slash? " << notcontain_slash(filesforR.at(i))<<endl;

		if (1)
		{
			string path;
			// loop each files
			path = curr_dir + '/' + filesforR.at(i);
			//cout << path << "<-dirs"<<endl;

			struct stat f2;
			if ( stat(path.c_str(), &f2) == -1 )
			{
				perror ("stat");
			}

			vector<string> innerfiles;
			DIR *open_innerdir = opendir((char*) path.c_str());
			if (open_innerdir == NULL)
			{
				perror ("open fail.");
				//exit(1);
			}	
			if(S_ISDIR(f2.st_mode))
			{
				//cout <<  path << "this is dirs!!!!!!"<< endl;
				if (open_innerdir != NULL)
				{
				//	cout << "opened" << endl;
				//cout << path << "dir name " << endl;
				// is dir
					dirent *b;

					while ( (b = readdir(open_innerdir)) )
					{
						if (b == NULL)
						{
							perror ("read directory fail.");
							exit(1);
						}

						string c = b->d_name;
						innerfiles.push_back(c);
						//needname.push_back(path);
					}
					
					
					if (closedir(open_innerdir) == -1)
					{
						perror ("close directory");
					}
	
				}
			}
			else
			{
				// do nothing
			}
		
			sort_dir(innerfiles);
	
			if (innerfiles.size()>0)
			{
				if (filesforR.at(i)== "." || filesforR.at(i) == "..")
				{
				}
				else
				{
					//cout << "did push_back" << endl;
					needrecur.push_back(innerfiles);
					needname.push_back (path);
				}
			}
		
		}
				//dot =false;
		}
	}
	// after checking print content
	
	//string r = "";
	
	print_file(aaa,lll,filesforR,curr_dir,1, 1);

	//bool notf = false;
	//now do the innder dirs
	for (unsigned int q =0; q < needrecur.size(); q++)
	{
		//for (int w=0; w<needrecur.at(q).size();w++)
		//{
		//	cout << needrecur.at(q).at(w) << endl;
		//}
		do_R (aaa, lll, needrecur.at(q),needname.at(q),0);
	}
	//cout << endl;
	return;

	
}

bool is_a_file (string a)
{
	vector <string> check;

	string dir = ".";

	DIR *open = opendir ( (char*) dir.c_str() );
	
	if (open == NULL)
	{
		perror("open fail");
		exit(1);
	}
	dirent *b;
	while ( (b = readdir(open)) )
	{
		if (b == NULL)
		{
			perror ("read directory fail");
			exit(1);
		}

			string c = b-> d_name;
			check.push_back (c);
	}
	
	if (closedir(open) == -1)
	{
		perror ("close directory fail.");
	}
	//struct stat s;
	//stat( ('.' + '/'+ a).c_str(), &s );
	//if (S_ISDIR)

	for (unsigned r =0; r < check.size(); r++)
	{
		if (check.at(r) == a)
		{
			return true;
		}
	}

	return false;
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

	//cout << "a: " << has_a << endl;
	//cout << "l: " << has_l << endl;
	//cout << "g: " << has_g << endl;
	//cout << "R: " << has_R << endl;	
	//cout << directory.at(0)<< endl;
	//cout << directory.at(1);

	if (directory.size() == 0)
	{
		directory.push_back(".");
	}
	
	for (unsigned int i = 0; i < directory.size(); i++)
	{
		// loop for each directory

		vector <string> files;
			
		DIR *opened_dir = opendir((char* ) directory.at(i).c_str());
		if (opened_dir == NULL)
		{
			perror ("opendir");
		}
		//cout << "222" << endl;

		bool do_it = true;
		
		string qq = directory.at(i);
		string ww = "./"+qq;		
		struct stat s2;
		stat (ww.c_str() , &s2);

		if (opened_dir == NULL)
		{
			if (is_a_file (directory.at(i)) && !S_ISDIR(s2.st_mode) )
			{
				if (has_l)
				{
					vector<string> justone;
					string qwq = ".";
					bool QAQ = true;
					justone.push_back(directory.at(i));
					print_file (QAQ,QAQ,justone, qwq ,0,1);
				}
				else
				{
					cout << directory.at(i) << endl;
					cout << endl;
				}
				do_it = false;
			}
			else
			{
				//cout << directory.at(i) << endl;
				perror("opening not existing directory.");
				exit(1);
			}
		}

	if (do_it)
	{
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

		//if (closedir(opened_dir) == -1)
		//{
		//	perror ("close directory fail.");
		//}

		sort_dir (files);

		// finish getting files from directory----------------------------------------


		//for (int y =0; y<files.size();y++)
		//{
		//	cout << files.at(y)<< endl;
		//}

		if (!has_R)
		{
			print_file (has_a, has_l, files, directory.at(i),0, 1);
		}
		else
		{
			bool yesf = true;
			do_R ( has_a, has_l, files, directory.at(i), yesf);
		}

		if (closedir(opened_dir) == -1)
		{
			perror ("close directory");
		}
	}
	else
	{
		if (closedir(opened_dir) == -1)
		{
			//perror ("close directory");
		}
	}
	}
	
	
	return 0;
}
