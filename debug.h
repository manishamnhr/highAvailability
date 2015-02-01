#ifndef	__DEBUG_H__
#define	__DEBUG_H__
/*----------------------------------------------------------------------------
 * Author		:	Manisha Manohar
 * Name			:	debug.h
 * Purpose		:	header file debug/syslog facility
----------------------------------------------------------------------------*/

// If Debug is turned on, we will run in foreground and not become a daemon
// and all logging goes to terminal
// If debug is turned off, we wil be a daemon with no teminal support
// and all logging goes to syslog

#define		DBG_LOG			0
#define		DBG_INFO		1
#define		DBG_DEBUG		2
#define		DBG_ERROR		3
#define		DBG_CRIT		4

#if defined(DEBUG_ON)

 #define	dlog(...)	do {	if (debug_level <= DBG_LOG)	{		\
 									fprintf(stdout, __VA_ARGS__);	\
								}									\
						} while(0)


 #define	derr(...)	do {	if (debug_level <= DBG_ERROR)	{	\
 									fprintf(stdout, __VA_ARGS__);	\
								}									\
						} while(0)

 #define	dbg(...)	do {	if (debug_level <= DBG_DEBUG)	{	\
 									fprintf(stdout, __VA_ARGS__);	\
								}									\
						} while(0)

#else

 #define	dlog(...)	do {	if (debug_level <= DBG_LOG)	{		\
 									syslog(LOG_NOTICE, __VA_ARGS__)	\
								}									\
						} while(0)


 #define	derr(...)	do {	if (debug_level <= DBG_ERROR)	{	\
 									syslog(LOG_ERR, __VA_ARGS__)	\
								}									\
						} while(0)

 #define	dbg(...)	do {	if (debug_level <= DBG_DEBUG)	{	\
 									syslog(LOG_INFO, __VA_ARGS__)	\
								}									\
						} while(0)

#endif

#endif	//	__DEBUG_H__


