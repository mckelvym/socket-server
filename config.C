#include <cassert>
#include "config.H"

config_t::config_t ()
{
	m_verbose = false;
	m_serial_device = "/dev/ttyS0";
	m_server_port = 8800;
	m_buffer_size = 4096;
	m_queue_size = 2;
	m_socket_id = -1;
	m_connection_id = -1;
	m_socket_fd = -1;
	m_serial_fd = -1;
}

