#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <errno.h>

void send_message(int client_socket, const char* message) {
    if (send(client_socket, message, strlen(message), 0) < 0) {
        perror("Error");
        exit(EXIT_FAILURE);
    }
}

void cd(int client_socket, char** current_directory, char* new_directory, char* root_directory) {
    char* target_directory = (char*)malloc(1024);


    // handle absolute path
    if (new_directory[0] == '/') {
        snprintf(target_directory, 1024, "%s", new_directory);
    }
    else if (sizeof(new_directory) == 0) {
        snprintf(target_directory, 1024, "%s", root_directory);
    }
    // handle home directory
    else if (new_directory[0] == '~') {
        if (sizeof(new_directory) == 1) {snprintf(target_directory, 1024, "%s", root_directory);}    
        if (sizeof(new_directory) > 1) {
            memmove(new_directory, new_directory+1, strlen(new_directory));
            char* result_directory = (char*)malloc(1024);
            snprintf(result_directory, 1024, "%s%s", root_directory, new_directory);
            snprintf(target_directory, 1024, "%s", result_directory);
        }
    }
    // handle previous directory
    else if (strcmp(new_directory, "-") == 0) {
        snprintf(target_directory, 1024, "%s", getenv("OLDPWD"));
    }
    // handle parent directory
    else if (strcmp(new_directory, "..") == 0) {
        // Find the last slash in the current directory
        size_t last_slash = strlen(*current_directory) - 1;
        while (last_slash > 0 && (*current_directory)[last_slash] != '/') {
            last_slash--;
        }
        if (last_slash > 0) {
            // Truncate the current directory path
            (*current_directory)[last_slash] = '\0';
            snprintf(target_directory, 1024, "%s", *current_directory);
        } else {
            send_message(client_socket, "Cannot move up from root directory");
            free(target_directory);
            return;
        }
    }
    // handle root directory
    else if (strcmp(new_directory, "/") == 0) {
        snprintf(target_directory, 1024, "%s", root_directory);
    }
    // handle relative path
    else {
        snprintf(target_directory, 1024, "%s/%s", *current_directory, new_directory);
    }

    // check if target directory is outside root directory
    if (strncmp(target_directory, root_directory, strlen(root_directory)) != 0) {
        send_message(client_socket, "Cannot access directory outside root directory");
        free(target_directory);
        return;
    }

    // attempt to change directory
    if (chdir(target_directory) == 0) {
        getcwd(*current_directory, 1024);
        setenv("OLDPWD", *current_directory, 1);
        send_message(client_socket, "Directory changed");
    } else {
        send_message(client_socket, "No such file or directory");
    }

    free(target_directory);
}

/*int main() {
    char* current_directory = (char*)malloc(1024);
    strcpy(current_directory, "/home/yong/Socket/code");
    char* root_directory = "/home/yong";
    char new_directory[1024];

    // Example usage of cd function
    printf("Current directory: %s\n", current_directory);

    strcpy(new_directory, "..");
    cd(0, &current_directory, new_directory, root_directory);
    printf("Current directory: %s\n", current_directory);

    strcpy(new_directory, "/home");
    cd(0, &current_directory, new_directory, root_directory);
    printf("Current directory: %s\n", current_directory);

    strcpy(new_directory, "~");
    cd(0, &current_directory, new_directory, root_directory);
    printf("Current directory: %s\n", current_directory);

    strcpy(new_directory, "-");
    cd(0, &current_directory, new_directory, root_directory);
    printf("Current directory: %s\n", current_directory);

    free(current_directory);
    return 0;
} */
