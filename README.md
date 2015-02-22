# rshell

rshell is a simple Unix command shell with the ability to:

| Features | Example |
|---|---|
| 1. run all commands found in /bin   | pwd |
| 2. use semi-colon (;)               | pwd; ls |
| 3. use or (||)                      | pwd || ls |
| 4. use and (&&)                     | pwd && ls |
| 5. use arguments without spaces     | ls-a-l |
| 6. use >                            | ls > out.txt |
| 7. use >>                           | ls >> out.txt |
| 8. use <                            | ./a.out < in.txt |
| 9. use <<<                          | cat <<< "hello world" |
| 10. use &#124;                      | echo hello world &#124; cat |
| 11. use 1> or 2>                    | ls 2> err.txt |
| 12. use 1>> or 2>>                  | ls 2>> err.txt |


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

1. -a flag
2. -R flag
3. -l flag
4. optional files and folders
5. using multiple flags at once such as -al
 
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