#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// void send_message(int client_socket, const char* message) {
//     if (send(client_socket, message, strlen(message), 0) < 0) {
//         perror("Error");
//         exit(EXIT_FAILURE);
//     }
// }

void get_file(int client_socket, const char* directory, const char* filename) {
    // Construct the full path of the file on the server
    char server_path[512];
    snprintf(server_path, sizeof(server_path), "%s/%s", directory, filename);

    FILE* server_file = fopen(server_path, "rb");
    if (server_file == NULL) {
        send_message(client_socket, "Cannot open file on server.");
        return;
    }

    // Open the file in the client's current directory for writing
    // char client_path[1024];
    // char current_directory[512];
    // getcwd(current_directory, sizeof(current_directory));
    // snprintf(client_path, sizeof(client_path), "%s/%s", current_directory, filename);

    char client_path[1024];
    char* relative_path = "client.c";
    char resolved_path[1024];
    if (realpath(relative_path, resolved_path) != NULL) {
        size_t last_slash = strlen(resolved_path) - 1;
        while (last_slash > 0 && (resolved_path)[last_slash] != '/') {
            last_slash--;
        }
        if (last_slash > 0) {
            // Truncate the current directory path
            (resolved_path)[last_slash] = '\0';
            snprintf(client_path, 1024, "%s", resolved_path);
        }
    }
    FILE* client_file = fopen(client_path, "wb");
    if (client_file == NULL) {
        fclose(server_file);
        send_message(client_socket, "Cannot create file on client.");
        return;
    }

    // Read chunks of data from the server and write to the client file
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), server_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, client_file);
        if (bytes_written != bytes_read) {
            fclose(server_file);
            fclose(client_file);
            send_message(client_socket, "Error writing to client file.");
            return;
        }
    }

    fclose(server_file);
    fclose(client_file);
    char sent_mess[1024];
    snprintf(sent_mess, 1024, "%s %s %s", "Download", filename, "100%");
    send_message(client_socket, sent_mess);
}

// int main() {
//     const char* directory = "/home/dum/new";
//     const char* filename = "example.txt";

//     // Example usage of get_file function
//     get_file(0, directory, filename); // Assuming client socket value 0 represents standard output

//     return 0;
// }

