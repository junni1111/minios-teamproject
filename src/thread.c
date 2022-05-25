#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "main.h"

void *thread_routine_make_directory(void *arg) {
    ThreadArg *p_threadArg = ((ThreadArg *)arg);
    DirectoryTree *p_directoryTree = p_threadArg->p_directoryTree;
    DirectoryTree *p_preTree;
    char *command = p_threadArg->command;

    DirectoryNode *tmpNode = p_directoryTree->current;
    char tmp[MAX_DIRECTORY_SIZE];
    char pStr[MAX_DIRECTORY_SIZE];
    char tmpStr[MAX_DIRECTORY_SIZE];
    char directoryName[MAX_DIRECTORY_SIZE];
    int directoryNameLength = 0;
    int isDirectoryExist;

    strncpy(tmp, command, MAX_DIRECTORY_SIZE);

    if (strstr(command, "/") == NULL) {
        make_new(p_directoryTree, command, 'd', p_threadArg->additionalValue);
    } else if (p_threadArg->additionalValue && strlen(p_threadArg->additionalValue) == 1) {  // -p threading
        int tmpLen = strlen(tmp), i = 0;
        if (tmp[0] == '/') {
            p_directoryTree->current = p_directoryTree->root;
            i = 1;
        }
        if (tmp[tmpLen - 1] == '/') {
            tmpLen -= 1;
        }
        for (; i < tmpLen; i++) {
            pStr[i] = tmp[i];
            pStr[i + 1] = 0;
            directoryName[directoryNameLength++] = tmp[i];
            if (tmp[i] == '/') {
                directoryName[--directoryNameLength] = 0;
                strncpy(tmpStr, pStr, i - 1);
                isDirectoryExist = move_current_tree(p_directoryTree, directoryName);
                if (isDirectoryExist == -1) {
                    make_new(p_directoryTree, directoryName, 'd', NULL);
                    isDirectoryExist = move_current_tree(p_directoryTree, directoryName);
                    printf("%d, %s, %s \n", isDirectoryExist, pStr, p_directoryTree->current->name);
                }
                directoryNameLength = 0;
            }
        }
        make_new(p_directoryTree, directoryName, 'd', NULL);
        p_directoryTree->current = tmpNode;
    } else {
        char *p_get_directory = get_directory(command);
        isDirectoryExist = move_directory_path(p_directoryTree, p_get_directory);
        if (isDirectoryExist != 0) {
            printf("mkdir: '%s': 그런 파일이나 디렉터리가 없습니다\n", p_get_directory);
        } else {
            char *str = strtok(tmp, "/");
            char *p_directory_name;
            while (str != NULL) {
                p_directory_name = str;
                str = strtok(NULL, "/");
            }
            make_new(p_directoryTree, p_directory_name, 'd', p_threadArg->additionalValue);
            p_directoryTree->current = tmpNode;
        }
    }

    pthread_exit(NULL);
}

void *thread_routine_touch(void *arg) {
    ThreadArg *p_threadArg = ((ThreadArg *)arg);
    DirectoryTree *p_directoryTree = p_threadArg->p_directoryTree;
    char *command = p_threadArg->command;

    DirectoryNode *tmpNode = p_directoryTree->current;
    char tmp[MAX_DIRECTORY_SIZE];
    int isDirectoryExist;

    strncpy(tmp, command, MAX_DIRECTORY_SIZE);
    if (strstr(command, "/") == NULL) {
        make_new(p_directoryTree, command, 'f', p_threadArg->additionalValue);
    } else {
        char *p_get_directory = get_directory(command);
        isDirectoryExist = move_directory_path(p_directoryTree, p_get_directory);
        if (isDirectoryExist != 0) {
            printf("touch: '%s': 그런 파일이나 디렉터리가 없습니다\n", p_get_directory);
        } else {
            char *str = strtok(tmp, "/");
            char *p_directory_name;
            while (str != NULL) {
                p_directory_name = str;
                str = strtok(NULL, "/");
            }
            make_new(p_directoryTree, p_directory_name, 'f', p_threadArg->additionalValue);
            p_directoryTree->current = tmpNode;
        }
    }

    pthread_exit(NULL);
}

void *thread_routine_copy(void *arg) {
    ThreadArg *p_threadArg = ((ThreadArg *)arg);
    DirectoryTree *p_directoryTree = p_threadArg->p_directoryTree;
    char *command = p_threadArg->command;

    DirectoryNode *tmpNode = p_directoryTree->current;
    char tmp[MAX_DIRECTORY_SIZE];
    int isDirectoryExist;

    strncpy(tmp, command, MAX_DIRECTORY_SIZE);
    if (strstr(command, "/") == NULL) {
        make_new(p_directoryTree, command, 'd', p_threadArg->additionalValue);
    } else {
        char *p_get_directory = get_directory(command);
        isDirectoryExist = move_directory_path(p_directoryTree, p_get_directory);
        if (isDirectoryExist != 0) {
            printf("mkdir: '%s': 그런 파일이나 디렉터리가 없습니다\n", p_get_directory);
        } else {
            char *str = strtok(tmp, "/");
            char *p_directory_name;
            while (str != NULL) {
                p_directory_name = str;
                str = strtok(NULL, "/");
            }
            make_new(p_directoryTree, p_directory_name, 'd', p_threadArg->additionalValue);
            p_directoryTree->current = tmpNode;
        }
    }

    pthread_exit(NULL);
}
