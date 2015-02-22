# rshell

rshell is a simple Unix command shell with the ability to:

| Features | Example |
|---|---|
| use GNU Coreutils                | pwd |
| use ;                            | pwd; ls |
| use &#124;&#124;                 | pwd &#124;&#124; ls |
| use &&                           | pwd && ls |
| use arguments without spaces     | ls-a-l |
| use >                            | ls > out.txt |
| use >>                           | ls >> out.txt |
| use <                            | ./a.out < in.txt |
| use <<<                          | cat <<< "hello world" |
| use &#124;                      | echo hello world &#124; cat |
| use 1> or 2>                    | ls 2> err.txt |
| use 1>> or 2>>                  | ls 2>> err.txt |


###exit command
The exit command (exit) allows user to quit rshell

###installation
to download and use rshell:
```
$ git clone  http://github.com/yliu075/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell
```

###known bugs

1. cannot use || before &&          example: pwd || ls && pwd

2. cannot use || before ||          example: pwd || ls || pwd

3. cannot use gethostname() and getlogin() when running on Windows

4. ~~sometimes requires two exits to quit~~ FIXED
 
5. cannot only use | and <<< by themselves

6. ~~cannot use both input redirection and output redirection together~~ FIXED



#ls command

My ls command supports:

| Features | Example |
|---|---|
| -a flag | bin/ls -a |
| -R flag | bin/ls -R |
| -l flag | bin/ls -l |
| optional files | bin/ls a.out |
| optional folders | bin/ls src |
| using multiple flags at once | bin/ls -aRl |
 
###installation
to download and use ls:
```
$ git clone  http://github.com/yliu075/rshell.git
$ cd rshell
$ git checkout hw1
$ make
$ bin/ls
```
###known bugs

1. ~~using the -R flag with -a in any instance creates recursive bombs if user has root access, recursion is limited to 10 for user without root access~~ FIXED
2. error with optional files and -l flag