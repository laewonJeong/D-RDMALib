#include "D_RDMALib.hpp"
#define port 40145
#define num_of_node 4
#define server_ip "192.168.0.100"

string node[num_of_node] = {server_ip,"192.168.0.101","192.168.0.102","192.168.0.103"};

char send_buffer[num_of_node][buf_size];
char recv_buffer[num_of_node][buf_size];

bool is_server(string ip){
  if(ip == server_ip)
    return true;
  return false;
}

int main(int argc, char* argv[]){
  if(argc != 2)
  {
    cerr << argv[0] << " <MY IP> " << endl;
    exit(1);
  }
  if(server_ip != node[0]){
    cerr << "node[0] is not server_ip" << endl;
    exit(1);
  }
  D_RDMALib d_rdmalib;

  d_rdmalib.initialize_rdma_connection(argv[1], node, num_of_node, port,send_buffer,recv_buffer);
  d_rdmalib.create_rdma_info();
  d_rdmalib.send_info_change_qp();

  cerr << "================================================================================" << endl;
  
  string ip = argv[1];
  string msg;
  string opcode = "send"; //send, send_with_imm, write, write_with_imm
  
  cerr << "========================== many_to_many_communication ==========================" << endl;

  /* many to many communication*/
  msg = "[ " + ip + " ] Hi many-to-many communication!";
  d_rdmalib.rdma_comm(opcode, msg);
  for(int i=0;i<num_of_node-1;i++){
    printf("recv_buffer[%d]: %s\n", i, recv_buffer[i]); 
  }
  cerr << "========================== one_to_many_communication ==========================" << endl;

  /* 1 to many communication */
  if(is_server(ip)){
    msg = "Hi one-to-many communication!";
    d_rdmalib.rdma_one_to_many_send_msg(opcode, msg);
  }
  else{
    d_rdmalib.rdma_one_to_many_recv_msg(opcode);
    printf("recv_buffer[0]: %s\n", recv_buffer[0]);
  }

  cerr << "========================== many_to_one_communication ==========================" << endl;

  /* many to 1 communication */
  if(is_server(ip)){
    d_rdmalib.rdma_many_to_one_recv_msg(opcode);
    for(int i=0;i<num_of_node-1;i++){
      printf("recv_buffer[%d]: %s\n", i, recv_buffer[i]);
    }
  }
  else{
    msg = "Hi many-to-one communication!";
    d_rdmalib.rdma_many_to_one_send_msg(opcode,msg);
  }

  cerr << "================================================================================" << endl;

  d_rdmalib.exit_rdma();
  
  return 0;
}
