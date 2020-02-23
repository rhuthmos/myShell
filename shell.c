#include <stdio.h>
#include <unistd.h>
//ls | wc -l
int main(){
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
		char* const args[] = {"/bin/ls", NULL};
		printf("Child is executing\n");
		/* write your code here *//* patch-1 */
		ret = execv (args[0], args);
		/* NOT REACHED*/
		printf ("failed to exec ls\n");
		}
	else if (pid > 0) {
		/* Verify that wc exists at /usr/bin/wc *//* to verify run: which wc */
		//char* const args[] = {"/usr/bin/wc", "-l", NULL};/* write your code here */
		/* patch-2 */
		//execv (args[0], args);
		/* NOT REACHED*/
		int pid2 = wait();
		printf ("Child exitted\n");
	}
	else {
		printf ("Unable to fork\n");
	}
	return 0;
}
