# rshell

rshell is a program I wrote that functions almost the same as BASH terminal.
it can run commands, and use sperators such as `;` , `||` ,and `&&`.
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


# Piping

Piping is a program I wrote that functions similar to bash i/o redirection.
It runs commands by using `>`, `<`, and `|`.

the symbol `>` is output redirection. It will put the output of command to the file on the left.

the symbol `<` takes the content of the file on the right, and input it to the command on the left.

the symbol `|` takes the output of the first command, and use it as the input of the next command.

##How to run rshell

```
	$git clone https://github.com/sandiexie/rshell.git
	$cd rshell
	$git checkout hw2
	$make
	$bin/rshell
```

##My program works in the form of below:

	file > file (do nothing) (error to exc "file")
	exc > file (write to file)
	file > exc ( do nothing ) (error)
	exc > exc (create a file named "exc" )

	file < file (error on excing)
	exc < exc  (open exc2 fail, run exc1)
	file < exc (error on excing)
	exc < file (run exc1)

	file | file (try to exc both, fail)
	exc | exc ( exc2 prints out)
	file | exc (try to exc1, fail)
	exc | file ( try to exc2, fail)

	special case:
	1. inputing many file to a command will only read the first file and print error on the second file.

## Bugs:
	1. some cases result in not responding terminal, didn't close file descriptots right.
	2. > output to multiple files does not work.
	3. chaining pipe together result in printing out content multiple times.

# cd

cd is a program that changes directories.

1. `cd <PATH>` brings to the folder with name `<PATH>`

2. `cd -` brings to the previous folder

3. `cd ~` beings to the home folder

## How to run cd

```
	$git clone https://github.com/sandiexie/rshell.git
	$cd rshell
	$git checkout hw3
	$make
	$bin/rshell
```

