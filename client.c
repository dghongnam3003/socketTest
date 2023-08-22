#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>  

int main(int argc, char *argv[]) 
{
    int opt;
    char *ip = NULL;
    int port = 0;

    const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"ip", required_argument, NULL, 'i'},
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "hi:p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                printf("Help message\n");
                exit(EXIT_SUCCESS);
            case 'i':
                ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                printf("Unknown option: %s\n", argv[optind-1]);
                break;
            case ':':
                printf("Option requires an argument: %s\n", argv[optind-1]);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    //char* hello = "Den gio roi";
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    

    while(1) {
        //get command from user 
        char cmd[128] = "";
        printf("Enter command: ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0;

        //send command to server
        if (send(client_fd, cmd, strlen(cmd), 0) < 0) {
            printf("send failed");
            return -1;
        }

        //receive output from server
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_fd, buffer, sizeof(buffer), 0) < 0) {
            printf("\nreceive failed\n");
            return -1;
        }
        //print output from console
        printf("%s\n", buffer);
    }
  
    // closing the connected socket
    close(client_fd);

    return 0;
}