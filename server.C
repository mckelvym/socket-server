#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include "config.H"

using namespace std;


#define SERVER_PORT 8800		/* arbitrary, but client and server must agree */
#define BUF_SIZE 4096			/* block transfer size */
#define QUEUE_SIZE 10

// ---------------------------------------------------------------------------
char const *progname;
config_t *cfg;

void setup_connection ();
void quit_app ();
void signal_handler (int signal);
void print_usage_and_exit ();
void message (string type, string str);
// ---------------------------------------------------------------------------


int main(int argc, char *args[])
{
	int opt;
	bool usage_error = false;

	// Set default values
	progname = basename (args[0]);
	signal (SIGINT, signal_handler);
	signal (SIGKILL, signal_handler);
	cfg = new config_t ();

	while ((opt = getopt (argc, args, "b:p:q:s:v?")) != -1 && !usage_error)
	{
		switch (opt)
		{
		case 'b':
			if (sscanf (optarg, "%u", &cfg->m_buffer_size) != 1)
				usage_error = true;
			break;
		case 'p':
			if (sscanf (optarg, "%u", &cfg->m_server_port) != 1)
				usage_error = true;
			break;
		case 'q':
			if (sscanf (optarg, "%u", &cfg->m_queue_size) != 1)
				usage_error = true;
			break;
		case 's':
			cfg->m_serial_device = optarg;
			break;
		case 'v':
			cfg->m_verbose = true;
			break;
		case '?':
			usage_error = true;
			break;
		}
	}
	if (usage_error)
		print_usage_and_exit ();

	// ----------------------------------------


	setup_connection ();

	int bytes, ackbytes;
	char * buffer = new char[cfg->m_buffer_size];
	char * ack = new char[cfg->m_buffer_size];


	// Socket is now set up and bound. Wait for connection and process it.
	while (true)
	{
		message ("info", "waiting for connection.");
		// block while waiting for connection request
		cfg->m_socket_fd = accept (cfg->m_socket_id, 0, 0);

		if (cfg->m_socket_fd < 0)
		{
			message ("error", "can't accept connections.");
			quit_app ();
		}
		else
			message ("info", "got a connection.");

		// open serial device to begin reading data
		message ("info", "opening serial device for reading.");
		cfg->m_serial_fd = open (cfg->m_serial_device.c_str (), O_RDONLY);
		if (cfg->m_serial_fd < 0)
		{
			message ("error", "couldn't open device for reading (are you root?)");
			quit_app ();
		}
		while (true)
		{
			bytes = read (cfg->m_serial_fd, buffer, cfg->m_buffer_size); /* read from file */
			ackbytes = read (cfg->m_socket_fd, ack, cfg->m_buffer_size);
			message ("info", "finished reading data.");

			if (bytes <= 0)	{
				message ("warning", "no data read from serial device.");
				break;
			}
			if (ackbytes > 0 && ack[0] == '1'){
				message ("info", "writing data to client.");
				int sent = send (cfg->m_socket_fd, buffer, bytes, 0);
				if (cfg->m_verbose)
					fprintf (stderr, "%s: info: wrote %u bytes.\n",
						 progname, sent);
			}
			else
			{
				message ("info", "disconnected from client.");
				break;
			}
		}
		message ("info", "closing file descriptors for socket and serial device.");
		close (cfg->m_serial_fd);
		close (cfg->m_socket_fd);
	}
}

void setup_connection ()
{
	int on = 1;

	// Build address structure to bind to socket.
	memset (&cfg->m_channel, 0, sizeof(cfg->m_channel));
	cfg->m_channel.sin_family = AF_INET;
	cfg->m_channel.sin_addr.s_addr = htonl (INADDR_ANY);
	cfg->m_channel.sin_port = htons (cfg->m_server_port);

	// Passive open. Wait for connection.
	message ("info", "allocating socket.");
	cfg->m_socket_id = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cfg->m_socket_id < 0)
	{
		message ("error", "socket allocation failed.");
		quit_app ();
	}
	setsockopt (cfg->m_socket_id, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof (on));

	int bind_result = bind (cfg->m_socket_id, (struct sockaddr *) &cfg->m_channel, sizeof (cfg->m_channel));
	if (bind_result < 0)
	{
		message ("error", "couldn't bind to socket.");
		quit_app ();
	}
	else
		message ("info", "socket is bound.");

	message ("info", "setting up listen queue.");
	int listen_result = listen (cfg->m_socket_id, cfg->m_queue_size);		/* specify queue size */
	if (listen_result < 0)
	{
		message ("error", "couldn't setup listen queue.");
		quit_app ();
	}
}

void quit_app ()
{
	message ("info", "closing socket and shutting down.\n");

	// indicate to server to stop sending data
	char ack[2] = "0";
	send (cfg->m_socket_id, ack, strlen (ack)+1, 0);
	shutdown (cfg->m_socket_id, SHUT_RDWR);
	exit (0);
}

void signal_handler (int signal)
{
	printf ("%s: received signal %d, Exiting.\n",
		progname, signal);
	quit_app ();
}

void print_usage_and_exit ()
{
	printf ("\nUsage: %s <options>\n", progname);
	printf ("\t[-b #]\t\t Buffer transfer size [%u].\n", cfg->m_buffer_size);
	printf ("\t[-p #]\t\t Port to use [%u].\n", cfg->m_server_port);
	printf ("\t[-q #]\t\t Queue size [%u].\n", cfg->m_queue_size);
	printf ("\t[-s dev]\t Serial device to read from (must have proper permissions) [%s].\n",
		cfg->m_serial_device.c_str ());
	printf ("\t[-v]\t\t Turn on verbosity.\n");
	printf ("\n\n");
	exit (0);
}

void message (string type, string str)
{
	if (cfg->m_verbose)
	{
		fprintf (stderr, "%s: %s: %s\n", progname,
			 type.c_str (),
			 str.c_str ());
	}
}

