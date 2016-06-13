#include "ace/Log_Msg.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_main.h"
#include "ace/ATM_Acceptor.h"
#include "ace/ATM_Addr.h"
#include "ace/High_Res_Timer.h"

#define SIZE_DATA 18
#define	SIZE_BUF 1024
#define NO_ITERATIONS 5

class Server{
private:
	char *data_buf_;
	ACE_INET_Addr server_addr_;
	ACE_INET_Addr client_addr_;
	ACE_SOCK_Acceptor peer_acceptor_;
	ACE_SOCK_Stream new_stream_;
public:
	Server(int port) :server_addr_(port), peer_acceptor_(server_addr_){
		data_buf_ = new char[SIZE_BUF];
	}
	int handle_connection(){
		for (int i = 0; i < NO_ITERATIONS; i++){
			int byte_count = 0;
			if ((byte_count = new_stream_.recv_n(data_buf_, SIZE_DATA, 0)) == -1){
				ACE_ERROR((LM_ERROR, "%p\n", "Error in recv"));
			}
			else{
				data_buf_[byte_count] = 0;
				ACE_DEBUG((LM_DEBUG, "Server received %s \n", data_buf_));
			}
		}
		if (new_stream_.close() == -1){
			ACE_ERROR((LM_ERROR,"%p\n","close"));
		}
		return 0;
	}

	int accept_connections(){
		if (peer_acceptor_.get_local_addr(server_addr_) == -1){
			ACE_ERROR_RETURN((LM_ERROR, "%p\n", "Error in get_local_addr"), 1);
		}
		ACE_DEBUG((LM_DEBUG, "Starting server at port %d\n", server_addr_.get_port_number()));
		while (1){
			ACE_Time_Value timeout(ACE_DEFAULT_TIMEOUT);
			if (peer_acceptor_.accept(new_stream_, &client_addr_, &timeout)){
				ACE_ERROR((LM_ERROR, "%p\n", "accept"));
				continue;
			}
			else{
				ACE_DEBUG((LM_DEBUG, "Connection established with remote %s:%d\n",
					client_addr_.get_host_name(), client_addr_.get_port_number()));
				handle_connection();
			}

		}
	}
};

int main(int argc, char *argv[]){
	if (argc < 2){
		ACE_ERROR((LM_ERROR, "Usage %s <port number>", argv[0]));
		ACE_OS::exit(1);
	}
	Server server(ACE_OS::atoi(argv[1]));
	//Server server(11223);
	server.accept_connections();
	return 0;
}