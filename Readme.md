This simple shell implementation is my take on a project described in [this assignment](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/processes-shell) for the Operating Systems: Three easy pieces book. The only functionality I added beside the described are pipes.
# Supported features

 - Running commands found in the search path (more in the built-in section). Ex: `ls -l`
 - Multiple commands `ls & cat foo.txt & echo "bar"`
 - Output redirection `ls -l > foo.txt`
 - Pipes `ls -l | grep foo | wc`
 - Running a batch file containing a list of commands `./mishell batch.txt`
 - A usable CLI provided by the readline lib - better line editing and command history
 
# Built in commands

 - `path p1 p2` ... - Adds p1, p2... to the shell search path. The search path is a list of directories in which the shell searches for the command the user wants to execute. By default the search path is equal to `/usr/bin`.
 - `cd dir`- Change the current working directory to `dir`.
 - `exit`- exit the shell
# My opinion
Since this was the first bigger c-project I did I'm fairly satisfied. However the way I did parsing seems a bit hacky. I definitely to rewrite this part. Another one is error reporting which is almost nonexistent  because every failure results in the same "An error has occurred" message. 
