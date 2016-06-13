#include "ace/Log_Msg.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_main.h"
#include "ace/ATM_Acceptor.h"
#include "ace/ATM_Addr.h"
#include "ace/High_Res_Timer.h"
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "ace/OS_NS_string.h"

#define SIZE_BUF 128
#define NO_ITERATIONS 5

class Client{
private:
	char *data_buf_;
	ACE_INET_Addr remote_addr_;
	ACE_SOCK_Connector connector_;
	ACE_SOCK_Stream client_stream_;
public:
	Client(char *hostname, int port) :remote_addr_(port, hostname){
		data_buf_ = "Hello from client";
	}
	int connect_to_server(){
		ACE_DEBUG((LM_DEBUG, "(%P|%t) Starting connect to %s:%d",
			remote_addr_.get_host_name(), remote_addr_.get_port_number()));
		if ((connector_.connect(client_stream_, remote_addr_)) == -1){
			ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "connection failed"),-1);
		}
		else
		{
			ACE_DEBUG((LM_DEBUG, "(%P|%t) connect to %s\n",
				remote_addr_.get_host_name()));
		}
		return 0;
	}
	int send_to_server(){
		for (int i = 0; i < NO_ITERATIONS; i++){
			if (client_stream_.send_n(data_buf_,
				ACE_OS::strlen(data_buf_) + 1, 0) == -1){
				ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "send_n"), 0);
				break;
			}
		}
		close();
	}
	int close(){
		if (client_stream_.close() == -1){
			ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "CLOSE"), -1);
		}
		else{
			return 0;
		}
	}
};

int main(int argc, char *argv[]){
	if (argc < 3){
		ACE_DEBUG((LM_DEBUG, "Usage %s <hostname> <portnumber>\n", argv[0]));
		ACE_OS::exit(1);
	}
	Client client(argv[1], ACE_OS::atoi(argv[2]));
	client.connect_to_server();
	client.send_to_server();
}