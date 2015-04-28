#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Timer.h"

using namespace std;
int main (int argc, char *argv[])
{
	char* i = argv[3];

	if (*i != '1')
	{
		struct stat s;

		if (stat(argv[2],&s) == 0)
		{
			perror("new file already exists.");
			exit(1);
		}

	// part1
	Timer t1;
	double eTime1;
	t1.start();

	ifstream fileold;
	fileold.open(argv[1]);
	if (!fileold.is_open())
	{
		perror("open1 fail");
		//exit(1);
	}

	ofstream filenew;
	filenew.open(argv[2]);
	if (!filenew.is_open())
	{
		perror ("open2 fail");
		//exit(1);
	}
	char c0;
	while (fileold.get(c0))
	{
		filenew.put(c0);
	}

	fileold.close();
	filenew.close();
	
	t1.elapsedUserTime(eTime1);
	cout << "Time1: " << eTime1 << endl;

	// part2
	
	Timer t2;
	double eTime2;
	t2.start();


	int fdnew;
	int fdold;

	if(-1 == (fdnew = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR))) 
	{
	    perror("There was an error with open(). ");
	        exit(1);
	}   
	
	if(-1 == (fdold = open(argv[1], O_RDONLY))) 
	{
	    perror("There was an error with open(). ");
		exit(1); 
	}

	int size;
	char c[1];

	if(-1 == (size = read (fdold, c, sizeof(c))))
	{
		perror("read failed");
	}
	while(size > 0)
	{
		if (-1 == write (fdnew, c, size))
		{
			perror ("error with write()");
		}
		if(-1 == (size = read(fdold, c, sizeof(c))))
		{
			perror("read2 failed");
		}
	} 


	if(-1 == close(fdnew))
	{
    		perror("There was an error with close(). ");
        	exit(1);
	}
	if(-1 == close(fdold))
	{
	    perror("There was an error with close(). ");
	    exit(1);
	}

	t2.elapsedUserTime(eTime2);
	cout << "Time2: " << eTime2 << endl;

	// part3
	Timer t3;
	double eTime3;
	t3.start();

	
	if(-1 == (fdnew = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR))) 
	{
	    perror("There was an error with open(). ");
	        exit(1);
	}   
	
	if(-1 == (fdold = open(argv[1], O_RDONLY))) 
	{
	    perror("There was an error with open(). ");
		exit(1); 
	}

 	size=0;
	char c1[BUFSIZ];

	if(-1 == (size = read(fdold, c1, sizeof(c1))))
	{
		perror("There was an error with read(). ");
		exit(1);
	}
	
	while(size > 0)
	{
		if(-1 == write(fdnew, c1, size))
		{
		perror("There was an error with write(). ");
		exit(1);
		}

		if(-1 == (size = read(fdold, c1, sizeof(c1))))
		{
			perror("There was an error with read(). ");
			exit(1);
		}
	}
	t3.elapsedUserTime(eTime3);
	cout << "Time3: " << eTime3 << endl;

	}

	else if (*i == '1')
	{
		int size=0;
		int fdnew;
		int fdold;

		int check;

	struct stat s;

	if (stat(argv[2],&s) == 0)
	{
		perror("new file already exists.");
		exit(1);
	}

	if(-1 == (fdnew = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)))
	{
	    perror("file already exists. ");
	        exit(1);
	}   
	
	if(-1 == (fdold = open(argv[1], O_RDONLY))) 
	{
	    perror("There was an error with open(). ");
		exit(1); 
	}

 	size=0;
	char c1[BUFSIZ];

	if(-1 == (size = read(fdold, c1, sizeof(c1))))
	{
		perror("There was an error with read(). ");
		exit(1);
	}
	
	while(size > 0)
	{
		if(-1 == write(fdnew, c1, size))
		{
		perror("There was an error with write(). ");
		exit(1);
		}

		if(-1 == (size = read(fdold, c1, sizeof(c1))))
		{
			perror("There was an error with read(). ");
			exit(1);
		}
	}
		
	}
	return 0;
}

