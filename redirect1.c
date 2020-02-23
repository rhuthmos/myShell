#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
//ls | wc -l
int main(int argc, char *argv[]){
	
	/* Verify that ls exists at /bin/ls *//* to verify run: which ls */
	char* const args[] = {argv[1], NULL};
	printf("Child is executing\n");
	/* write your code here *//* patch-1 */
	close(1);
	open(argv[2], O_RDWR | O_APPEND);
	int ret = execv (args[0], args);
	
	/* NOT REACHED*/
	printf ("Command not found\n");
	
	
	return 0;
}
