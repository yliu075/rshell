all:
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
rshell:
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
bin:
	mkdir bin