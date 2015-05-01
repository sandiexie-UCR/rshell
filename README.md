# rshell

rshell is a program I wrote that functions alomost the same sa BASH terminal.
it can run commands, and use seperators such as `;` , `||` ,and `&&`.
it also take in comments, which is indicated after the sign `#`. everything after `#` in a command line is a comment, and will not execute with the commands.

each command can take in arguments using space to seperate each term.

inputing `true` in the command line means "command" executed.
inputing `false` in the command line means "command" did not executed.

to exit the program, simply type `exit` as a command.


##Edge Case:
	1. seperator comes in pair: having `&&&` will consider having a seperator `&&` and a 
	   command `&`, which will return false. the same apply for `|||`.
	2. however, having `;;;` will means nothing, and the program does nothing.
	3. having single command with connector, for example `&& ls;`, the command ls will be 
	   executed, so as in the case of `ls &&;`. the same apply to seperator `||`.
	4. however, having a space before or after the connector will result in false, such 
	   as `  && ls;` or `ls &&    ;`.  

##How to run rshell

```
	$git clone https://github.com/sandiexie/rshell.git
	$cd rshell
	$git checkout hw0
	$make
	$bin/rshell
```

##Bugs
1. quotation marks will not work as expected.for example `echo "hi" ` will output `"hi"`.
2. spaces and tab by itself wont be ignored. and be run as error command.
3. comment by is self will report as error command.


# ls

ls is a program I wrote that prints out the files inside a directory.
It takes in three flags `-R`,  `-l` and  `-a` flags together or seperately.

`a` prints out all files including hidden files.
`R` prints the files inside the directory and the files in side the directories inside the current directory.
`l` prints out the informations about the files in the directory.


it runs commands in a format of :`bin/ls [flags] [directory]` where it can have multiple flags and directories.

Directories are printed out in blue. 
Hidden directories are printed out in blue and gray blackground. 
Executables are printed out in green. 

## Edge Cases:
	1. runing the program without flags will prints all non-hidden files.
  	2. runing without input directory names will print files in current directory.
	3. runing with invalide flags return an error message and exit program.
	4. runing with non existing directory return an error message and exit program.

##How to run rshell

```
	$git clone https://github.com/sandiexie/rshell.git
	$cd rshell
	$git checkout hw1
	$make
	$bin/ls
```

## Print out ordering

Files prints out in the order of :

	1. directory name
	2. hidden files
	3. capital case files
	4. lower case files

