//////////////////////////////////////////////////////////////////////
//                              Main.c                              //
//////////////////////////////////////////////////////////////////////
//           Auteur: Adnan RIHAN (Max13) <adnan@rihan.fr>           //
//////////////////////////////////////////////////////////////////////

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int	*gsock;

void	exit_prog(int sig)
{
	puts("Quitting...");
        close(*gsock);
	exit(sig);
}

int	main(int ac, char **av)
{
	if (ac < 2)
	{
		puts("Second argument missing (port)");
		return (-1);
	}

	char	*tmp;
	char	msg[34];
	fd_set	fd;
	int	sock;
	int	csock;
	int	sel;
	struct sigaction	act;
	struct sockaddr_in	sin;
	struct sockaddr_in csin;
	socklen_t	sinsize = sizeof(&csin);
	u_short	port = atoi(av[1]);

	gsock = &sock;

	act.sa_handler = exit_prog;
	if (sigaction(SIGINT, &act, NULL) == -1)
	{
                puts(strerror(errno));
                return (errno);
        }

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		puts(strerror(errno));
		return (errno);
	}

	sin.sin_addr.s_addr	= htonl(INADDR_ANY);
	sin.sin_family		= AF_INET;
 	sin.sin_port		= htons(port);

	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		puts(strerror(errno));
		return (errno);
	}

	if (listen(sock, 10) == -1)
	{
		puts(strerror(errno));
		return (errno);
	}

	do {
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
		sel = select(sock+1, &fd, NULL, NULL, NULL);
		if (sel != -1)
		{
			if (FD_ISSET(sock, &fd))
			{
				if ((csock = accept(sock, (struct sockaddr *)&csin, &sinsize)) != -1)
				{
					tmp = inet_ntoa(csin.sin_addr);
					if (tmp != NULL)
					{
						strcpy(msg, "Here is your IP: ");
						strcat(msg, tmp);
						strcat(msg, "\n");
						send(csock, "\n", 2, 0);
						send(csock, msg, strlen(msg)+1, 0);
						send(csock, "\n", 2, 0);
					}
					close(csock);
				}
			}
		}
	} while (1);

	exit_prog(0);
	return (0);
}
