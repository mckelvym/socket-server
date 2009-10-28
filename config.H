#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <netinet/in.h>

using namespace std;

class config_t
{
public:
	config_t ();

	bool m_verbose;

	string m_serial_device;

	// client and server must agree on port
	unsigned m_server_port;

	// block transfer size
	unsigned m_buffer_size;

	// sleep amount before trying reconnect
	unsigned m_sleep_amount;

	// used to represent an entry in the hosts database,
	// info about server
	struct hostent *m_hostentry;

	// a local or remote endpoint address that a socket connects to,
	// holds IP address
	struct sockaddr_in m_channel;

	// server queue size for number of connections
	unsigned m_queue_size;

	int m_socket_id;
	int m_connection_id;
	int m_socket_fd;
	int m_serial_fd;

private:
};

#endif
