/*----------------------------------------------------------------------------
 * Author		:	Manisha Manohar, Sharan Srivatsa, Chinmay Rudrapatna
 * Name			:	daemon.c
 * Purpose		:	sample code to demonstrate daemon, syslog
 *					and socket communication
 *					starts off and becomes a daemon, opens a socket and
 *					listens on a specified socket
----------------------------------------------------------------------------*/

// Our only include file
#define		__MAIN_SRC_FILE__
#include	"daemon.h"

/*----------------------------------------------------------------------------
 * Function		:	signal_handler()
 * Purpose		:	upon receiving a signal, print mesg for now 
 * Input		:	signal number
 * Returns		:	none
----------------------------------------------------------------------------*/
static void
signal_handler(int signum)
{
	dlog("Signal %d arrived\n", signum);
	return;
}

/*----------------------------------------------------------------------------
 * Function		:	close_socket()
 * Purpose		:	closes a given socket and shrinks socks array
 * Input		:	sock that just closed at peer end
 * Returns		:	-1 on error, 0 on success
----------------------------------------------------------------------------*/
static void
close_socket(int sock)
{
	int i;

	// First close our socket
	(void)close(sock);

	// Find that socket in our socket list
	for (i = 0; i < sock_count; i++)
	{
		if (sock == socks[i])				// Find that socket first
		{
			for (; i < sock_count; i++)
			{
				socks[i] = socks[i+1];		// Pull back all later socks
			}
			sock_count--;					// Decrement sock count
			return;							// We are done
		}
	}
	if (i == sock_count)					// Yikes, we did not find that !!
	{
		derr("Given socket missing from sock list??\n");
	}
	return;
}

/*----------------------------------------------------------------------------
 * Function		:	process_socket()
 * Purpose		:	read in data and respond
 * Input		:	sock socket used for communication
 * Returns		:	-1 on error, 0 on success
----------------------------------------------------------------------------*/
static int
process_socket(int sock)
{
	int		ret;
	char	buf[MAX_BUF_SIZE];

	// Do a socket read, see what is there
	if ((ret = read(sock, buf, MAX_BUF_SIZE -1)) < 0)
	{
		// Did sock read fail ?
		derr("Socket read failed %s\n", strerror(errno));
		close_socket(sock);					// Peer must have closed socket
		return -1;
	}

	else if (ret == 0)
	{
		// Looks like end of file, meaning socket definitely closed at peer
		derr("Peer socket closed\n");
		close_socket(sock);					// Peer must have closed socket
	}
	else
	{
		buf[ret] = 0;
		dlog("Socket read %s\n", buf);
	}

	// TODO: What do we do now? Send response
	return 0;

}
/*----------------------------------------------------------------------------
 * Function		:	accept_socket()
 * Purpose		:	accept an incoming connection
 * Input		:	listen socket
 * Returns		:	-1 on error, 0 on success
----------------------------------------------------------------------------*/
static int
accept_socket(int sock)
{
	int		csock;

	// Got a connect request.
	if ((csock = accept(sock, (struct sockaddr *)NULL, NULL)) == -1)
	{
		// Could not connect??, go back and listen
		derr("accept failed %s\n", strerror(errno));
		return -1;
	}

	dlog("New connection accepted\n");

	if (sock_count < (MAX_SOCKS - 1))
	{
		socks[sock_count] = csock;
		sock_count++;
	}

	// Too many sockets open, Hence though accepted, just close it.
	else			
	{
		close(csock);
	}
	return 0;
}
/*----------------------------------------------------------------------------
 * Function		:	init_socket()
 * Purpose		:	opens a listen socket
 * Input		:	port no
 * Returns		:	-1 on error, 0 on success
----------------------------------------------------------------------------*/
static int
init_socket(int port_no)
{
	struct	sockaddr_in addr; 
	int		sock, reuse = 1;	// previous instances do not cause problems

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		derr("socket open failed %s\n", strerror(errno));
		return -1;
	}

	// So that we can rebind with out TIME_WAIT issues.
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	// Now set the socket to non-blocking
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);

	(void)memset((void *)&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port_no); 

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		derr("socket bind failed %s\n", strerror(errno));
		return -1;
	}
	dlog("Socket init success\n");
	return sock;
}

/*----------------------------------------------------------------------------
 * Function		:	make_daemon()
 * Purpose		:	forks, catches signals, gives up controlling terminal,
 *					changes session leader, opens syslog and returns.
 * Input		:	None
 * Returns		:	-1 on error, 0 on success
----------------------------------------------------------------------------*/
static int
make_daemon()
{
	struct	sigaction	sig_action;

#if	!defined(DEBUG_ON)	
	int		ret;
	pid_t	ch_pid;
#endif

	// Catch ALL signals and handle them.

	sig_action.sa_handler = signal_handler;
	sigemptyset(&sig_action.sa_mask);
	sig_action.sa_flags = 0;

	// Just some sample signals for now. TODO Catch all later
	(void)sigaction(SIGHUP, &sig_action, NULL);
	(void)sigaction(SIGINT, &sig_action, NULL);
	(void)sigaction(SIGTERM, &sig_action, NULL);

#if	!defined(DEBUG_ON)	
	switch(ch_pid = fork())				// Start child process
	{

		case	-1:						// fork failed?
			fprintf(stderr, "Unable to fork <%s>\n", strerror(errno));
			return -1;

		default:						// Parent process, shall exit
			exit(0);

		case	0:						// Child process.
			break;
	}

	// Now we are the child process only.

	// Setting my mask to 0 & change current directort to root.
	// not really needed, not creating any files are we??
	(void)umask(0);
	ret = chdir("/");

	// set a new session (automatically become session leader).
	if (setsid() == -1)
	{
		fprintf(stderr, "Unable to setsid <%s>\n", strerror(errno));
		return -1;
	}

	// Close standard in, out and error file handles
	close(0);
	close(1);
	close(2);

	// Open the log file in syslog facility
	openlog("PES_", LOG_PID, LOG_DAEMON);

	// Phew success atlast
#endif	// DEBUG_ON

	return 0;
}
/*----------------------------------------------------------------------------
 * Function		:	main()
 * Purpose		:	Becomes a daemon. Opens a socket and listens endlessly
 *					When a connection arrives, automatically starts processing
 * Input		:	None
 * Outputs 		:	None
----------------------------------------------------------------------------*/
int
main(int ac, char **av)
{
	int			sock;								// Listening user socket
	sigset_t	new_mask, orig_mask;


	if (ac == 1)
	{
		debug_level = DBG_LOG;						// debug level not specified
	}
	else
	{
		debug_level = atoi(av[1]);					// debug level specified
		// If specified level is bogus, default to logging everything.
		if (debug_level < 0 || debug_level > DBG_CRIT)
			debug_level = DBG_LOG;
	}

	// Make self a daemon
	if (make_daemon() == -1) 						// deamonizing failed? exit
	{
		return 0;
	}

	dlog("Daemon started\n");

	// Init the listen user's socket
	if ((sock = init_socket(USER_PORT)) == -1)		// init_socket failed, exit
	{
		return 0;
	}

	// Add all of our signals to watch list
	sigemptyset(&new_mask);
	sigaddset(&new_mask, SIGTERM);
	sigaddset(&new_mask, SIGHUP);
	sigaddset(&new_mask, SIGINT);
	(void)sigprocmask(SIG_UNBLOCK, &new_mask, &orig_mask);

	// Call the main loop
	dlog("Entering main loop\n");
	if (main_loop(sock, orig_mask) != -1)			// Should never come out
	{
		// Nothing to do
	}

	return 0;
}

/*----------------------------------------------------------------------------
 * Function		:	main_loop()
 * Purpose		:	Runs endlessly, listening servicing users till signalled
 *					to exit.
 * Input		:	listen socket & original signal mask.
 * Outputs 		:	-1 on any errors and 0 on success
----------------------------------------------------------------------------*/
static int
main_loop(int sock, sigset_t mask)
{
	fd_set		rfds;
	struct		timespec	tv;
	int			i, ret;

	dlog("Main Loop\n");

	// Listen to our socket alone
	if (listen(sock, 1) == -1)
	{
		// if it failed, cannot do much go back
		derr("listen failed %s\n", strerror(errno));
		return -1;
	}

	// Do endlessly
	while(1)
	{
		// Start listening/processing etc
		FD_ZERO(&rfds);							// Clear out read FDs
		FD_SET(sock, &rfds);					// Set our socket

		tv.tv_sec  = 5;							// Five second timer.
		tv.tv_nsec = 0;

		// Main listen socket + all of the other sockets
		ret = pselect(MAX_SOCKS + 2, &rfds, NULL, NULL, &tv, &mask);
		if (ret == -1)
		{
			if (errno != EINTR)
			{
				derr("Select failed ? %s\n", strerror(errno));
				return -1;
			}
			dlog("Some signal needs to be handled\n");
			continue;
		}

		// Was that app pinging me with a new connect request?
		if (FD_ISSET(sock, &rfds))
		{
			ret = accept_socket(sock);			// No use of ret value for now
		}

		// Could that be some communication from some app?
		for (i = 0; i < sock_count; i++)
		{
			if (FD_ISSET(socks[i], &rfds))
			{
				ret = process_socket(sock);		// No use of ret value for now
			}
		}

		// Must be time out situation.
		// TODO: What do we do in case of a timeout
		dlog("timed out\n");
	}
	return 0;
}

