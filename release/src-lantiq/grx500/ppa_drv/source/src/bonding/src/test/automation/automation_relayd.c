#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

//#define ABORT(format, ...) fprintf( stderr, "%s(%d): " format "\n", ##__VA_ARGS__ , __FILE__, __LINE__ ); exit(-1);
#define ABORT(format, ...) fprintf( stderr, "%s(%d): " format "\n", __FILE__, __LINE__, ##__VA_ARGS__ ); exit(-1);
#define ABORTERRNO( format, ...) ABORT( format ": %s", ##__VA_ARGS__ , strerror(errno));
#define BUFFLEN 1024
#define CONTEXT (&gContext)
#define CLOSE( fd ) close((fd)); (fd) = -1; SETMAXFD();
#define EMPTYMASTERBUFF() CONTEXT->master_buff_len = 0;
#define EMPTYSTUBBUFF() CONTEXT->stub_buff_len = 0;
#define LOG(format, ...) fprintf (stdout, format, ##__VA_ARGS__)
#define MASTER_PORT 48090
#define OPENMASTERSOCKET() OPENSOCKET( "master", (MASTER_PORT), &CONTEXT->masterlistenfd );
#define OPENSTUBSOCKET() OPENSOCKET( "stub", (STUB_PORT), &CONTEXT->stublistenfd );
#define MASTER_BUFF_IS_EMPTY()  CONTEXT->master_buff_len == 0
#define STUB_BUFF_IS_EMPTY()  CONTEXT->stub_buff_len == 0
#define STUB_PORT (MASTER_PORT+1)
#define HEARTBEAT_PORT MASTER_PORT

typedef enum  {
	NONE= 0,
	MASTER_ONLY,
	STUB_ONLY,
	BOTH
} State_t;

typedef enum {
	MasterOpenedConnection = 1,
	StubOpenedConnection,
	DataFromMaster,
	DataFromStub,
	ReadyToWriteToMaster,
	ReadyToWriteToStub,
	MasterClosedConnection,
	StubClosedConnection
} Event_t;

struct thread_arg{
	
	State_t state;
	
	int masterlistenfd,
		stublistenfd,
		masterconnfd,
		stubconnfd,
		maxfd;
		
	char master_buff[BUFFLEN],
		 stub_buff[BUFFLEN];
		 
	int master_buff_len,
		stub_buff_len;

	struct sockaddr_in master,
				stub,
				broadcast;

	pthread_t heartbeat;
	int heartbeatfd;

} gContext;

static inline void
SETMAXFD(void)
{
	if ( CONTEXT->maxfd < CONTEXT->masterconnfd ) CONTEXT->maxfd = CONTEXT->masterconnfd;
	if ( CONTEXT->maxfd < CONTEXT->stubconnfd ) CONTEXT->maxfd = CONTEXT->stubconnfd;
	if ( CONTEXT->maxfd < CONTEXT->masterlistenfd ) CONTEXT->maxfd = CONTEXT->masterlistenfd;
	if ( CONTEXT->maxfd < CONTEXT->stublistenfd ) CONTEXT->maxfd = CONTEXT->stublistenfd;
}

static inline char *
state2str(State_t state)
{
	static char msg[32];

	switch(state) {
		case NONE: return "NONE";
		case MASTER_ONLY: return "MASTER-ONLY";
		case STUB_ONLY: return "STUB-ONLY";
		case BOTH: return "BOTH";
		default: snprintf( msg, sizeof(msg), "Unknown State: %d", state); return msg;
	}
}

static inline char *
event2str(Event_t event)
{
	static char msg[32];

	switch(event) {
		case MasterOpenedConnection: return "MasterOpenedConnection";
		case StubOpenedConnection: return "StubOpenedConnection";
		case DataFromMaster: return "DataFromMaster";
		case DataFromStub: return "DataFromStub";
		case ReadyToWriteToMaster: return "ReadyToWriteToMaster";
		case ReadyToWriteToStub: return "ReadyToWriteToStub";
		case MasterClosedConnection: return "MasterClosedConnection";
		case StubClosedConnection: return "StubClosedConnection";
		default: snprintf( msg, sizeof(msg), "Unknown Event: %d", event); return msg;
	}
}

static inline void
ACCEPTMASTERCONN(void)
{
	static char addr[INET_ADDRSTRLEN ];
	socklen_t len = sizeof(CONTEXT->master);
	
	CONTEXT->masterconnfd = accept(CONTEXT->masterlistenfd, 
								   (struct sockaddr *)&CONTEXT->master,
								   &len);
 	if (CONTEXT->masterconnfd < 0) 
 	{
		ABORTERRNO( "acccept from master failed");
 		exit(-1);
 	}
	SETMAXFD();
	LOG( "Accepted connection request from master: %s\n", 
			inet_ntop(AF_INET, (void *)&(CONTEXT->master.sin_addr), addr, INET_ADDRSTRLEN ));
}

static inline void
ACCEPTSTUBCONN(void)
{
	static char addr[INET_ADDRSTRLEN ];
	socklen_t len = sizeof(CONTEXT->stub);
	
	CONTEXT->stubconnfd = accept(CONTEXT->stublistenfd, 
								   (struct sockaddr *)&CONTEXT->stub,
								   &len);
 	if (CONTEXT->stubconnfd < 0) 
 	{
		ABORTERRNO( "acccept from stub failed");
 		exit(-1);
 	}
	SETMAXFD();
	LOG( "Accepted connection request from stub: %s\n", 
			inet_ntop(AF_INET, (void *)&(CONTEXT->stub.sin_addr), addr, INET_ADDRSTRLEN ));
}

static inline void
OPENSOCKET( char *label, short port, int *fd)
{
	int opt;
	struct sockaddr_in local_addr;
	
	*fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*fd < 0)
	{
		ABORTERRNO( "failed to open %s listening socket", label );
	}
	
	opt = 1;

	if (setsockopt(*fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		LOG( "%s(%d): setsockopt-SO_REUSEADDR on socket %d failed: %s\n", 
				__FILE__, __LINE__, *fd, strerror(errno));
	}

	if (setsockopt(*fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		LOG( "%s(%d): setsockopt-SO_REUSEPORT on socket %d failed: %s\n", 
				__FILE__, __LINE__, *fd, strerror(errno));
	}

	bzero(&local_addr, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons((short)port);
	if (bind(*fd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) 
	{
		ABORTERRNO( "bind failed to %s listening socket", label );
	}

	if (listen(*fd, 2) < 0) 
	{
		ABORTERRNO( "listen failed on %s listening socket", label );
	}

	SETMAXFD();

	LOG( "Listening on %s port number %hu\n", label, port );		
}

#define REJECTMASTERSTR "0\n2\nMASTER_ALREADY_CONNECTED\nMaster already connected\n"
static inline void
REJECTMASTERCONN(void)
{
	int connfd;
	struct sockaddr_in remote_addr;
	static char addrstr[INET_ADDRSTRLEN ];
	socklen_t len = sizeof(remote_addr);
	
	connfd = accept(CONTEXT->masterlistenfd, 
								   (struct sockaddr *)&remote_addr,
								   &len);
 	if (connfd < 0) 
 	{
		ABORTERRNO( "acccept from master failed");
 		exit(-1);
 	}
	
	write(connfd, REJECTMASTERSTR, strlen(REJECTMASTERSTR) );
	close(connfd);
	
	LOG( "Rejected connection request from master: %s", 
			inet_ntop(AF_INET, (void *)&(remote_addr.sin_addr), addrstr, INET_ADDRSTRLEN ));
}

#define REJECTSTUBSTR "0\n2\nSTUB_ALREADY_CONNECTED\nStub already connected\n"
static inline void
REJECTSTUBCONN(void)
{
	int connfd;
	struct sockaddr_in remote_addr;
	static char addrstr[INET_ADDRSTRLEN ];
	socklen_t len = sizeof(remote_addr);
	
	connfd = accept(CONTEXT->stublistenfd, 
								   (struct sockaddr *)&remote_addr,
								   &len);
 	if (connfd < 0) 
 	{
		ABORTERRNO( "acccept from stub failed");
 		exit(-1);
 	}
	
	write(connfd, REJECTSTUBSTR, strlen(REJECTSTUBSTR) );
	close(connfd);
	
	LOG( "Rejected connection request from stub: %s", 
			inet_ntop(AF_INET, (void *)&(remote_addr.sin_addr), addrstr, INET_ADDRSTRLEN ));
}

#define STUBNOTCONNECTEDSTR "0\n2\nSTUB_NOT_CONNECTED\nStub not connected\n"
static inline void
NOTIFY_STUB_NOT_CONNECTED(void)
{
	snprintf( CONTEXT->stub_buff + CONTEXT->stub_buff_len, BUFFLEN - CONTEXT->stub_buff_len,
			  STUBNOTCONNECTEDSTR);
	CONTEXT->stub_buff_len += strlen( CONTEXT->stub_buff );
}

#define MASTERDISCONNECTEDSTR "0\n2\nMASTER_DISCONNECTED\nMaster has disconnected\n"
static inline void
NOTIFY_MASTER_DISCONNECTED(void)
{
	snprintf( CONTEXT->master_buff + CONTEXT->master_buff_len, BUFFLEN - CONTEXT->master_buff_len,
			  MASTERDISCONNECTEDSTR );
	CONTEXT->master_buff_len += strlen( CONTEXT->master_buff );
}

static inline int
READ_FROM_MASTER(void)
{
	int bytes_to_read;
	ssize_t bytes_read;
		
	if (CONTEXT->master_buff_len == BUFFLEN) return 0;
	
	if (ioctl(CONTEXT->masterconnfd, FIONREAD, &bytes_to_read) < 0) {
		ABORTERRNO( "ioctl(FIONREAD) on master connection failed" );
	}
	
	if( bytes_to_read > (BUFFLEN - CONTEXT->master_buff_len)) {
		bytes_to_read = BUFFLEN - CONTEXT->master_buff_len;
	}
	
	bytes_read = read( CONTEXT->masterconnfd,
					   CONTEXT->master_buff + CONTEXT->master_buff_len,
					   bytes_to_read );

printf( "read %d bytes from master in state: %s\n", bytes_read, state2str(CONTEXT->state) ); fflush(stdout);
	
	if (bytes_read == 0) {
		/* Master has closed connection */
		return -1;
	
	} else if (bytes_read < 0) {
		ABORTERRNO( "read from master connection failed" );
	}
	
	CONTEXT->master_buff_len += bytes_read;
	
	return bytes_read;
}

static inline int
READ_FROM_STUB(void)
{
	int bytes_to_read;
	ssize_t bytes_read;
		
	if (CONTEXT->stub_buff_len == BUFFLEN) return 0;
	
	if (ioctl(CONTEXT->stubconnfd, FIONREAD, &bytes_to_read) < 0) {
		ABORTERRNO( "ioctl(FIONREAD) on stub connection failed" );
	}
	
	if( bytes_to_read > (BUFFLEN - CONTEXT->stub_buff_len)) {
		bytes_to_read = BUFFLEN - CONTEXT->stub_buff_len;
	}
	
	bytes_read = read( CONTEXT->stubconnfd,
					   CONTEXT->stub_buff + CONTEXT->stub_buff_len,
					   bytes_to_read );
	
	if (bytes_read == 0) {
		/* Stub has closed connection */
		return -1;
	
	} else if (bytes_read < 0) {
		ABORTERRNO( "read from stub connection failed" );
	}
	
	CONTEXT->stub_buff_len += bytes_read;
	
	return bytes_read;
}

static inline int
WRITE_TO_MASTER(void)
{
	ssize_t bytes_sent;
		
	if (CONTEXT->stub_buff_len == 0) return 0;
	
	bytes_sent = write( CONTEXT->masterconnfd,
					   CONTEXT->stub_buff, CONTEXT->stub_buff_len);
	
	if (bytes_sent == EPIPE) {
		/* Master has closed connection */
		return -1;
	
	} else if (bytes_sent < 0) {
		ABORTERRNO( "write to master connection failed" );
	}
	
	memmove( CONTEXT->stub_buff, CONTEXT->stub_buff + bytes_sent,
			 CONTEXT->stub_buff_len - bytes_sent );
	CONTEXT->stub_buff_len -= bytes_sent;
	
	return bytes_sent;
}

static inline int
WRITE_TO_STUB(void)
{
	ssize_t bytes_sent;
		
	if (CONTEXT->master_buff_len == 0) return 0;
	
	bytes_sent = write( CONTEXT->stubconnfd,
					   CONTEXT->master_buff, CONTEXT->master_buff_len);
	
	if (bytes_sent == EPIPE) {
		/* Stub has closed connection */
		return -1;
	
	} else if (bytes_sent < 0) {
		ABORTERRNO( "write to stub connection failed" );
	}
	
	memmove( CONTEXT->master_buff, CONTEXT->master_buff + bytes_sent,
			 CONTEXT->master_buff_len - bytes_sent );
	CONTEXT->master_buff_len -= bytes_sent;
	
	return bytes_sent;
}

void
fsm( Event_t event )
{
	char msg[INET_ADDRSTRLEN];
printf( "FSM ENTRY: State=%s, Event=%s\n", state2str(CONTEXT->state), event2str(event) ); fflush(stdout);
	
	switch (CONTEXT->state) {
		
		case NONE:
			
			switch (event){
				case MasterOpenedConnection:
					ACCEPTMASTERCONN();
					EMPTYMASTERBUFF();
printf( "Changing state to MASTER-ONLY\n" ); fflush(stdout);
					CONTEXT->state = MASTER_ONLY;
					break;
					
				case StubOpenedConnection:
					ACCEPTSTUBCONN();
					EMPTYSTUBBUFF();
					CONTEXT->state = STUB_ONLY;
					break;

				case DataFromMaster:
				case ReadyToWriteToMaster:
				case MasterClosedConnection:
					LOG( "This event %s in state %s is not possible",
							event2str(event), state2str(CONTEXT->state) );
					CLOSE( CONTEXT->masterconnfd );
					EMPTYMASTERBUFF();
					CLOSE( CONTEXT->masterlistenfd );
					OPENMASTERSOCKET();
					break;

				case DataFromStub: break;
				case ReadyToWriteToStub: break;
				case StubClosedConnection: break;
					LOG( "This event %s in state %s is not possible",
							event2str(event), state2str(CONTEXT->state) );
					CLOSE( CONTEXT->stubconnfd );
					EMPTYSTUBBUFF();
					CLOSE( CONTEXT->stublistenfd );
					OPENSTUBSOCKET();
					break;

				default: 
					LOG( "Unknown event %d in state %s",
							event, state2str(CONTEXT->state) );
					break;
			}
			break;
				
		case MASTER_ONLY:
			switch (event){
				case MasterOpenedConnection:
					REJECTMASTERCONN();
					break;
				
				case StubOpenedConnection:
					ACCEPTSTUBCONN();
					EMPTYSTUBBUFF();
					EMPTYMASTERBUFF();
					CONTEXT->state = BOTH;
					break;
				
				case DataFromMaster:
					if ( STUB_BUFF_IS_EMPTY() ) {
						NOTIFY_STUB_NOT_CONNECTED();
					}
					if ( READ_FROM_MASTER() < 0 ) {
						fsm( MasterClosedConnection );
					} else {
printf( "Data from master is %d bytes long\n", CONTEXT->master_buff_len ); fflush(stdout);
						EMPTYMASTERBUFF();
					}
					break;

				case ReadyToWriteToMaster:
					if ( WRITE_TO_MASTER() < 0 ) {
						fsm( MasterClosedConnection );
					}
					break;

				case MasterClosedConnection:
					LOG( "Closed connection with master: %s", 
							inet_ntop(AF_INET, (void *)&(CONTEXT->master.sin_addr), msg, INET_ADDRSTRLEN ));
					CLOSE( CONTEXT->masterconnfd );
					EMPTYMASTERBUFF();
					EMPTYSTUBBUFF();
					CONTEXT->state = NONE;
					break;
					
				case DataFromStub:
				case ReadyToWriteToStub:
				case StubClosedConnection:
					LOG( "This event %s in state %s is not possible",
							event2str(event), state2str(CONTEXT->state) );
					CLOSE( CONTEXT->stubconnfd );
					EMPTYSTUBBUFF();
					EMPTYMASTERBUFF();
					CLOSE( CONTEXT->stublistenfd );
					OPENSTUBSOCKET();
					break;
					
				default:
					LOG( "Unknown event %d in state %s",
							event, state2str(CONTEXT->state) );
					break;
			}
			break;
				
		case STUB_ONLY:
			switch (event){
				case MasterOpenedConnection:
					ACCEPTMASTERCONN();
					EMPTYMASTERBUFF();
					CONTEXT->state = BOTH;
					break;

				case StubOpenedConnection:
					REJECTSTUBCONN();
					break;
				
				case DataFromStub:
					if ( MASTER_BUFF_IS_EMPTY() ) {
						NOTIFY_MASTER_DISCONNECTED();
					}
					if ( READ_FROM_STUB() < 0 ) {
						fsm( StubClosedConnection );
					} else {
						EMPTYSTUBBUFF();
					}
					break;
					
				case ReadyToWriteToStub:
					if ( WRITE_TO_STUB() < 0 ) {
						fsm( StubClosedConnection );
					} else if (MASTER_BUFF_IS_EMPTY()) {
						// Desired Message sent to stub. Time to fake StubClosedConnection event:
						fsm( StubClosedConnection );
					}						
					break;
				
				case StubClosedConnection:
					LOG( "Closed connection with stub: %s", 
							inet_ntop(AF_INET, (void *)&(CONTEXT->stub.sin_addr), msg, INET_ADDRSTRLEN ));
					CLOSE( CONTEXT->stubconnfd );
					EMPTYSTUBBUFF();
					EMPTYMASTERBUFF();
					CONTEXT->state = NONE;
					break;
				
				case DataFromMaster:
				case ReadyToWriteToMaster:
				case MasterClosedConnection:
					LOG( "This event %s in state %s is not possible",
							event2str(event), state2str(CONTEXT->state) );
					CLOSE( CONTEXT->masterconnfd );
					EMPTYMASTERBUFF();
					CLOSE( CONTEXT->masterlistenfd );
					OPENMASTERSOCKET();
					break;
				
				default:
					LOG( "Unknown event %d in state %s",
							event, state2str(CONTEXT->state) );
					break;
			}
			break;
				
		case BOTH:
			switch (event){
				case MasterOpenedConnection:
					REJECTMASTERCONN();
					break;
					
				case StubOpenedConnection:
					REJECTSTUBCONN();
					break;
					
				case DataFromMaster:
					if ( READ_FROM_MASTER() < 0 ) {
						fsm( MasterClosedConnection );
					}
					break;
				
				case DataFromStub:
					if ( READ_FROM_STUB() < 0 ) {
						fsm( StubClosedConnection );
					}
					break;
				
				case ReadyToWriteToMaster:
					if ( WRITE_TO_MASTER() < 0 ) {
						fsm( MasterClosedConnection );
					}
					break;
				
				case ReadyToWriteToStub:
					if ( WRITE_TO_STUB() < 0 ) {
						fsm( StubClosedConnection );
					}
					break;
				
				case MasterClosedConnection:
					CLOSE( CONTEXT->masterconnfd);
					EMPTYMASTERBUFF();
					NOTIFY_MASTER_DISCONNECTED();
					CONTEXT->state = STUB_ONLY;
					break;
				
				case StubClosedConnection:
					CLOSE( CONTEXT->stubconnfd);
					EMPTYSTUBBUFF();
					if ( !MASTER_BUFF_IS_EMPTY() ) {
						NOTIFY_STUB_NOT_CONNECTED();
					}
					CONTEXT->state = MASTER_ONLY;
					break;
				
				default:
					LOG( "Unknown event %d in state %s",
							event, state2str(CONTEXT->state) );
					break;
			}
			break;
	}

printf( "FSM EXIT: State=%s\n", state2str(CONTEXT->state)); fflush(stdout);
}

void *heartbeat_fn(void * arg)
{
	int broadcast_signal;
	struct timeval timeout = { 1, 0};
	
	while( 1 ) {

    	if (sendto(CONTEXT->heartbeatfd, (void *)&broadcast_signal, sizeof(broadcast_signal),
				0, (struct sockaddr*)&CONTEXT->broadcast, sizeof(CONTEXT->broadcast)) <0) {

			if (errno == EPERM) {/* Special handling */
				LOG( "Hearbeat broadcast's Sendto failed due to EPERM. Hopefully conntrack would permit it again\n");
			} else {
				ABORTERRNO( "sendto broadcast failed" );
			}
		}	

		if (select( 0, NULL, NULL, NULL, &timeout ) < 0 ) {
			ABORTERRNO( "timeout select failed" );
		}

	}

	return NULL;
}

void
init(void)
{
    int broadcastEnable = 1;
	int broadcast_signal = 1;

	CONTEXT->state = NONE;
	CONTEXT->maxfd = -1;
	CONTEXT->masterlistenfd = -1;
	CONTEXT->stublistenfd = -1;
	CONTEXT->masterconnfd = -1;
	CONTEXT->stubconnfd = -1;
	CONTEXT->master_buff_len = 0;
	CONTEXT->stub_buff_len = 0;
	
	// Open the 2 listening sockets 
	OPENMASTERSOCKET();
	OPENSTUBSOCKET();

	printf( "Master Listen Fd: %d, Stub listen fd: %d\n", CONTEXT->masterlistenfd, CONTEXT->stublistenfd ); fflush(stdout);
	
	// Open UDP heartbeat client socket
	CONTEXT->heartbeatfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (CONTEXT->heartbeatfd < 0) {
		ABORTERRNO( "failed to open UDP socket");
	}
	
	if(setsockopt(CONTEXT->heartbeatfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
	{
		ABORTERRNO( "failed to setsockopt UDP socket to enable broadcast");
	}

	memset(&CONTEXT->broadcast, 0, sizeof(CONTEXT->broadcast));
	CONTEXT->broadcast.sin_family = AF_INET;
	CONTEXT->broadcast.sin_port = htons(HEARTBEAT_PORT);
	CONTEXT->broadcast.sin_addr.s_addr = inet_addr("192.168.1.255");
	
    if (sendto(CONTEXT->heartbeatfd, (void *)&broadcast_signal, sizeof(broadcast_signal),
				0, (struct sockaddr*)&CONTEXT->broadcast, sizeof(CONTEXT->broadcast)) <0){
		if (errno == EPERM) {/* Special handling */
				LOG( "Hearbeat broadcast's Sendto failed due to EPERM. Hopefully conntrack would permit it again");
		} else {
			ABORTERRNO( "sendto broadcast failed" );
		}
	}

	// Start Heartbeat thread
 	if (pthread_create(&CONTEXT->heartbeat, NULL, &heartbeat_fn, NULL) < 0) {
		ABORTERRNO( "pthread_create heartbeat failed" );
	}

}

void
loop(void)
{
	fd_set read_fds, write_fds, err_fds;
	
	while(1) {

		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		FD_ZERO(&err_fds);
		
		if (CONTEXT->masterlistenfd >= 0) {
			FD_SET( CONTEXT->masterlistenfd, &read_fds);
			FD_SET( CONTEXT->masterlistenfd, &err_fds);
		}

		if (CONTEXT->stublistenfd >= 0) {
			FD_SET( CONTEXT->stublistenfd, &read_fds);
			FD_SET( CONTEXT->stublistenfd, &err_fds);
		}

		if (CONTEXT->masterconnfd >= 0) {
			FD_SET( CONTEXT->masterconnfd, &read_fds);
			FD_SET( CONTEXT->masterconnfd, &err_fds);
			if (!STUB_BUFF_IS_EMPTY()) {
				FD_SET( CONTEXT->masterconnfd, &write_fds);
			}
		}

		if (CONTEXT->stubconnfd >= 0) {
			FD_SET( CONTEXT->stubconnfd, &read_fds);
			FD_SET( CONTEXT->stubconnfd, &err_fds);
			if (!MASTER_BUFF_IS_EMPTY()) {
				FD_SET( CONTEXT->stubconnfd, &write_fds);
			}
		}

		if( select(CONTEXT->maxfd + 1, &read_fds, &write_fds, &err_fds, NULL) < 0) {
			ABORTERRNO( "select call failed" );
		}

fprintf( stdout, "select returned\n" ); fflush(stdout);

		if ( CONTEXT->masterlistenfd >= 0) {
			if (FD_ISSET(CONTEXT->masterlistenfd, &err_fds)) {
				if (errno == EPERM ) {
					LOG( "Master listen socked failed due to EPERM. Hopefully conntrack would permit it again");
				} else {
					ABORT( "master listen socket in error" );
				}
			} else if (FD_ISSET(CONTEXT->masterlistenfd, &read_fds)) {
printf( "mmmmmmmmmmmmmmmcccccccccccccccccccc\n" );fflush(stdout);
				fsm(MasterOpenedConnection);
			}
		}

		if ( CONTEXT->stublistenfd >= 0) {
			if (FD_ISSET(CONTEXT->stublistenfd, &err_fds)) {
				if (errno == EPERM ) {
					LOG( "Stub listen socked failed due to EPERM. Hopefully conntrack would permit it again");
				} else {
					ABORT( "stub listen socket in error" );
				}
			} else if (FD_ISSET(CONTEXT->stublistenfd, &read_fds)) {
printf( "ssssssssssssssscccccccccccccccccccc\n" );fflush(stdout);
				fsm(StubOpenedConnection);
			}
		}

		if ( CONTEXT->masterconnfd >= 0) {
			if (FD_ISSET(CONTEXT->masterconnfd, &err_fds)) {
				fsm(MasterClosedConnection);
			} else {

				if (FD_ISSET(CONTEXT->masterconnfd, &read_fds)) {
printf( "mmmmmmmmmmmmmmmrrrrrrrrrrrrrrrrrrrr\n" );fflush(stdout);
					fsm(DataFromMaster);
				}

				if (CONTEXT->masterconnfd >= 0 && FD_ISSET(CONTEXT->masterconnfd, &write_fds)) {
printf( "mmmmmmmmmmmmmmmwwwwwwwwwwwwwwwwwwww\n" );fflush(stdout);
					fsm(ReadyToWriteToMaster);
				}
			}
		}

		if ( CONTEXT->stubconnfd >= 0) {
			if (FD_ISSET(CONTEXT->stubconnfd, &err_fds)) {
				fsm(StubClosedConnection);
			} else {
				if (FD_ISSET(CONTEXT->stubconnfd, &read_fds)) {
printf( "sssssssssssssssrrrrrrrrrrrrrrrrrrrr\n" );fflush(stdout);
					fsm(DataFromStub);
				}

				if (CONTEXT->stubconnfd >= 0 && FD_ISSET(CONTEXT->stubconnfd, &write_fds)) {
printf( "ssssssssssssssswwwwwwwwwwwwwwwwwwww\n" );fflush(stdout);
					fsm(ReadyToWriteToStub);
				}
			}
		}		
	}
}

int
main( int argc, char **argv)
{
	init();
	// daemonize();
	loop();
}
