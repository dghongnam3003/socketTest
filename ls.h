#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>



long get_file_size(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

void list_files(int client_socket, const char* directory) {
    char response[1024] = ""; // Buffer to store the response message
    DIR* dir = opendir(directory); // Open the directory
    if (dir == NULL) {
        strcpy(response, "Cannot open directory."); // Directory opening failed
        send_message(client_socket, response); // Send error message to client
        return;
    }
    
    struct dirent* entry; // Directory entry structure
    while ((entry = readdir(dir)) != NULL) { // Read each directory entry
        char name[256];
        strcpy(name, entry->d_name); // Get the name of the entry
        
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            // Exclude current directory (".") and parent directory ("..")

            char path[512];
            snprintf(path, sizeof(path), "%s/%s", directory, name);
            // Construct the full path of the entry
            
            char type = (entry->d_type == DT_DIR ? 'd' : '-');
            // Determine the type of the entry ('d' for directory, '-' for file)
            
            char size[16];
            if (entry->d_type == DT_DIR) {
                strcpy(size, "-"); // Set size as "-" for directories
            } else {
                long file_size = get_file_size(path);
                snprintf(size, sizeof(size), "%ld", file_size);
                // Get the size of the file (in bytes) and convert it to a string
            }
            
            char entry_info[512];
            snprintf(entry_info, sizeof(entry_info), "%-24c%-24s%s\n", type, name, size);
            // Format the entry information (type, name, size) as a string
            
            strcat(response, entry_info); // Append the entry information to the response
        }
    }
    
    closedir(dir); // Close the directory
    send_message(client_socket, response); // Send the response to the client
}

/*int main() {
    const char* directory = "/home/dum/new";
    // Example usage of list_files function
    list_files(0, directory);
    
    return 0;
}*/
