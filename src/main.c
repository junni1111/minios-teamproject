#include "main.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    char command[50];

    gp_directoryTree = load_directory();
    gp_userList = load_user_list();
    gp_directoryStack = initialize_stack();

    login(gp_userList, gp_directoryTree);
    print_start();
    save_user_list(gp_userList);

    while (1) {
        if (feof(stdin)) {
            rewind(stdin);
            printf("\n");
        }
        print_head(gp_directoryTree, gp_directoryStack);
        fgets(command, sizeof(command), stdin);
        command[strlen(command) - 1] = '\0';
        init(gp_directoryTree, command);
    }

    return 0;
}
