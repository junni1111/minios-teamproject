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
                }
                directoryNameLength = 0;
            }
        }
        directoryName[directoryNameLength] = 0;
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
    if (p_threadArg->additionalValue && strlen(p_threadArg->additionalValue) == 1) {  // -m
        DirectoryNode *fileNode = is_exist_directory(p_directoryTree, tmp, 'f');
        if (fileNode == NULL) {
            printf("touch: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp);
        } else {
            // get time
            time(&ltime);
            today = localtime(&ltime);
            // mode_to_permission(fileNode);
            fileNode->month = today->tm_mon + 1;
            fileNode->day = today->tm_mday;
            fileNode->hour = today->tm_hour;
            fileNode->minute = today->tm_min;
        }
    } else if (p_threadArg->additionalValue) {  // -t
        DirectoryNode *fileNode = is_exist_directory(p_directoryTree, tmp, 'f');

        if (fileNode == NULL) {
            printf("touch: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp);
        } else {
            char time[10];
            strcpy(time, p_threadArg->additionalValue);
            int month, day, hour, minute;
            month = (time[0] - '0') * 10 + (time[1] - '0');
            day = (time[2] - '0') * 10 + (time[3] - '0');
            hour = (time[4] - '0') * 10 + (time[5] - '0');
            minute = (time[6] - '0') * 10 + (time[7] - '0');

            // mode_to_permission(fileNode);
            fileNode->month = month;
            fileNode->day = day;
            fileNode->hour = hour;
            fileNode->minute = minute;
        }
    } else if (strstr(command, "/") == NULL) {
        make_new(p_directoryTree, command, 'f', NULL);
    } else {
        char *p_get_directory = get_directory(command);
        isDirectoryExist = move_directory_path(p_directoryTree, p_get_directory);
        if (isDirectoryExist != 0) {
            printf("touch: '%s': 그런 파일이나 디렉터리가 없습니다\n", p_get_directory);
        } else {
            char *str = strtok(tmp, "/");
            char *p_file_name;
            while (str != NULL) {
                p_file_name = str;
                str = strtok(NULL, "/");
            }
            make_new(p_directoryTree, p_file_name, 'f', NULL);
            p_directoryTree->current = tmpNode;
        }
    }

    pthread_exit(NULL);
}

void *thread_routine_copy(void *arg) {
    ThreadArg *p_threadArg = ((ThreadArg *)arg);
    DirectoryTree *p_directoryTree = p_threadArg->p_directoryTree;
    char *command = p_threadArg->command;
    char *fileName = p_threadArg->additionalValue;  // fileName
    char *copyPath = p_threadArg->copyPath;         // copyPath

    DirectoryNode *tmpNode = NULL;
    DirectoryNode *preNode = p_directoryTree->current;
    DirectoryNode *fileNameNode = p_threadArg->fileNameNode;
    char tmp[MAX_DIRECTORY_SIZE];
    int isDirectoryExist;

    if (copyPath) {  // copyPath가 존재할때
        move_directory_path(p_directoryTree, copyPath);
    }

    if (strcmp(command, fileName) == 0 && strcmp(command, "-r") != 0) {  // 복사할 파일이랑 대상이랑 이름이 같으면 종료 (복사 불가)
        printf("cp: '%s': 이미 존재하는 파일 이름 입니다.\n", fileName);
    } else {
        DirectoryNode *checkOverlap = p_directoryTree->current->LeftChild;
        while (checkOverlap != NULL) {
            if (strcmp(checkOverlap->name, command) == 0 && checkOverlap->type == fileNameNode->type) {
                break;
            }
            checkOverlap = checkOverlap->RightSibling;
        }
        if (checkOverlap != NULL) {  // 이름이 같은 파일이 존재하면 node트리에서 삭제함
            rm(p_directoryTree, command);
        }

        DirectoryNode *NewNode = (DirectoryNode *)malloc(sizeof(DirectoryNode));
        // initialize NewNode
        NewNode->LeftChild = NULL;
        NewNode->RightSibling = NULL;

        // get time
        time(&ltime);
        today = localtime(&ltime);

        // set NewNode
        strncpy(NewNode->name, command, MAX_NAME_SIZE);

        for (int i = 0; i < 9; i++) {
            NewNode->permission[i] = fileNameNode->permission[i];
        }
        NewNode->mode = fileNameNode->mode;
        NewNode->SIZE = fileNameNode->SIZE;
        NewNode->UID = fileNameNode->UID;
        NewNode->GID = fileNameNode->GID;
        NewNode->month = today->tm_mon + 1;
        NewNode->day = today->tm_mday;
        NewNode->hour = today->tm_hour;
        NewNode->minute = today->tm_min;
        NewNode->Parent = p_directoryTree->current;
        NewNode->type = fileNameNode->type;

        if (p_directoryTree->current->LeftChild == NULL) {
            p_directoryTree->current->LeftChild = NewNode;
        } else {
            tmpNode = p_directoryTree->current->LeftChild;

            while (tmpNode->RightSibling != NULL) {
                tmpNode = tmpNode->RightSibling;
            }
            tmpNode->RightSibling = NewNode;
        }
    }

    p_directoryTree->current = preNode;
    pthread_exit(NULL);
}
