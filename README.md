# rshell

rshell is a program I wrote that functions alomost the same sa BASH terminal.
it can run commands, and use seperators such as `;` , `||` ,and `&&`.
it also take in comments, which is indicated after the sign `#`. everything after `#` in a command line is a comment, and will not execute with the commands.

each command can take in arguments using space to seperate each term.

inputing `true` in the command line means "command" executed.
inputing `false` in the command line means "command" did not executed.

to exit the program, simply type `exit` as a command.


##EDGE CASES:
	1. seperator comes in pair: having `&&&` will consider having a seperator `&&` and a command `&`, which will return false. the same apply for `|||`.
	2. however, having `;;;` will means nothing, and the program does nothing.
	3. having single command with connector, for example `&& ls;`, the command ls will be executed, so as in the case of `ls &&;`. the same apply to seperator `||`.
	4. however, having a space before or after the connector will result in false, such as `  && ls;` or `ls &&    ;`.  

#How to run rshell

```
	$git clone https://github.com/sandiexie/rshell.git
	$cd rshell
	$git checkout hw0
	$make
	$bin/rshell
```

#Bugs
1. quotation marks will not work as expected.for example `echo "hi" ` will output `"hi"`.
2. spaces and tab by itself wont be ignored. and be run as error command.

