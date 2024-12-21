
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <unistd.h>
#include <bsd/unistd.h>
#include <string.h>
#include <pthread.h>


// we are gonna maintaining an array of fd <-> sockaddr mapping
// to able to answer getpeername() calls with the expected socket address.
struct sockfd_peername {
	int fd;
	struct sockaddr_in sockaddr;
	socklen_t addrlen;
};

typedef struct sockfd_peername * sockfd_peername_ptr;


static sockfd_peername_ptr captured_sockets = NULL;
static unsigned int captured_sockets_len = 0;
pthread_mutex_t captured_sockets_lock;


sockfd_peername_ptr _get_captured_socket(int fd)
{
	// find the sockfd_peername structure which hold the sockaddr for fd.
	// even if it's no longer valid.
	for(unsigned int idx = 0; idx < captured_sockets_len; idx++)
	{
		if(captured_sockets[idx].fd == fd) return &captured_sockets[idx];
	}
	return NULL;
}

void _note_sockfd_peername(int fd, const struct sockaddr_in * sockaddr, socklen_t addrlen)
{
	// record the sockaddr for the given fd.
	pthread_mutex_lock(&captured_sockets_lock);
	sockfd_peername_ptr sockinfo = _get_captured_socket(fd);
	if(sockinfo == NULL)
	{
		// fd is not recorded so far.
		if(captured_sockets == NULL)
		{
			// captured_sockets is not allocated yet at all.
			captured_sockets = malloc(sizeof(struct sockfd_peername));
			if(captured_sockets == NULL) abort();
			captured_sockets_len = 1;
			sockinfo = captured_sockets;
		}
		else
		{
			// allocate one more slot.
			captured_sockets = realloc(captured_sockets, sizeof(struct sockfd_peername) * (captured_sockets_len+1));
			if(captured_sockets == NULL) abort();
			captured_sockets_len += 1;
			sockinfo = &captured_sockets[captured_sockets_len-1];
		}
	}
	// copy fd number, socket address, and address length for later recall.
	// write to a freshly allocated sockfd_peername structure or an old one, overwriting old data in that case.
	// captured_sockets is a grow-only array. we don't free up space because it's mainly for programs making a few intercepted connect() calls in their lifetime.
	sockinfo->fd = fd;
	memcpy(&sockinfo->sockaddr, sockaddr, addrlen);
	sockinfo->addrlen = addrlen;
	pthread_mutex_unlock(&captured_sockets_lock);
}

void _autossl_ip_parse_error(const char* s, const size_t len)
{
	fprintf(stderr, "autossl: failed to parse ip address '%.*s'\n", len, s);
}

int connect(int sockfd, const struct sockaddr_in *orig_sockaddr, socklen_t addrlen)
{
	char *upgrade_ip_str;
	struct in_addr upgrade_ip;
	char *upgrade_port_str;
	in_port_t upgrade_port = 0;
	
	char *tls_cmd;
	#define IP_STR_LEN 39+1
	#define PORT_STR_LEN 5+1
	char connect_ip[IP_STR_LEN];
	char connect_port[PORT_STR_LEN];
	
	char *next_separator;
	char *autossl_errno_str;
	
	struct sockaddr_in to_sockaddr;
	int (*orig_connect)(int, const struct sockaddr_in *, socklen_t) = dlsym(RTLD_NEXT, "connect");
	
	
	
	// TODO suport ipv6
	if(orig_sockaddr->sin_family != AF_INET) goto stdlib;
	
	upgrade_port_str = getenv("AUTOSSL_UPGRADE_PORTS");
	if(upgrade_port_str == NULL) goto stdlib;
	
	
	// determine if need to intercept this connect() according to AUTOSSL_UPGRADE_PORTS
	int upgrade_port_matched = 0;
	next_separator = NULL;
	
	do{
		next_separator = strchrnul(upgrade_port_str, ' ');
		upgrade_port = atoi(upgrade_port_str);
		if(upgrade_port == 0) { fprintf(stderr, "autossl: failed to parse port number(s): %s\n", upgrade_port_str); goto error_case; }
		if(upgrade_port == ntohs(orig_sockaddr->sin_port)) upgrade_port_matched = 1;
		upgrade_port_str = (char*)(next_separator + 1);
	}
	while(!upgrade_port_matched && *next_separator != '\0');
	
	if(!upgrade_port_matched) goto stdlib;
	
	// determine if need to intercept this connect() according to AUTOSSL_UPGRADE_IPS
	upgrade_ip_str = getenv("AUTOSSL_UPGRADE_IPS");
	if(upgrade_ip_str != NULL)
	{
		int upgrade_ip_matched = 0;
		char *next_separator = NULL;
		
		do{
			next_separator = strchrnul(upgrade_ip_str, ' ');
			if(inet_aton(upgrade_ip_str, &upgrade_ip) == 0)
			{
				_autossl_ip_parse_error(upgrade_ip_str, next_separator - upgrade_ip_str);
				goto error_case;
			}
			if(ntohl(orig_sockaddr->sin_addr.s_addr) == ntohl(upgrade_ip.s_addr)) {
				upgrade_ip_matched = 1;
			}
			upgrade_ip_str = (char*)(next_separator + 1);
		}
		while(!upgrade_ip_matched && *next_separator != '\0');
		
		if(!upgrade_ip_matched) goto stdlib;
	}
	
	tls_cmd = getenv("AUTOSSL_TLS_CMD");
	if(tls_cmd == NULL) goto stdlib;
	
	
	int sockpair[2];
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair) == -1)
	{
		perror("autossl: sockpair");
		goto error_case;
	}
	
	pid_t childpid = fork();
	if(childpid < 0)
	{
		perror("autossl: fork");
		goto error_case;
	}
	if(childpid == 0)
	{
		/* save the ip and port we wanted to connect to as strings */
		snprintf(connect_ip, IP_STR_LEN, "%s", inet_ntoa(orig_sockaddr->sin_addr));
		snprintf(connect_port, PORT_STR_LEN, "%d", ntohs(orig_sockaddr->sin_port));
		/* wire STDIO to the newly created socket */
		dup2(sockpair[1], 0);
		dup2(sockpair[1], 1);
		/* leave stderr open */
		/* close dangling files */
		closefrom(3);
		execlp(tls_cmd, tls_cmd, connect_ip, connect_port, NULL);
		_exit(127);
	}
	
	close(sockpair[1]);
	if(dup2(sockpair[0], sockfd) == -1)
	{
		perror("autossl: dup2");
		close(sockpair[0]);
		goto error_case;
	}
	
	if(!getenv("AUTOSSL_SILENT"))
		fprintf(stderr, "autossl: redirecting %s:%d -> fd#%d\n", inet_ntoa(orig_sockaddr->sin_addr), ntohs(orig_sockaddr->sin_port), sockpair[0]);
	
	_note_sockfd_peername(sockfd, orig_sockaddr, addrlen);
	
	/* the caller closes sockfd only, not sockpair[0], so unused open
	files may pile up eventually in long running programs. */
	
	/* childpid process won't be reaped, so don't be scared on the
	zombie processes, they will be disappear as the main program exits. */
	
	return 0;
	
	error_case:
	autossl_errno_str = getenv("AUTOSSL_ERRNO");
	if(autossl_errno_str)
	{
		errno = atoi(autossl_errno_str);
		return -1;
	}
	
	stdlib:
	return orig_connect(sockfd, orig_sockaddr, addrlen);
}

int shutdown(int sockfd, int how)
{
	int (*orig_shutdown)(int, int) = dlsym(RTLD_NEXT, "shutdown");
	
	// shutdown() is intercepted only to mark which fd <-> sockaddr mapping is not valid anymore.
	
	pthread_mutex_lock(&captured_sockets_lock);
	int err = orig_shutdown(sockfd, how);
	if(err == 0)
	{
		// shutdown() succeeded on the fd, so invalidate its slot in captured_sockets array - if it's there.
		sockfd_peername_ptr sockinfo = _get_captured_socket(sockfd);
		if(sockinfo != NULL)
		{
			sockinfo->addrlen = 0;  // mark this slot not being used anymore
		}
	}
	pthread_mutex_unlock(&captured_sockets_lock);
	return err;
}

int getpeername(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen)
{
	int (*orig_getpeername)(int, struct sockaddr *, socklen_t *) = dlsym(RTLD_NEXT, "getpeername");
	
	// check if this fd is in captured_sockets array,
	// if so, then answer with the recorded socket address.
	
	pthread_mutex_lock(&captured_sockets_lock);
	sockfd_peername_ptr sockinfo = _get_captured_socket(sockfd);
	if(sockinfo != NULL && sockinfo->addrlen != 0)
	{
		memcpy(addr, &sockinfo->sockaddr, sockinfo->addrlen);
		*addrlen = sockinfo->addrlen;
		pthread_mutex_unlock(&captured_sockets_lock);
		return 0;
	}
	pthread_mutex_unlock(&captured_sockets_lock);
	
	return orig_getpeername(sockfd, addr, addrlen);
}
