#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	FILE *conf = fopen("config","r");
	pid_t pids[20];
	int respawn[20];
	char progs[20][70];
	char str[70];
	char *tok;
	char path[20];
	char num[2];
	char *args[10];
	int i = 0;		//index of argument
	int l = 0;		//index of program
	int c_o_r = 0;		//count of running
	pid_t pid;	
	while (fgets(str, sizeof(str), conf) != NULL) {
		strcpy(progs[l], str);	
		tok = strtok(str, " ");
		while (tok != NULL) {
			args[i++] = tok;
			tok = strtok(NULL, " ");
		}
		if (args[--i][0] == 'r')		
			respawn[l] = 'r';
		args[i] = NULL;
		i = 0;
		pid = fork();
		switch (pid) {
		case -1:
			printf("Error:Fork failed.\n");
			return -1;
		case 0:
			execvp(args[0],args);
			exit(0);
		default:
			pids[l] = pid;
			strcpy(path, "/tmp/");
			strcat(path, args[0]);
			sprintf(num,"%d",l++);
			strcat(path, num);
			strcat(path, ".pid");
			FILE *of = fopen(path,"w");
			fprintf(of, "%d", pid);
			fclose(of);
			//sleep(1);
		}	
	}
	fclose(conf);
	c_o_r = l;
	int j;
	while (c_o_r) {
		pid = wait(NULL);
		for (j=0; j<l; j++) {
			if (pids[j] == pid) {
				strcpy(str, progs[j]);
				tok = strtok(str, " ");
				strcpy(path, "/tmp/");
				strcat(path, tok);
				sprintf(num,"%d",j);
				strcat(path, num);
				strcat(path, ".pid");
				if (respawn[j] == 'r') {
					respawn[j] = 0;
					tok = strtok(progs[j], " ");
					while (tok != NULL) {
						args[i++] = tok;
						tok = strtok(NULL, " ");
					}
					args[--i] = NULL;
					pid = fork();
					switch (pid) {
					case -1:
						printf("Error:Fork failed.\n");
						return -1;
					case 0:
						execvp(args[0],args);
						exit(0);
					default:
						pids[j] = pid;
						FILE *of = fopen(path,"w");
						fprintf(of, "%d", pid);
						fclose(of);
					}
				} else {
					pids[j] = 0;
					remove(path);
					c_o_r--;
				}
			}	
		}
	}
	return 0;
} 
