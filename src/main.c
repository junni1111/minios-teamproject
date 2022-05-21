#include "./main.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char cmd[50];

    gp_directoryTree = load_directory();
    gp_userList = load_user_list();
    gp_directoryStack = initialize_stack();

    login(gp_userList, gp_directoryTree);
    print_start();
    save_user_list(gp_userList);

    while (1)
    {
        print_head(gp_directoryTree, gp_directoryStack);
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strlen(cmd) - 1] = '\0';
        instruction(gp_directoryTree, cmd);
    }

    return 0;
}
