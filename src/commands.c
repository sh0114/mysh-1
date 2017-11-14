#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <pthread.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include "commands.h"
#include "built_in.h"

static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv }
};

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }

  return -1; // Not found
}

/*
 * Description: Currently this function only handles single built_in commands. You should modify this structure to launch process and offer pipeline functionality.
 */
int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  if (n_commands > 0) {
    struct single_command* com = (*commands);

	char path [512] = "";
	int pid = fork();

    assert(com->argc != 0);

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
      return 1;
    } else if (strcmp(com->argv[0],"/bin/ls") == 0 || ((strcmp(com->argv[0],"/bin/cat")== 0) && com->argc == 2) || strcmp(com->argv[0],"/usr/bin/vim") == 0)
    {
        if(pid == 0)
        {
                execv(com->argv[0], com->argv);
        }
        else
        {
                wait((int*) 0);
        }

    }
   else if(strcmp(com->argv[0],"ls") == 0 || strcmp(com->argv[0],"cat")==0)
   {
        memset(path,0x00,sizeof(path));
        if(pid == 0)
        {
                strcat(path,"/bin/");
                strcat(path,com->argv[0]);
//              printf("%s\n", path);
                com->argv[0] = path;
                execv(path,com->argv);
        }
        else
        {
                wait((int*) 0);
        }
//      memset(path,0x00,sizeof(path));
   }else if(strcmp(com->argv[0], "vim") == 0){
        memset(path,0x00,sizeof(path));
        if(pid == 0)
        {
                strcat(path,"/usr/bin/vim");
                com->argv[0] = path;
                execv(path,com->argv);
        }
        else
        {
                wait((int*) 0);
        }
//      memset(path,0x00,sizeof(path));
   }
/*   else if(strcmp(com->argv[0],"/bin/cat") == 0 && com->argc > 2 )
   {
        int fd1, fd2, fd3;
        if (pid == 0) // server?
        {
                int pid_0, status;
                pthread_t s_thrd;
                
                fd1 = open("./mysh", O_CREAT | O_WRONLY | 0644);
                fd2 = dup(fd1);

                int server_sock, client_sock, size, msg;
                
                char buf[512];
                char data[512];
                char temp[50];

                struct sockaddr_in server_adr;
                struct sockaddr_in client_adr;

                memset(&server_adr,0x00,sizeof(server_adr));
                memset(&client_adr,0x00,sizeof(client_adr));
                memset(buf,0x00, sizeof(buf));
                memset(data,0x00, sizeof(data));

                server_sock = socket(AF_INET, SOCK_STREAM, 0);
                server_adr.sin_family = AF_INET;
                server_adr.sin_addr.s_addr = htonl(INADDR_ANY);
                server_adr.sin_port = htons(atoi(com->argv[1]));
                
                size = sizeof(client_adr);

                bind(server_sock, (struct sockaddr *)&server_adr, size);
                listen(server_sock, 5);
                

                while(1)
                {
                        dup2(fd2,STDOUT_FILENO);
                        client_sock = accept(server_sock, (struct sockaddr *) &client_adr, &size);
                
                        inet_ntop(AF_INET, &client_adr.sin_addr.s_addr,temp, sizeof(temp));
                        send(client_sock,com->argv[1], 0xff,0);
                        msg = recv(client_sock, (char *)buf, 512 , 0);
                        send(client_sock, (char *)buf, msg, 0);

                        close(client_sock);
                }

                close(server_sock);

        }
//      else if(pid2 == 0) // client?
        //{
                pthread_t c_thrd;
        
                int client_sock, size;
                struct sockaddr_in client_adr;
                char data[512];

                client_sock = socket(PF_INET, SOCK_STREAM, 0);
                
                client_adr.sin_addr.s_addr = inet_addr(IPADDR);
                client_adr.sin_family = AF_INET;
                client_adr.sin_port = htons(PORT);  
               connect(client_sock, (struct sockaddr *)&client_adr, sizeof(client_adr));

                /////////////////////

                recv(client_sock, data, sizeof(data), 0);
                close(client_sock);
                char* str2[512];

                str2[0] = com->argv[2];
                str2[1] = com->argv[3];
                str2[2] = data;
                
                for(int i=0; i<strlen(data); i++)
                {
                        printf("%c", data[i]);
                }


                printf("%s\n", data);
                execv(str2[0],str2);
        }
        else
        {
                wait((int *) 0);
        }
   }
*/
                                                              
 else {
      fprintf(stderr, "%s: command not found\n", com->argv[0]);
      return -1;
    }
  }

  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      free(argv[j]);
    }

    free(argv);
  }

  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}
