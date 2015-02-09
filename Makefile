all: bin
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls
	cp
rshell:
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
ls:
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls
cp:
	g++ src/cp.cpp -o bin/cp
bin:
	mkdir bin
