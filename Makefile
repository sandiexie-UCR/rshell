COMPILER = g++
CFLAGS = -ansi -pedantic -Wall -Werror
RESULT = ./bin/rshell
DIRECTORY = ./src/ls.cpp
all:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(DIRECTORY) -o $(RESULT)
	$(COMPILER) $(CFLAGS) ./src/ls.cpp -o ./bin/ls


rshell:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(DIRECTORY) -o $(RESULT)

ls :
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) ./src/ls.cpp -o ./bin/ls
clean:
	rm -rf ./bin
