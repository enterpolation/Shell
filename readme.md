# Unix shell
The classic Unix command line interpreter. Written in C using system
system calls.

##Main features
- External program startup:
```
$ ls -a
```
- Background processes (&):
```
$ xterm &
```
- I/O redirection (>, >>, <):
```
$ ls > file.txt
```
```
$ ls >> file.txt
```
```
$ cat < file.txt
```
- Pipeline (|):
```
$ cat < file.txt | sort | uniq
```
##How to execute
```
$ gcc -g -Wall -o main main.c
```