#include "main.h"
#include <stdio.h>
#include <stdlib.h>

// command
int mkdir(DirectoryTree *p_directoryTree, char *command)
{
    DirectoryNode *tmpNode = NULL;
    char *str;
    char tmp[MAX_DIRECTORY_SIZE];
    char tmp2[MAX_DIRECTORY_SIZE];
    char tmp3[MAX_DIRECTORY_SIZE];
    int val;
    int tmpMode;
    if (command == NULL)
    {
        printf("mkdir: 잘못된 연산자\n");
        printf("Try 'mkdir --help' for more information.\n");
        return -1;
    }

    tmpNode = p_directoryTree->current;
    if (command[0] == '-')
    {
        if (strcmp(command, "-p") == 0)
        {
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            if (strncmp(str, "/", 1) == 0)
            {
                p_directoryTree->current = p_directoryTree->root;
            }
            str = strtok(str, "/");
            while (str != NULL)
            {
                val = move_current_tree(p_directoryTree, str);
                if (val != 0)
                {
                    make_directory(p_directoryTree, str, 'd');
                    move_current_tree(p_directoryTree, str);
                }
                str = strtok(NULL, "/");
            }
            p_directoryTree->current = tmpNode;
        }
        else if (strcmp(command, "-m") == 0)
        {
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            if (str[0] - '0' < 8 && str[1] - '0' < 8 && str[2] - '0' < 8 && strlen(str) == 3)
            {
                tmpMode = atoi(str);
                str = strtok(NULL, " ");
                if (str == NULL)
                {
                    printf("mkdir: 잘못된 연산자\n");
                    printf("Try 'mkdir --help' for more information.\n");
                    return -1;
                }
                val = make_directory(p_directoryTree, str, 'd');
                if (val == 0)
                {
                    tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                    tmpNode->mode = tmpMode;
                    mode_to_permission(tmpNode);
                }
            }
            else
            {
                printf("mkdir: 잘못된 모드: '%s'\n", str);
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
        }
        else if (strcmp(command, "--help") == 0)
        {
            printf("사용법: mkdir [옵션]... 디렉터리...\n");
            printf("  Create the DIRECTORY(ies), if they do not already exists.\n\n");
            printf("  Options:\n");
            printf("    -m, --mode=MODE\t set file mode (as in chmod)\n");
            printf("    -p, --parents  \t no error if existing, make parent directories as needed\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("mkdir: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
        }
    }
    else
    {
        str = strtok(NULL, " ");
        if (str == NULL)
        {
            strncpy(tmp, command, MAX_DIRECTORY_SIZE);
            if (strstr(command, "/") == NULL)
            {
                make_directory(p_directoryTree, command, 'd');
                return 0;
            }
            else
            {
                strncpy(tmp2, get_directory(command), MAX_DIRECTORY_SIZE);
                val = move_directory_path(p_directoryTree, tmp2);
                if (val != 0)
                {
                    printf("mkdir: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL)
                {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                make_directory(p_directoryTree, tmp3, 'd');
                p_directoryTree->current = tmpNode;
            }
        }
        else
        {
            DirectoryNode *NewNode = (DirectoryNode *)malloc(sizeof(DirectoryNode));
            DirectoryNode *tmpNode2 = NULL;
            int fd[2];
            int state = 0;
            pid_t pid;

            state = pipe(fd);
            if (state == -1)
            {
                printf("pipe() error\n");
                return -1;
            }

            state = sem_init(&semp, 1, 1);
            write(fd[0], NewNode, sizeof(DirectoryNode));
            pid = fork();
            if (pid == 0)
            {
                sleep(0.1);
                sem_wait(&semp);
                read(fd[1], NewNode, sizeof(DirectoryNode));
                // get time
                time(&ltime);
                today = localtime(&ltime);

                // initialize NewNode
                NewNode->LeftChild = NULL;
                NewNode->RightSibling = NULL;

                // set NewNode
                strncpy(NewNode->name, str, MAX_NAME_SIZE);
                if (str[0] == '.')
                {
                    NewNode->type = 'd';
                    // rwx------
                    NewNode->mode = 700;
                    NewNode->SIZE = 4096;
                }
                else
                {
                    NewNode->type = 'd';
                    // rwxr-xr-x
                    NewNode->mode = 755;
                    NewNode->SIZE = 4096;
                }
                mode_to_permission(NewNode);
                NewNode->UID = gp_userList->current->UID;
                NewNode->GID = gp_userList->current->GID;
                NewNode->month = today->tm_mon + 1;
                NewNode->day = today->tm_mday;
                NewNode->hour = today->tm_hour;
                NewNode->minute = today->tm_min;
                NewNode->Parent = NULL;

                write(fd[1], NewNode, sizeof(DirectoryNode));
                sem_post(&semp);
                exit(0);
            }
            else
            {
                make_directory(p_directoryTree, command, 'd');
                sleep(0.1);
                sem_wait(&semp);
                read(fd[0], NewNode, sizeof(DirectoryNode));
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0)
                {
                    printf("mkdir: '%s' 디렉터리를 만들 수 없습니다: 허가 거부\n", NewNode->name);
                    free(NewNode);
                    return -1;
                }
                if (strcmp(NewNode->name, ".") == 0 || strcmp(NewNode->name, "..") == 0)
                {
                    printf("mkdir: '%s' 디렉터리를 만들 수 없습니다\n", NewNode->name);
                    free(NewNode);
                    return -1;
                }
                tmpNode = is_exist_directory(p_directoryTree, NewNode->name, 'd');
                if (tmpNode != NULL && tmpNode->type == 'd')
                {
                    printf("mkdir: '%s' 디렉터리를 만들 수 없습니다: 파일이 존재합니다\n", NewNode->name);
                    free(NewNode);
                    return -1;
                }

                NewNode->Parent = p_directoryTree->current;

                if (p_directoryTree->current->LeftChild == NULL)
                {
                    p_directoryTree->current->LeftChild = NewNode;
                }
                else
                {
                    tmpNode = p_directoryTree->current->LeftChild;

                    while (tmpNode->RightSibling != NULL)
                    {
                        tmpNode = tmpNode->RightSibling;
                    }
                    tmpNode->RightSibling = NewNode;
                }

                sem_post(&semp);
            }
            sem_destroy(&semp);
        }
    }

    return 0;
}

int rm(DirectoryTree *p_directoryTree, char *command)
{
    DirectoryNode *currentNode = NULL;
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;
    char *str;
    char tmp[MAX_DIRECTORY_SIZE];
    char tmp2[MAX_DIRECTORY_SIZE];
    char tmp3[MAX_DIRECTORY_SIZE];
    int val;

    if (command == NULL)
    {
        printf("rm: 잘못된 연산자\n");
        printf("Try 'rm --help' for more information.\n");
        return -1;
    }
    currentNode = p_directoryTree->current;
    if (command[0] == '-')
    {
        if (strcmp(command, "-r") == 0)
        {
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                printf("rm: 잘못된 연산자\n");
                printf("Try 'rm --help' for more information.\n");

                return -1;
            }
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL)
            {
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                if (tmpNode == NULL)
                {
                    printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", str);
                    return -1;
                }
                else
                {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0)
                    {
                        printf("rm: '%s'디렉터리 또는 파일을 지울 수 없습니다: 허가거부\n", str);
                        return -1;
                    }
                    remove_directory(p_directoryTree, str);
                }
            }
            else
            {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                val = move_directory_path(p_directoryTree, tmp2);
                if (val != 0)
                {
                    printf("rm: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL)
                {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
                if (tmpNode == NULL)
                {
                    printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else
                {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0)
                    {
                        printf("rm: '%s'디렉터리 또는 파일을 지울 수 없습니다: 허가거부\n", tmp3);
                        p_directoryTree->current = currentNode;
                        return -1;
                    }
                    remove_directory(p_directoryTree, tmp3);
                }
                p_directoryTree->current = currentNode;
            }
        }
        else if (strcmp(command, "-f") == 0)
        {
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                return -1;
            }
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL)
            {
                tmpNode = is_exist_directory(p_directoryTree, str, 'f');
                tmpNode2 = is_exist_directory(p_directoryTree, str, 'd');

                if (tmpNode2 != NULL)
                {
                    return -1;
                }
                if (tmpNode == NULL)
                {
                    return -1;
                }
                else
                {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0)
                    {
                        return -1;
                    }
                    remove_directory(p_directoryTree, str);
                }
            }
            else
            {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                val = move_directory_path(p_directoryTree, tmp2);
                if (val != 0)
                {
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL)
                {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'f');
                tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'd');

                if (tmpNode2 != NULL)
                {
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                if (tmpNode == NULL)
                {
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else
                {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0)
                    {
                        p_directoryTree->current = currentNode;
                        return -1;
                    }
                    remove_directory(p_directoryTree, tmp3);
                }
                p_directoryTree->current = currentNode;
            }
        }
        else if (strcmp(command, "-rf") == 0)
        {
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                return -1;
            }
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL)
            {
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                if (tmpNode == NULL)
                {
                    return -1;
                }
                else
                {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0)
                    {
                        return -1;
                    }
                    remove_directory(p_directoryTree, str);
                }
            }
            else
            {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                val = move_directory_path(p_directoryTree, tmp2);
                if (val != 0)
                {
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL)
                {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
                if (tmpNode == NULL)
                {
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else
                {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0)
                    {
                        p_directoryTree->current = currentNode;
                        return -1;
                    }
                    remove_directory(p_directoryTree, tmp3);
                }
                p_directoryTree->current = currentNode;
            }
        }
        else if (strcmp(command, "--help") == 0)
        {
            printf("사용법: rm [<옵션>]... [<파일>]...\n");
            printf("  Remove (unlink) the FILE(s).\n\n");
            printf("  Options:\n");
            printf("    -f, --force    \t ignore nonexistent files and arguments, never prompt\n");
            printf("    -r, --recursive\t remove directories and their contents recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("rm: 잘못된 연산자\n");
                printf("Try 'rm --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("rm: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'rm --help' for more information.\n");
                return -1;
            }
        }
    }
    else
    {
        strncpy(tmp, command, MAX_DIRECTORY_SIZE);
        if (strstr(command, "/") == NULL)
        {
            tmpNode = is_exist_directory(p_directoryTree, command, 'f');
            tmpNode2 = is_exist_directory(p_directoryTree, command, 'd');

            if (tmpNode2 != NULL)
            {
                printf("rm:'%s'를 지울 수 없음: 디렉터리입니다\n", command);
                return -1;
            }
            if (tmpNode == NULL)
            {
                printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", command);
                return -1;
            }
            else
            {
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0)
                {
                    printf("rm: '%s'디렉터리 또는 파일을 지울 수 없습니다: 허가거부\n", command);
                    return -1;
                }
                remove_directory(p_directoryTree, command);
            }
        }
        else
        {
            strncpy(tmp2, get_directory(command), MAX_DIRECTORY_SIZE);
            val = move_directory_path(p_directoryTree, tmp2);
            if (val != 0)
            {
                printf("rm: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL)
            {
                strncpy(tmp3, str, MAX_NAME_SIZE);
                str = strtok(NULL, "/");
            }
            tmpNode = is_exist_directory(p_directoryTree, tmp3, 'f');
            tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'd');

            if (tmpNode2 != NULL)
            {
                printf("rm:'%s'를 지울 수 없음: 디렉터리입니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            }
            if (tmpNode == NULL)
            {
                printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            }
            else
            {
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0)
                {
                    printf("rm: '%s'디렉터리 또는 파일을 지울 수 없습니다: 허가거부\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                remove_directory(p_directoryTree, tmp3);
            }
            p_directoryTree->current = currentNode;
        }
    }

    return 0;
}

int cd(DirectoryTree *p_directoryTree, char *command)
{
    DirectoryNode *tmpNode = NULL;
    char *str = NULL;
    char tmp[MAX_DIRECTORY_SIZE];
    int val;

    if (command == NULL)
    {
        strcpy(tmp, gp_userList->current->dir);
        move_directory_path(p_directoryTree, tmp);
    }
    else if (command[0] == '-')
    {
        if (strcmp(command, "--help") == 0)
        {
            printf("사용법: cd 디렉터리...\n");
            printf("  Change the shell working directory.\n\n");
            printf("  Options:\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("cd: 잘못된 연산자\n");
                printf("Try 'cd --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("cd: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'cd --help' for more information.\n");

                return -1;
            }
        }
    }
    else
    {
        tmpNode = is_exist_directory(p_directoryTree, command, 'd');
        if (tmpNode != NULL)
        {
            if (is_node_has_permission(tmpNode, 'r') != 0)
            {
                printf("-bash: cd: '%s': 허가거부\n", command);
                return -1;
            }
        }
        tmpNode = is_exist_directory(p_directoryTree, command, 'f');
        if (tmpNode != NULL)
        {
            printf("-bash: cd: '%s': 디렉터리가 아닙니다\n", command);
            return -1;
        }
        val = move_directory_path(p_directoryTree, command);
        if (val != 0)
            printf("-bash: cd: '%s': 그런 파일이나 디렉터리가 없습니다\n", command);
    }
    return 0;
}

int pwd(DirectoryTree *p_directoryTree, Stack *p_directoryStack, char *command)
{
    char *str = NULL;
    if (command == NULL)
    {
        print_directory_path(p_directoryTree, p_directoryStack);
    }
    else if (command[0] == '-')
    {
        if (strcmp(command, "--help") == 0)
        {
            printf("사용법: pwd\n");
            printf("  Print the name of the current working directory.\n\n");
            printf("  Options:\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("pwd: 잘못된 연산자\n");
                printf("Try 'cd --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("pwd: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'pwd --help' for more information.\n");
                return -1;
            }
        }
    }

    return 0;
}

int ls(DirectoryTree *p_directoryTree, char *command)
{
    DirectoryNode *tmpNode = NULL;
    char *str;
    int val;

    if (command == NULL)
    {
        list_directory(p_directoryTree, 0, 0);
        return 0;
    }

    if (command[0] == '-')
    {
        if (strcmp(command, "-al") == 0 || strcmp(command, "-la") == 0)
        {
            str = strtok(NULL, " ");
            if (str != NULL)
            {
                tmpNode = p_directoryTree->current;
                val = move_directory_path(p_directoryTree, str);
                if (val != 0)
                    return -1;
            }
            list_directory(p_directoryTree, 1, 1);
        }
        else if (strcmp(command, "-l") == 0)
        {
            str = strtok(NULL, " ");
            if (str != NULL)
            {
                tmpNode = p_directoryTree->current;
                val = move_directory_path(p_directoryTree, str);
                if (val != 0)
                    return -1;
            }
            list_directory(p_directoryTree, 0, 1);
        }
        else if (strcmp(command, "-a") == 0)
        {
            str = strtok(NULL, " ");
            if (str != NULL)
            {
                tmpNode = p_directoryTree->current;
                val = move_directory_path(p_directoryTree, str);
                if (val != 0)
                    return -1;
            }
            list_directory(p_directoryTree, 1, 0);
        }
        else if (strcmp(command, "--help") == 0)
        {
            printf("사용법: ls [<옵션>]... [<파일>]...\n");
            printf("  List information about the FILEs (the current directory by default).\n\n");
            printf("  Options:\n");
            printf("    -a, --all\t do not ignore entries starting with .\n");
            printf("    -l       \t use a long listing format\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("ls: 잘못된 연산자\n");
                printf("Try 'ls --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("ls: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'ls --help' for more information.\n");
                return -1;
            }
        }
    }
    else
    {
        tmpNode = p_directoryTree->current;
        val = move_directory_path(p_directoryTree, command);
        if (val != 0)
            return -1;
        list_directory(p_directoryTree, 0, 0);
        p_directoryTree->current = tmpNode;
    }

    if (str != NULL)
        p_directoryTree->current = tmpNode;

    return 0;
}

int cat(DirectoryTree *p_directoryTree, char *command)
{
    DirectoryNode *currentNode = NULL;
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;
    char *str;
    char tmp[MAX_DIRECTORY_SIZE];
    char tmp2[MAX_DIRECTORY_SIZE];
    char tmp3[MAX_DIRECTORY_SIZE];
    int val;

    /**
        cat0: write, EOF to save
        cat1: read
        cat2: read w/ line number
    **/

    if (command == NULL)
    {
        printf("cat: 잘못된 연산자\n");
        return -1;
    }
    currentNode = p_directoryTree->current;

    if (strcmp(command, ">") == 0)
    {
        str = strtok(NULL, " ");
        if (str == NULL)
        {
            printf("cat: 잘못된 연산자\n");
            printf("Try 'cat --help' for more information.\n");
            return -1;
        }
        strncpy(tmp, str, MAX_DIRECTORY_SIZE);
        if (strstr(str, "/") == NULL)
        {
            if (is_node_has_permission(p_directoryTree->current, 'w') != 0)
            {
                printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                return -1;
            }
            tmpNode = is_exist_directory(p_directoryTree, str, 'd');
            if (tmpNode != NULL)
            {
                printf("cat: '%s': 디렉터리입니다\n", str);
                return -1;
            }
            else
            {
                concatenate(p_directoryTree, str, 0);
            }
        }
        else
        {
            strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
            val = move_directory_path(p_directoryTree, tmp2);
            if (val != 0)
            {
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL)
            {
                strncpy(tmp3, str, MAX_NAME_SIZE);
                str = strtok(NULL, "/");
            }
            if (is_node_has_permission(p_directoryTree->current, 'w') != 0)
            {
                printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                p_directoryTree->current = currentNode;
                return -1;
            }
            tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
            if (tmpNode != NULL)
            {
                printf("cat: '%s': 디렉터리입니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            }
            else
            {
                concatenate(p_directoryTree, tmp3, 0);
            }
            p_directoryTree->current = currentNode;
        }
        return 0;
    }
    else if (command[0] == '-')
    {
        if (strcmp(command, "-n") == 0)
        {
            str = strtok(NULL, " ");
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL)
            {
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0)
                {
                    printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                    return -1;
                }
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                tmpNode2 = is_exist_directory(p_directoryTree, str, 'f');

                if (tmpNode == NULL && tmpNode2 == NULL)
                {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", str);
                    return -1;
                }
                else if (tmpNode != NULL && tmpNode2 == NULL)
                {
                    printf("cat: '%s': 디렉터리입니다\n", str);
                    return -1;
                }
                else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0)
                {
                    printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                    return -1;
                }
                else
                {
                    concatenate(p_directoryTree, str, 2);
                }
            }
            else
            {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                val = move_directory_path(p_directoryTree, tmp2);
                if (val != 0)
                {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL)
                {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
                tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'f');

                if (tmpNode == NULL && tmpNode2 == NULL)
                {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else if (tmpNode != NULL && tmpNode2 == NULL)
                {
                    printf("cat: '%s': 디렉터리입니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0)
                {
                    printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else
                {
                    concatenate(p_directoryTree, tmp3, 2);
                }
                p_directoryTree->current = currentNode;
            }
        }
        else if (strcmp(command, "-b") == 0)
        {
            str = strtok(NULL, " ");
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL)
            {
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0)
                {
                    printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                    return -1;
                }
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                tmpNode2 = is_exist_directory(p_directoryTree, str, 'f');
                if (tmpNode == NULL && tmpNode2 == NULL)
                {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", str);
                    return -1;
                }
                else if (tmpNode != NULL && tmpNode2 == NULL)
                {
                    printf("cat: '%s': 디렉터리입니다\n", str);
                    return -1;
                }
                else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0)
                {
                    printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                    return -1;
                }
                else
                {
                    concatenate(p_directoryTree, str, 3);
                }
            }
            else
            {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                val = move_directory_path(p_directoryTree, tmp2);
                if (val != 0)
                {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL)
                {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
                tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'f');
                if (tmpNode == NULL && tmpNode2 == NULL)
                {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else if (tmpNode != NULL && tmpNode2 == NULL)
                {
                    printf("cat: '%s': 디렉터리입니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0)
                {
                    printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                else
                {
                    concatenate(p_directoryTree, tmp3, 3);
                }
                p_directoryTree->current = currentNode;
            }
        }
        else if (strcmp(command, "--help") == 0)
        {
            printf("사용법: cat [<옵션>]... [<파일>]...\n");
            printf("  FILE(들)을 합쳐서 표준 출력으로 보낸다.\n\n");
            printf("  Options:\n");
            printf("    -n, --number         \t number all output line\n");
            printf("    -b, --number-nonblank\t number nonempty output line\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("cat: 잘못된 연산자\n");
                printf("Try 'cat --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("cat: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'cat --help' for more information.\n");
                return -1;
            }
        }
    }
    else
    {
        if (strcmp(command, "/etc/passwd") == 0)
        {
            concatenate(p_directoryTree, command, 4);
            return 0;
        }

        strncpy(tmp, command, MAX_DIRECTORY_SIZE);
        if (strstr(command, "/") == NULL)
        {
            if (is_node_has_permission(p_directoryTree->current, 'w') != 0)
            {
                printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                return -1;
            }
            tmpNode = is_exist_directory(p_directoryTree, command, 'd');
            tmpNode2 = is_exist_directory(p_directoryTree, command, 'f');
            if (tmpNode == NULL && tmpNode2 == NULL)
            {
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", command);
                return -1;
            }
            else if (tmpNode != NULL && tmpNode2 == NULL)
            {
                printf("cat: '%s': 디렉터리입니다\n", command);
                return -1;
            }
            else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0)
            {
                printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                return -1;
            }
            else
            {
                concatenate(p_directoryTree, command, 1);
            }
        }
        else
        {
            strncpy(tmp2, get_directory(command), MAX_DIRECTORY_SIZE);
            val = move_directory_path(p_directoryTree, tmp2);
            if (val != 0)
            {
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL)
            {
                strncpy(tmp3, str, MAX_NAME_SIZE);
                str = strtok(NULL, "/");
            }
            tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
            tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'f');
            if (tmpNode == NULL && tmpNode2 == NULL)
            {
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            }
            else if (tmpNode != NULL && tmpNode2 == NULL)
            {
                printf("cat: '%s': 디렉터리입니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            }
            else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0)
            {
                printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                p_directoryTree->current = currentNode;
                return -1;
            }
            else
            {
                concatenate(p_directoryTree, tmp3, 1);
            }

            p_directoryTree->current = currentNode;
        }
    }
    return 1;
}

int chmod(DirectoryTree *p_directoryTree, char *command)
{
    DirectoryNode *tmpNode = NULL;
    char *str;
    int tmp;

    if (command == NULL)
    {
        printf("chmod: 잘못된 연산자\n");
        printf("Try 'chmod --help' for more information.\n");
        return -1;
    }
    if (command[0] == '-')
    {
        if (strcmp(command, "-R") == 0)
        {
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                printf("chmod: 잘못된 연산자\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            if (str[0] - '0' < 8 && str[1] - '0' < 8 && str[2] - '0' < 8 && strlen(str) == 3)
            {
                tmp = atoi(str);
                str = strtok(NULL, " ");
                if (str == NULL)
                {
                    printf("chmod: 잘못된 연산자\n");
                    printf("Try 'chmod --help' for more information.\n");
                    return -1;
                }
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                if (tmpNode != NULL)
                {
                    if (tmpNode->LeftChild == NULL)
                        change_mode(p_directoryTree, tmp, str);
                    else
                    {
                        change_mode(p_directoryTree, tmp, str);
                        change_all_mode(tmpNode->LeftChild, tmp);
                    }
                }
                else
                {
                    printf("chmod: '%s': 그런 파일이나 디렉터리가 없습니다\n", str);
                    return -1;
                }
            }
            else
            {
                printf("chmod: 잘못된 모드: '%s'\n", str);
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
        }
        else if (strcmp(command, "--help") == 0)
        {
            printf("사용법: chmod [옵션]... 8진수-MODE... 디렉터리...\n");
            printf("  Change the mode of each FILE to MODE.\n\n");
            printf("  Options:\n");
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("chmod: 잘못된 연산자\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("chmod: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
        }
    }
    else
    {
        if (command[0] - '0' < 8 && command[1] - '0' < 8 && command[2] - '0' < 8 && strlen(command) == 3)
        {
            tmp = atoi(command);
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                printf("chmod: 잘못된 연산자\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            change_mode(p_directoryTree, tmp, str);
        }
        else
        {
            printf("chmod: 잘못된 모드: '%s'\n", command);
            printf("Try 'chmod --help' for more information.\n");
            return -1;
        }
    }
    return 0;
}

int chown_(DirectoryTree *p_directoryTree, char *command)
{
    DirectoryNode *tmpNode = NULL;
    UserNode *tmpUser = NULL;
    char *str;
    char tmp[MAX_NAME_SIZE];

    if (command == NULL)
    {
        printf("chown: 잘못된 연산자\n");
        printf("Try 'chown --help' for more information.\n");
        return -1;
    }
    if (command[0] == '-')
    {
        if (strcmp(command, "-R") == 0)
        {
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                printf("chown: 잘못된 연산자\n");
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
            tmpUser = is_exist_user(gp_userList, str);
            if (tmpUser != NULL)
            {
                strncpy(tmp, str, MAX_NAME_SIZE);
            }
            else
            {
                printf("chown: 잘못된 사용자: '%s'\n", str);
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                printf("chown: 잘못된 연산자\n");
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
            tmpNode = is_exist_directory(p_directoryTree, str, 'd');
            if (tmpNode != NULL)
            {
                if (tmpNode->LeftChild == NULL)
                    change_owner(p_directoryTree, tmp, str);
                else
                {
                    change_owner(p_directoryTree, tmp, str);
                    change_all_owner(tmpNode->LeftChild, tmp);
                }
            }
            else
            {
                printf("chown: '%s': 그런 파일이나 디렉터리가 없습니다\n", str);
                return -1;
            }
        }
        else if (strcmp(command, "--help") == 0)
        {
            printf("사용법: chown [옵션]... [소유자]... 파일...\n");
            printf("  Change the owner and/or group of each FILE to OWNER and/or GROUP.\n\n");
            printf("  Options:\n");
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("chown: 잘못된 연산자\n");
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("chown: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'chown --help' for more information.\n");
                return -1;
            }
        }
    }
    else
    {
        strncpy(tmp, command, MAX_NAME_SIZE);
        str = strtok(NULL, " ");
        if (str == NULL)
        {
            printf("chown: 잘못된 연산자\n");
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
        else
        {
            change_owner(p_directoryTree, tmp, str);
        }
    }
    return 0;
}

int find_(DirectoryTree *p_directoryTree, char *command)
{
    char *str;
    if (command == NULL)
    {
        find_directory(p_directoryTree, p_directoryTree->current->name, 1);
        return 0;
    }
    else if (command[0] == '-')
    {
        if (strcmp(command, "-name") == 0)
        {
            str = strtok(NULL, " ");
            if (str == NULL)
            {
                printf("find: 잘못된 연산자\n");
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
            find_directory(p_directoryTree, str, 0);
        }
        else if (strcmp(command, "--help") == 0)
        {
            printf("사용법: find [<옵션>]... [<파일>]...\n");
            printf("\n");
            printf("  Options:\n");
            printf("    -name\t finds file by name\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else
        {
            str = strtok(command, "-");
            if (str == NULL)
            {
                printf("find: 잘못된 연산자\n");
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
            else
            {
                printf("find: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
        }
    }
    else
    {
        find_directory(p_directoryTree, command, 1);
    }

    return 0;
}

void instruction(DirectoryTree *p_directoryTree, char *command)
{
    char *str;
    int val;
    if (strcmp(command, "") == 0 || command[0] == ' ')
    {
        return;
    }
    str = strtok(command, " ");

    if (strcmp(str, "mkdir") == 0)
    {
        str = strtok(NULL, " ");
        val = mkdir(p_directoryTree, str);
        if (val == 0)
        {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    }
    else if (strcmp(str, "rm") == 0)
    {
        str = strtok(NULL, " ");
        val = rm(p_directoryTree, str);
        if (val == 0)
        {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    }
    else if (strcmp(str, "cd") == 0)
    {
        str = strtok(NULL, " ");
        cd(p_directoryTree, str);
    }
    else if (strcmp(str, "pwd") == 0)
    {
        str = strtok(NULL, " ");
        pwd(p_directoryTree, gp_directoryStack, str);
    }
    else if (strcmp(str, "ls") == 0)
    {
        str = strtok(NULL, " ");
        ls(p_directoryTree, str);
    }
    else if (strcmp(str, "cat") == 0)
    {
        str = strtok(NULL, " ");
        val = cat(p_directoryTree, str);
        if (val == 0)
        {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    }
    else if (strcmp(str, "chmod") == 0)
    {
        str = strtok(NULL, " ");
        val = chmod(p_directoryTree, str);
        if (val == 0)
        {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    }
    else if (strcmp(str, "chown") == 0)
    {
        str = strtok(NULL, " ");
        val = chown_(p_directoryTree, str);
        if (val == 0)
        {
            save_directory(p_directoryTree, gp_directoryStack);
        }
    }
    else if (strcmp(str, "find") == 0)
    {
        str = strtok(NULL, " ");
        find_(p_directoryTree, str);
    }
    else if (strcmp(command, "exit") == 0)
    {
        printf("로그아웃\n");
        exit(0);
    }
    else
    {
        printf("'%s': 명령을 찾을 수 없습니다\n", command);
    }
    return;
}

void print_start()
{
    // printf("Welcome to Ubuntu 18.04.2 LTS (GNU/Linux 4.18.0-17-generic x86_64)\n\n");
    // printf(" * Documentation: https://help.ubuntu.com\n");
    // printf(" * Management:    https://landscape.canonial.com\n");
    // printf(" * Support:       https://ubuntu.com/advantage\n\n\n");
    // printf(" * Canonial Livepatch is available for installation.\n");
    // printf("   - Reduce system reboots and improve kernel security. Activate at:\n");
    // printf("     https://ubuntu.com/livepatch\n\n");
    // printf("Your Hardware Enablement Stack(HWE) is supported until April 2023.\n");
    printf("Welcome to miniOS 1.0.0\n\n");

    printf("Last login: ");
    get_weekday(gp_userList->current->wday);
    get_month(gp_userList->current->month);
    printf("%d %02d:%02d:%02d %d\n", gp_userList->current->day, gp_userList->current->hour, gp_userList->current->minute, gp_userList->current->sec, gp_userList->current->year);
}

void print_head(DirectoryTree *p_directoryTree, Stack *p_directoryStack)
{
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
    printf("%s@miniOS", gp_userList->current->name);
    DEFAULT;
    printf(":");
    tmpNode = p_directoryTree->current;

    if (tmpNode == p_directoryTree->root)
    {
        strcpy(tmp, "/");
    }
    else
    {
        while (tmpNode->Parent != NULL)
        {
            push(p_directoryStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        while (is_empty(p_directoryStack) == 0)
        {
            strcat(tmp, "/");
            strcat(tmp, pop(p_directoryStack));
        }
    }

    strncpy(tmp2, tmp, strlen(gp_userList->current->dir));

    if (gp_userList->current == gp_userList->head)
    {
        BOLD;
        BLUE;
        printf("%s", tmp);
    }
    else if (strcmp(gp_userList->current->dir, tmp2) != 0)
    {
        BOLD;
        BLUE;
        printf("%s", tmp);
    }
    else
    {
        tmpNode = p_directoryTree->current;
        while (tmpNode->Parent != NULL)
        {
            push(p_directoryStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        pop(p_directoryStack);
        pop(p_directoryStack);
        BOLD;
        BLUE;
        printf("~");
        while (is_empty(p_directoryStack) == 0)
        {
            printf("/");
            printf("%s", pop(p_directoryStack));
        }
    }
    DEFAULT;
    printf("%c ", usr);
}
