#include "stdio.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "string.h"
#include "arpa/inet.h"
#include "stdlib.h"
#include "unistd.h"
#include "netinet/in.h"
#include "netinet/tcp.h"
#define SIZE 1048576*2 //for 2MB
#define xor "1100010100100100"       //   5    2    3    0
                                     // 0101 0010 0011 0000
                                     //xor
                                     //   9    7    1    4
                                     // 1001 0111 0001 0100
                                     //----------------------
                                     // 1100 0101 0010 0100

    int send_file(char *data, int sender_socket){
        size_t a;
        if((a=send(sender_socket,data,SIZE/2,0))==-1){
        perror("error in sending data.\n");
        exit(1);
        }
        bzero(data,SIZE/2);
        printf("a=%ld\n",a);
        return 0;
    }

    int send_file2(char *data, int sender_socket){
        size_t b;
        if((b=send(sender_socket,&data[SIZE/2],SIZE/2,0))==-1){
        perror("error in sending data.\n");
        exit(1);
        }
        bzero(data,SIZE/2);
        printf("b=%ld\n",b);
        return 0;
    }


int main(){
    //creating a socket
    int sender_socket;
    sender_socket=socket(AF_INET, SOCK_STREAM, 0);//because we are in linux the default cc is cubic.
    if(sender_socket==-1){
        printf("there is a problem with initializing sender.\n");
    }
    else{
        printf("-initialize successfully.\n");
    }

struct sockaddr_in Receiver_address;
FILE *fp;
char *filename ="Sender_massege.txt";

Receiver_address.sin_family = AF_INET;
Receiver_address.sin_port = htons(9999);
Receiver_address.sin_addr.s_addr = INADDR_ANY;

int connection_status = connect(sender_socket,(struct sockaddr *) &Receiver_address,sizeof(Receiver_address));
if(connection_status==-1){
    printf("there is an error with the connection.\n");
}
else{
printf("-connected.\n");
}

fp = fopen(filename, "r");
if(fp==NULL){
    perror("Error in reading file.");
    exit(1);
}

char data [SIZE]={0};//mooving text into array
printf("fread = %ld\n",fread(data,sizeof(char),SIZE,fp));


if(send_file(data,sender_socket)==0){
printf("-File data has been send successfully1.\n");
}
char server_response[33];
recv(sender_socket,&server_response, sizeof(server_response),0);
printf("The server sent the data: %s .\n", server_response);
if(!strcmp(xor,server_response))
{
    char *Reno = "reno";
    socklen_t Reno_len = strlen(Reno);
    if (setsockopt(sender_socket, IPPROTO_TCP,TCP_CONGESTION,Reno,Reno_len) != 0)//the change in CC from Cubic to Reno
    {
        perror("setsockopt");
        exit(1);
    }
    else{
        printf("-CC has changed.\n");
    }
     if(send_file2(data,sender_socket)==0){
        printf("-File data has been send successfully2.\n");
    }
}
else{
    perror("-The xor didn't make it.\n");
    exit(1);
}


close(sender_socket);
printf("-closing...\n");

return 0;
}