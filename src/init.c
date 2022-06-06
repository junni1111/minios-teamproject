#include <stdio.h>
#include <stdlib.h>

#include "main.h"

void init(DirectoryTree *p_directoryTree, char *command) {
    char *str;
    int isDirectoryExist;
    if (strcmp(command, "") == 0 || command[0] == ' ') {
        return;
    }
    str = strtok(command, " ");

    if (strcmp(str, "clear") == 0 || strcmp(str, "cls") == 0) {
        system("clear");
    } else if (strcmp(str, "mkdir") == 0) {
        str = strtok(NULL, " ");
        isDirectoryExist = mkdir(p_directoryTree, str);
        if (isDirectoryExist == 0) {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    } else if (strcmp(str, "touch") == 0) {
        str = strtok(NULL, " ");
        isDirectoryExist = touch(p_directoryTree, str);
        if (isDirectoryExist == 0) {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    } else if (strcmp(str, "cp") == 0) {
        str = strtok(NULL, " ");
        isDirectoryExist = cp(p_directoryTree, str);
        if (isDirectoryExist == 0) {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    } else if (strcmp(str, "rm") == 0) {
        str = strtok(NULL, " ");
        isDirectoryExist = rm(p_directoryTree, str);
        if (isDirectoryExist == 0) {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    } else if (strcmp(str, "cd") == 0) {
        str = strtok(NULL, " ");
        cd(p_directoryTree, str);
    } else if (strcmp(str, "pwd") == 0) {
        str = strtok(NULL, " ");
        pwd(p_directoryTree, gp_directoryStack, str);
    } else if (strcmp(str, "ls") == 0) {
        str = strtok(NULL, " ");
        ls(p_directoryTree, str);
    } else if (strcmp(str, "cat") == 0) {
        str = strtok(NULL, " ");
        isDirectoryExist = cat(p_directoryTree, str);
        if (isDirectoryExist == 0) {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    } else if (strcmp(str, "chmod") == 0) {
        str = strtok(NULL, " ");
        isDirectoryExist = chmod(p_directoryTree, str);
        if (isDirectoryExist == 0) {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    } else if (strcmp(str, "find") == 0) {
        str = strtok(NULL, " ");
        find_(p_directoryTree, str);
    } else if (strcmp(command, "exit") == 0) {
        printf("로그아웃\n");
        exit(0);
    } else {
        printf("'%s': 명령을 찾을 수 없습니다\n", command);
    }
    return;
}

void print_start() {
    printf("\n");
    printf("Last login: ");
    get_weekday(gp_userList->current->wday);
    get_month(gp_userList->current->month);
    printf("%d %02d:%02d:%02d %d\n", gp_userList->current->day, gp_userList->current->hour, gp_userList->current->minute, gp_userList->current->sec, gp_userList->current->year);

    BOLD;
    BLUE;
    printf("Welcome to miniOneS 1.0.0\n\n");
    printf("2022 1st semester OS team project\n");
    printf("팀장 : 조연준\n");
    printf("팀원 : 서호준, 윤덕우, 강채은, 조규용\n");
    printf("\n");
    printf("\n");
    printf("\n");
    DEFAULT;
}

void print_head(DirectoryTree *p_directoryTree, Stack *p_directoryStack) {
    // variables
    DirectoryNode *tmpNode = NULL;
    char tmp[MAX_DIRECTORY_SIZE] = "";
    char tmp2[MAX_DIRECTORY_SIZE] = "";
    char usr;

    if (gp_userList->current == gp_userList->head)
        usr = '#';
    else
        usr = '$';

    BOLD;
    GREEN;
    printf("%s@miniOneS", gp_userList->current->name);
    DEFAULT;
    printf(":");
    tmpNode = p_directoryTree->current;

    if (tmpNode == p_directoryTree->root) {
        strcpy(tmp, "/");
    } else {
        while (tmpNode->Parent != NULL) {
            push(p_directoryStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        while (is_empty(p_directoryStack) == 0) {
            strcat(tmp, "/");
            strcat(tmp, pop(p_directoryStack));
        }
    }

    strncpy(tmp2, tmp, strlen(gp_userList->current->dir));

    if (gp_userList->current == gp_userList->head) {
        BOLD;
        BLUE;
        printf("%s", tmp);
    } else if (strcmp(gp_userList->current->dir, tmp2) != 0) {
        BOLD;
        BLUE;
        printf("%s", tmp);
    } else {
        tmpNode = p_directoryTree->current;
        while (tmpNode->Parent != NULL) {
            push(p_directoryStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        pop(p_directoryStack);
        pop(p_directoryStack);
        BOLD;
        BLUE;
        printf("~");
        while (is_empty(p_directoryStack) == 0) {
            printf("/");
            printf("%s", pop(p_directoryStack));
        }
    }
    DEFAULT;
    printf("%c ", usr);
}
