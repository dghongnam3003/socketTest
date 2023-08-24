#include "cd.h"
#include "ls.h"


#define TRUE 1
#define FALSE 0
#define INFINITE 100000

int main(int argc, char *argv[]) {
    int opt;
    char *root = NULL;
    int port = 0;
    char *ip;

    const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"root", required_argument, NULL, 'r'},
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "hr:p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                printf("Help message\n");
                exit(EXIT_SUCCESS);
            case 'r':
                root = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                ip = optarg;
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
    

    int server_fd, new_socket, valread, client_socket[INFINITE], max_clients = INFINITE, activity, sd;
    int max_sd;
    struct sockaddr_in address;
    int option = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR, &option,
                   sizeof(option))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
  
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    puts("Waiting for connection...");

    if ((new_socket = accept(server_fd, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
        {  
                perror("accept");  
                exit(EXIT_FAILURE);  
        }

    //to check if 'cd' was executed before 'ls'
    int cd_executed = 0;

    while(TRUE){
        

        memset(buffer, 0, sizeof(buffer));
        if (recv(new_socket, buffer, sizeof(buffer), 0) < 0) {
                perror("receive failed");
                exit(EXIT_FAILURE);
        }

        //create buffer array to input command
        char *buffer_array[1000];
        int token = 0;
        buffer_array[token] = strtok(buffer, "\t\n ");
        while(buffer_array[token] != NULL && token < 1000) {
                token++;
                buffer_array[token] = strtok(NULL, "\t\n ");
        }

        //set root to absolutely root of the machine
        char *abs_root;
        realpath(root, abs_root);
        chdir(abs_root);

        //handle commands
        if (strcmp(buffer_array[0], "cd") == 0) {
            char* current_directory = (char*)malloc(1024);
            getcwd(current_directory, sizeof(current_directory));
            char new_directory[1024];
            strcpy(new_directory, buffer_array[1]);

            cd(new_socket, &current_directory, new_directory, root);
            cd_executed = 1;
        }
        else if (strcmp(buffer_array[0],"ls") == 0) {
            
            char directory[1024];
            getcwd(directory, sizeof(directory));

            //if 'cd' was executed before, list root, if not, list current directory of the machine.
            if (cd_executed == 0) {
                list_files(new_socket, root);
            } else if (cd_executed == 1) {
                list_files(new_socket, directory);
            }
        }

        
    }

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);

    /*if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    char client_ip[INET_ADDRSTRLEN];
    struct sockaddr_in* addr_in = (struct sockaddr_in*)&address;
    inet_ntop(AF_INET, &(addr_in->sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Client IP address: %s\n", client_ip);

    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
  
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);*/
    

    return 0;
}