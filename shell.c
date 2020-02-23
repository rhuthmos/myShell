#include <stdio.h>
#include <unistd.h>
//ls | wc -l
int main(int argc, char *argv[]){
	int pid;
	int fd[2];
	int ret;
	ret = pipe (fd);
	if (ret == -1) {
		printf ("Unable to create pipe\n");
		return 0;
		}
	pid = fork ();
	if (pid == 0) {
	/* Verify that ls exists at /bin/ls *//* to verify run: which ls */
		char* const args[] = {argv[1], NULL};
		printf("Child is executing\n");
		/* write your code here *//* patch-1 */
		close(1);
		dup(fd[1]);
		close(fd[1]);
		close(fd[0]);
		ret = execv (args[0], args);
		/* NOT REACHED*/
		printf ("Command not found\n");
		}
	else if (pid > 0) {
		/* Verify that wc exists at /usr/bin/wc *//* to verify run: which wc */
		char* const args[] = {argv[2], "-l", NULL};/* write your code here */
		/* patch-2 */
		close(0);
		dup(fd[0]);
		close(fd[0]);
		close(fd[1]);
		execv (args[0], args);
		/* NOT REACHED*/
		int pid2 = wait();
		printf ("Child exitted\n");
	}
	else {
		printf ("Unable to fork\n");
	}
	return 0;
}
