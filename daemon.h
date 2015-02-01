#ifndef	__DAEMON_H__
#define	__DAEMON_H__
/*----------------------------------------------------------------------------
 * Author		:	Manisha Manohar
 * Name			:	daemon.h
 * Purpose		:	header file for our daemin application under UNIX
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <syslog.h>
#include "debug.h"


// All defines here
#define	USER_PORT		6264		//	Some socket port number
#define	MAX_SOCKS		16			//	Why complicate with too many??
#define	MAX_BUF_SIZE	1024		//	Max communication data size

// All globals here
#if defined(__MAIN_SRC_FILE__)

 int		sock_count = 0;			// Number of open sockets.
 int		socks[MAX_SOCKS];		// All open socks here
 int		debug_level = DBG_LOG;	// debug level controller

#else

 extern	int		sock_count;			// Number of open sockets.
 extern	int		socks[];			// All open socks here
 extern int		debug_level;		// debug level controller

#endif

// All of our prototypes declared here.

static int		make_daemon(void);
static void		signal_handler(int signum);
static int		main_loop(int sock, sigset_t mask);
static int		init_socket(int port_no);
static int		accept_socket(int sock);
static void		close_socket(int sock);
static int		process_socket(int sock);

#endif	//	__DAEMON_H__

