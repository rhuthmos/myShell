#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<fcntl.h>
#include <sys/stat.h>


int checkArgument(char* src){
	char* ptr = src;
	int sz = 0;
	while(*(ptr+sz)!= ' '){
		if(*(ptr+sz)=='|' || *(ptr+sz)=='\0'){
			break;
		}
		sz++;
	}
	return sz;
}


int checkRedirect(char* src){
	char* ptr = src;
	while(*ptr == ' '){
		ptr++;
	}
	int sz = 0;
	
	bool out = false;
	while(*(ptr+sz) != ' '){
		if(*(ptr+sz)=='|' || *(ptr+sz)=='\0'){
			break;
		}
		if(*(ptr+sz) == '<' || *(ptr+sz) == '>'){
			if (*(ptr+sz-1)== ' ' || ((*(ptr+sz-1)=='1' || *(ptr+sz-1)=='2' ||*(ptr+sz-1)=='0' ) && *(ptr+sz-2)==' ')){
				out = true;
				sz++;
				if (*(ptr+sz)==*(ptr+sz-1)){
					sz++;
				}
				break;
			}
		}
		
		sz++;
	}
	if (!out){
		return 0;
	}
	while(*(ptr+sz)==' '){
		sz++;
	}
	while(*(ptr+sz)!=' '){
		if(*(ptr+sz)=='|' || *(ptr+sz)=='\0'){
			break;
		}
		sz++;
	}
	return sz;
}

void extractRedirect(char** redirection,int* t, char* src){
	char* ptr = src;
	int index= 0;
	while(*ptr == ' '){
		src++;
	}
	int type = 0;
	
	if (*ptr == '1' || *ptr == '2' || *ptr == '0'){
		if (*(ptr+1)=='>' || *(ptr+1)=='<'){
			index = ((int)*(ptr)) - 48;
			if(*(ptr+2)==*(ptr+1)){
				type = 1;
				ptr++;
			}
			ptr+=2;
		}
	}
	else if (*ptr == '>' || *ptr == '<'){
		if (*ptr == '>'){
			index = 1;
		}
		else{
			index = 0;
		}
		ptr++;
		if (*(ptr) == *(ptr-1)){
			type =1;
			ptr++;
		}
	}

	t[index] = type;

	while(*ptr == ' '){
		ptr++;
	}
	if (*ptr == '&'){
		redirection[index] = (char*)malloc(2*sizeof(char));
		memcpy(redirection[index], ptr+1, 1);
	}
	else{				
		int sz = 0;
		while(*(ptr+sz)!= ' '){
			sz++;
		}
		redirection[index] = (char*)malloc((sz+1)*sizeof(char));
		memcpy(redirection[index], ptr, sz);
	}


};





int main(int argc, char *argv[]){
	int pid;
	int status = 0;
	while(true){
		pid = fork();
		if (pid==0){
			write( STDOUT_FILENO, "$ ", 2 );
			char input[150]; //increase later
			size_t length = read(STDIN_FILENO, input, 148);
			input[length-1] = '\0';
			length--;
			if (strcmp(input, "exit")==0){
				write( STDOUT_FILENO, "Goodbye...\n", 11 );
				exit(20);
				break;
			}
			///////Number of pipes and initialisation///////
			int numpipes = 0;
			for (unsigned long int i=0; i<length; i++){
				if (input[i]=='|'){
					numpipes++;
				}
			}
			
			char** pipedTokens[numpipes+1];
			char* redirections[numpipes+1][3];
			for (int i=0; i<numpipes+1; i++){
				for (int j=0; j<3; j++){
					redirections[i][j] = NULL;
				}
			}
			int redType[numpipes+1][3];


			int counter = 0;
			int prev = 0;
			int numBytes = 1;
			int cmd1 = 0;
			int cmd2 = 0;
			int i = prev;

			/////////////////Preprocessing tokenwise////////////////
			while(counter<numpipes+1){
				int numArgs = 0;
				int numRed = 0;
				while (input[i]==' '){
						i++;
				}
				cmd1 = i;
				while(input[i]!= ' ' && input[i]!= '|' && input[i]!= '\0'){
					i++;
				}
				cmd2 = i;
				int a = 5;	//random value

				/////count numArgs//////////	
				while(input[i] != '|' && input[i] != '\0'){
					
					while(a!=0){
						while (input[i]==' ')
						{
							i++;
						}
						a = checkRedirect(&input[i]);
						if (a>0){
							numRed++;
						}
						i +=a;
					}
					while(input[i]==' '){
						i++;
					}
					a = checkArgument(&input[i]);
					i+=a;
					if(a!=0){
						numArgs++;
					}
					

				}

				/////////////////////////////////////

				///////command write//////////////////////
				pipedTokens[counter] = (char**) malloc((numArgs+2)*sizeof(char*));
				
				pipedTokens[counter][0] = (char*)malloc((cmd2-cmd1)*sizeof(char));
				
				memcpy(pipedTokens[counter][0], input+cmd1, cmd2-cmd1);
				i = cmd2;	// change to point after command

				int counter2 = 1;
				int counter3 = 0;
				/////////////////////////////////////Extract arguments and redirections////////////////////
				while(counter3<numArgs+numRed && input[i] != '|' && input[i] != '\0'){
					int a = 5;	//random value

					while(a!=0){
						while(input[i]==' '){
							i++;
						}
						a = checkRedirect(&input[i]);
						if (a>0){
							counter3++;
							extractRedirect(redirections[counter],redType[counter], &input[i]);
						}
						
						i +=a;
					}
					while(input[i]==' '){
						i++;
					}
					a = checkArgument(&input[i]);
					if (a>0){
						pipedTokens[counter][counter2] = (char*)malloc(a*sizeof(char));
						memcpy(pipedTokens[counter][counter2], &input[i], a);
						i+=a;
						counter3++;
						counter2++;
					}
					
				

				}
				////////////////////////////////////////////////////////////////////////Last Member put NULL////////////////
				while(input[i]==' ' ){
					i++;
				}
				if (input[i]=='|'){
					i++;
				}
				pipedTokens[counter][numArgs+1] = NULL;
				///add null value at the end///////
				counter++;
			}

		/////////////////Processing////////////////////////


			int mypipes[numpipes][2];
				for (int i=0; i< numpipes; i++){
					int a = pipe(mypipes[i]);
				}

			
			
			
			int ret;
			
			
			for (int i=0; i<numpipes; i++){
				int pid2 = fork();
				if (pid2==0){
					/////////////////////OUTPUT////////////////////////////////////
	
					if (redirections[i][1]!=NULL){
						if (*redirections[i][1]=='2'){
							close(1);
							dup(2);
						}
						else{
							close(1);
							if (redType[i][1]==1){
								open(redirections[i][1], O_WRONLY | O_CREAT | O_APPEND  , 0644);
							}
							else{
								open(redirections[i][1],  O_WRONLY | O_CREAT | O_TRUNC, 0644);
							}
						}
					}
					else{
						close(1);
						dup(mypipes[i][1]);
					}

						close(mypipes[i][1]);
						close(mypipes[i][0]);
					//////////////////////////INPUT/////////////////////////////
					if (redirections[i][0]!=NULL){
						if (*redirections[i][0]=='1'){
							;}
						
						else{
							close(0);
							open(redirections[i][0], O_WRONLY | O_CREAT | O_APPEND  , 0644);
						}
					}
					else{

						if (i!=0){
							close(0);
							dup(mypipes[i-1][0]);
							
							
						}
					}
					close(mypipes[i-1][0]);
							close(mypipes[i-1][1]);
					//////STDERR/////////////////////////////////////

					if (redirections[i][2]!=NULL){
						if (*redirections[i][2]=='1'){
							close(2);
							dup(1);
						}
						else{
							close(2);
							if (redType[i][2]==1){
								open(redirections[i][2], O_WRONLY | O_CREAT | O_APPEND  , 0644);
							}
							else{
								open(redirections[i][2],  O_WRONLY | O_CREAT | O_TRUNC, 0644);
							}
						}
					}

					///////////////////////////////////////////////////////////////
					execvp (pipedTokens[i][0], pipedTokens[i]);
				}
				else if (pid2>0){
					waitpid(pid2, &status, 0);
					close(mypipes[i][1]);
				}
				else{
					return 1;
				}

			}
			if (redirections[numpipes][1]!=NULL){
				if (*redirections[numpipes][1]=='2'){
					close(1);
					dup(2);
				}
				else{
					close(1);
					if (redType[numpipes][1]==1){
								open(redirections[numpipes][1], O_WRONLY | O_CREAT | O_APPEND  , 0644);
							}
							else{
								open(redirections[numpipes][1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
							}
				}
			}
			//////////////////////////INPUT/////////////////////////////
			if (redirections[numpipes][0]!=NULL){
				if (*redirections[numpipes][0]=='1'){
					;
				}
				else{
					close(0);
					open(redirections[numpipes][0], O_RDWR | O_APPEND);
				}
			}
			else{

				if (numpipes>0){
					close(0);
					dup(mypipes[numpipes-1][0]);
					
				}
			}
			close(mypipes[numpipes-1][0]);
			close(mypipes[numpipes-1][1]);
				

			//////STDERR/////////////////////////////////////

			if (redirections[numpipes][2]!=NULL){
				if (*redirections[numpipes][2]=='1'){
					close(2);
					dup(1);
				}
				else{
					close(2);
					if (redType[numpipes][2]==1){
								open(redirections[numpipes][2], O_WRONLY | O_CREAT | O_APPEND  , 0644);
							}
							else{
								open(redirections[numpipes][2],  O_WRONLY | O_CREAT | O_TRUNC, 0644);
							}
				}
			}


			/////////////
			execvp (pipedTokens[numpipes][0], pipedTokens[numpipes]);
			/* NOT REACHED*/
			return 0;
		}
		else{
			int status2 = waitpid(pid, &status,0);
			if ( WIFEXITED(status) ){
      			if(WEXITSTATUS(status)==20){
					  break;
				  }
			}
    
		}
	}

	return(0);
}

