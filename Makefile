all: bin
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls
rshell:
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
ls:
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls
bin:
	mkdir bin