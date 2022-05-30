#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "main.h"

int mkdir(DirectoryTree *p_directoryTree, char *command) {
    DirectoryNode *tmpNode = NULL;
    char *str;
    int isDirectoryExist;
    int tmpMode;

    if (command == NULL) {
        printf("mkdir: 잘못된 연산자\n");
        printf("Try 'mkdir --help' for more information.\n");
        return -1;
    }

    int t_count = 0;
    pthread_t t_command[MAX_THREAD_SIZE];
    ThreadArg p_threadArg[MAX_THREAD_SIZE];
    tmpNode = p_directoryTree->current;
    if (command[0] == '-') {
        if (strcmp(command, "-p") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }

            while (str != NULL) {
                printf("%s : str\n", str);
                p_threadArg[t_count].p_directoryTree = p_directoryTree;
                p_threadArg[t_count].additionalValue = ".";
                p_threadArg[t_count++].command = str;
                str = strtok(NULL, " ");
            }
        } else if (strcmp(command, "-m") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            if (str[0] - '0' < 8 && str[1] - '0' < 8 && str[2] - '0' < 8 && strlen(str) == 3) {
                tmpMode = atoi(str);
                char *modeStr = str;
                str = strtok(NULL, " ");
                if (str == NULL) {
                    printf("mkdir: 잘못된 연산자\n");
                    printf("Try 'mkdir --help' for more information.\n");
                    return -1;
                }

                while (str != NULL) {
                    p_threadArg[t_count].p_directoryTree = p_directoryTree;
                    p_threadArg[t_count].additionalValue = modeStr;
                    p_threadArg[t_count++].command = str;
                    str = strtok(NULL, " ");
                }
            } else {
                printf("mkdir: 잘못된 모드: '%s'\n", str);
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
        } else if (strcmp(command, "--help") == 0) {
            printf("사용법: mkdir [옵션]... 디렉터리...\n");
            printf("  Create the DIRECTORY(ies), if they do not already exists.\n\n");
            printf("  Options:\n");
            printf("    -m, --mode=MODE\t set file mode (as in chmod)\n");
            printf("    -p, --parents  \t no error if existing, make parent directories as needed\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            } else {
                printf("mkdir: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
        }
    } else {
        str = strtok(NULL, " ");

        p_threadArg[t_count].p_directoryTree = p_directoryTree;
        p_threadArg[t_count].additionalValue = NULL;
        p_threadArg[t_count++].command = command;

        while (str != NULL) {
            p_threadArg[t_count].p_directoryTree = p_directoryTree;
            p_threadArg[t_count].additionalValue = NULL;
            p_threadArg[t_count++].command = str;
            str = strtok(NULL, " ");
        }
    }

    for (int i = 0; i < t_count; i++) {  // 작업에 따라 담아 놓은 스레드 한번에 실행
        pthread_create(&t_command[i], NULL, thread_routine_make_directory, (void *)&p_threadArg[i]);
        pthread_join(t_command[i], NULL);
    }
    return 0;
}

int touch(DirectoryTree *p_directoryTree, char *command) {
    DirectoryNode *tmpNode = NULL;
    char *str;
    int isDirectoryExist;
    int tmpMode;

    if (command == NULL) {
        printf("touch: 잘못된 연산자\n");
        printf("Try 'touch --help' for more information.\n");
        return -1;
    }

    int t_count = 0;
    pthread_t t_command[MAX_THREAD_SIZE];
    ThreadArg p_threadArg[MAX_THREAD_SIZE];
    tmpNode = p_directoryTree->current;
    if (command[0] == '-') {
        if (strcmp(command, "-t") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("touch: 잘못된 연산자\n");
                printf("Try 'touch --help' for more information.\n");
                return -1;
            }

            char *time = str;
            str = strtok(NULL, " ");

            while (str != NULL) {
                p_threadArg[t_count].p_directoryTree = p_directoryTree;
                p_threadArg[t_count].additionalValue = time;
                p_threadArg[t_count++].command = str;
                str = strtok(NULL, " ");
            }

            p_directoryTree->current = tmpNode;
        } else if (strcmp(command, "-m") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("touch: 잘못된 연산자\n");
                printf("Try 'touch --help' for more information.\n");
                return -1;
            }

            while (str != NULL) {
                p_threadArg[t_count].p_directoryTree = p_directoryTree;
                p_threadArg[t_count].additionalValue = "M";
                p_threadArg[t_count++].command = str;
                str = strtok(NULL, " ");
            }

            p_directoryTree->current = tmpNode;
        } else if (strcmp(command, "--help") == 0) {
            printf("사용법: touch [옵션]... 파일...\n");
            printf("  Create the FILE(ies), if they do not already exists.\n\n");
            printf("  Options:\n");
            printf("    -t, --time=WORD\t chage the specified time\n");
            printf("    -m,   \t change only modification time\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("touch: 잘못된 연산자\n");
                printf("Try 'touch --help' for more information.\n");
                return -1;
            } else {
                printf("touch: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'touch --help' for more information.\n");
                return -1;
            }
        }
    } else {
        str = strtok(NULL, " ");
        p_threadArg[t_count].p_directoryTree = p_directoryTree;
        p_threadArg[t_count].additionalValue = NULL;
        p_threadArg[t_count++].command = command;

        while (str != NULL) {
            p_threadArg[t_count].p_directoryTree = p_directoryTree;
            p_threadArg[t_count].additionalValue = NULL;
            p_threadArg[t_count++].command = str;
            str = strtok(NULL, " ");
        }
    }

    for (int i = 0; i < t_count; i++) {
        pthread_create(&t_command[i], NULL, thread_routine_touch, (void *)&p_threadArg[i]);
        pthread_join(t_command[i], NULL);
    }
    return 0;
}

int cp(DirectoryTree *p_directoryTree, char *command) {
    DirectoryNode *tmpNode = NULL;
    char *str;
    int isDirectoryExist;
    int tmpMode;

    if (command == NULL) {
        printf("cp: 잘못된 연산자\n");
        printf("Try 'cp --help' for more information.\n");
        return -1;
    }

    int t_count = 0;
    char *fileName, *copyPath = NULL;
    pthread_t t_command[MAX_THREAD_SIZE];
    ThreadArg p_threadArg[MAX_THREAD_SIZE];
    tmpNode = p_directoryTree->current;
    if (command[0] == '-') {
        if (strcmp(command, "-r") == 0) {
            char *directoryName, *copyDirectoryName;
            str = directoryName = strtok(NULL, " ");
            if (str == NULL) {
                printf("cp: 잘못된 연산자\n");
                printf("Try 'cp --help' for more information.\n");
                return -1;
            }
            str = copyDirectoryName = strtok(NULL, " ");
            str = strtok(NULL, " ");

            if (str != NULL || copyDirectoryName == NULL) {
                printf("cp: 잘못된 입력 형식\n");
                printf("Try 'cp --help' for more information.\n");
                return -1;
            }
            DirectoryNode *directoryNode = is_exist_directory(p_directoryTree, directoryName, 'd');
            DirectoryNode *copyDirectoryNode = is_exist_directory(p_directoryTree, copyDirectoryName, 'd');

            if (directoryNode == NULL) {
                printf("cp: %s: 디렉토리는 존재하지 않습니다.\n", directoryName);
                printf("Try 'cp --help' for more information.\n");
                return -1;
            }

            copy_directory(p_directoryTree, directoryNode, copyDirectoryName, 'd', 1);

            p_directoryTree->current = tmpNode;
        } else if (strcmp(command, "--help") == 0) {
            printf("사용법: cp [옵션]... 파일/디렉터리...\n");
            printf("  Copy file or directory.\n\n");
            printf("  Options:\n");
            printf("    -r\t copy directory\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("cp: 잘못된 연산자\n");
                printf("Try 'cp --help' for more information.\n");
                return -1;
            } else {
                printf("cp: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'cp --help' for more information.\n");
                return -1;
            }
        }
    } else {
        fileName = command;
        str = strtok(NULL, " ");

        DirectoryNode *fileNameNode = p_directoryTree->current->LeftChild;
        while (fileNameNode != NULL) {
            if (strcmp(fileNameNode->name, fileName) == 0 && fileNameNode->type == 'f') {
                break;
            }
            fileNameNode = fileNameNode->RightSibling;
        }

        if (fileNameNode == NULL) {
            printf("cp: '%s': 그런 파일이 없습니다\n", fileName);
            return -1;
        }

        while (str != NULL) {
            if (strchr(str, '/') != NULL) {
                copyPath = str;
            } else {
                p_threadArg[t_count].p_directoryTree = p_directoryTree;
                p_threadArg[t_count].fileNameNode = fileNameNode;
                p_threadArg[t_count].additionalValue = fileName;  // fileName은 복사할 대상
                p_threadArg[t_count++].command = str;             // str은 복사한 파일 이름
            }
            str = strtok(NULL, " ");
        }

        if (copyPath) {
            int is_path_exist = move_directory_path(p_directoryTree, copyPath);
            if (is_path_exist == -1) {  // copyPath 경로에 디렉토리가 존재하지 않으면 종료
                printf("cp: %s 디렉토리는 존재하지 않습니다.\n", copyPath);
                printf("Try 'cp --help' for more information.\n");
                return -1;
            }
            p_directoryTree->current = tmpNode;
        }
    }

    for (int i = 0; i < t_count; i++) {
        p_threadArg[i].copyPath = copyPath ? copyPath : NULL;
        pthread_create(&t_command[i], NULL, thread_routine_copy, (void *)&p_threadArg[i]);
        pthread_join(t_command[i], NULL);
    }
    return 0;
}

int rm(DirectoryTree *p_directoryTree, char *command) {
    DirectoryNode *currentNode = NULL;
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;
    char *str;
    char tmp[MAX_DIRECTORY_SIZE];
    char tmp2[MAX_DIRECTORY_SIZE];
    char tmp3[MAX_DIRECTORY_SIZE];
    int isDirectoryExist;

    if (command == NULL) {
        printf("rm: 잘못된 연산자\n");
        printf("Try 'rm --help' for more information.\n");
        return -1;
    }
    currentNode = p_directoryTree->current;
    if (command[0] == '-') {
        if (strcmp(command, "-r") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("rm: 잘못된 연산자\n");
                printf("Try 'rm --help' for more information.\n");

                return -1;
            }
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL) {
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                if (tmpNode == NULL) {
                    printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", str);
                    return -1;
                } else {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0) {
                        printf("rm: '%s'디렉터리 또는 파일을 지울 수 없습니다: 허가거부\n", str);
                        return -1;
                    }
                    remove_directory(p_directoryTree, str);
                }
            } else {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                isDirectoryExist = move_directory_path(p_directoryTree, tmp2);
                if (isDirectoryExist != 0) {
                    printf("rm: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
                if (tmpNode == NULL) {
                    printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                } else {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0) {
                        printf("rm: '%s'디렉터리 또는 파일을 지울 수 없습니다: 허가거부\n", tmp3);
                        p_directoryTree->current = currentNode;
                        return -1;
                    }
                    remove_directory(p_directoryTree, tmp3);
                }
                p_directoryTree->current = currentNode;
            }
        } else if (strcmp(command, "-f") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                return -1;
            }
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL) {
                tmpNode = is_exist_directory(p_directoryTree, str, 'f');
                tmpNode2 = is_exist_directory(p_directoryTree, str, 'd');

                if (tmpNode2 != NULL) {
                    return -1;
                }
                if (tmpNode == NULL) {
                    return -1;
                } else {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0) {
                        return -1;
                    }
                    remove_directory(p_directoryTree, str);
                }
            } else {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                isDirectoryExist = move_directory_path(p_directoryTree, tmp2);
                if (isDirectoryExist != 0) {
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'f');
                tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'd');

                if (tmpNode2 != NULL) {
                    p_directoryTree->current = currentNode;
                    return -1;
                }
                if (tmpNode == NULL) {
                    p_directoryTree->current = currentNode;
                    return -1;
                } else {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0) {
                        p_directoryTree->current = currentNode;
                        return -1;
                    }
                    remove_directory(p_directoryTree, tmp3);
                }
                p_directoryTree->current = currentNode;
            }
        } else if (strcmp(command, "-rf") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                return -1;
            }
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL) {
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                if (tmpNode == NULL) {
                    return -1;
                } else {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0) {
                        return -1;
                    }
                    remove_directory(p_directoryTree, str);
                }
            } else {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                isDirectoryExist = move_directory_path(p_directoryTree, tmp2);
                if (isDirectoryExist != 0) {
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
                if (tmpNode == NULL) {
                    p_directoryTree->current = currentNode;
                    return -1;
                } else {
                    if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0) {
                        p_directoryTree->current = currentNode;
                        return -1;
                    }
                    remove_directory(p_directoryTree, tmp3);
                }
                p_directoryTree->current = currentNode;
            }
        } else if (strcmp(command, "--help") == 0) {
            printf("사용법: rm [<옵션>]... [<파일>]...\n");
            printf("  Remove (unlink) the FILE(s).\n\n");
            printf("  Options:\n");
            printf("    -f, --force    \t ignore nonexistent files and arguments, never prompt\n");
            printf("    -r, --recursive\t remove directories and their contents recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("rm: 잘못된 연산자\n");
                printf("Try 'rm --help' for more information.\n");
                return -1;
            } else {
                printf("rm: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'rm --help' for more information.\n");
                return -1;
            }
        }
    } else {
        strncpy(tmp, command, MAX_DIRECTORY_SIZE);
        if (strstr(command, "/") == NULL) {
            tmpNode = is_exist_directory(p_directoryTree, command, 'f');
            tmpNode2 = is_exist_directory(p_directoryTree, command, 'd');

            if (tmpNode == NULL && tmpNode2 != NULL) {
                printf("rm:'%s'를 지울 수 없음: 디렉터리입니다\n", command);
                return -1;
            }
            if (tmpNode == NULL) {
                printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", command);
                return -1;
            } else {
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0) {
                    printf("rm: '%s'디렉터리 또는 파일을 지울 수 없습니다: 허가거부\n", command);
                    return -1;
                }
                remove_directory(p_directoryTree, command);
            }
        } else {
            strncpy(tmp2, get_directory(command), MAX_DIRECTORY_SIZE);
            isDirectoryExist = move_directory_path(p_directoryTree, tmp2);
            if (isDirectoryExist != 0) {
                printf("rm: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME_SIZE);
                str = strtok(NULL, "/");
            }
            tmpNode = is_exist_directory(p_directoryTree, tmp3, 'f');
            tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'd');

            if (tmpNode2 != NULL) {
                printf("rm:'%s'를 지울 수 없음: 디렉터리입니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            }
            if (tmpNode == NULL) {
                printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            } else {
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0 || is_node_has_permission(tmpNode, 'w') != 0) {
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

int cd(DirectoryTree *p_directoryTree, char *command) {
    DirectoryNode *tmpNode = NULL;
    char *str = NULL;
    char tmp[MAX_DIRECTORY_SIZE];
    int isDirectoryExist;

    if (command == NULL) {
        strcpy(tmp, gp_userList->current->dir);
        move_directory_path(p_directoryTree, tmp);
    } else if (command[0] == '-') {
        if (strcmp(command, "--help") == 0) {
            printf("사용법: cd 디렉터리...\n");
            printf("  Change the shell working directory.\n\n");
            printf("  Options:\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("cd: 잘못된 연산자\n");
                printf("Try 'cd --help' for more information.\n");
                return -1;
            } else {
                printf("cd: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'cd --help' for more information.\n");

                return -1;
            }
        }
    } else {
        tmpNode = is_exist_directory(p_directoryTree, command, 'd');
        if (tmpNode != NULL) {
            if (is_node_has_permission(tmpNode, 'r') != 0) {
                printf("-bash: cd: '%s': 허가거부\n", command);
                return -1;
            }
        }
        if (tmpNode == NULL && is_exist_directory(p_directoryTree, command, 'f') != NULL) {
            printf("-bash: cd: '%s': 디렉터리가 아닙니다\n", command);
            return -1;
        }
        if (move_directory_path(p_directoryTree, command) != 0)
            printf("-bash: cd: '%s': 그런 파일이나 디렉터리가 없습니다\n", command);
    }
    return 0;
}

int pwd(DirectoryTree *p_directoryTree, Stack *p_directoryStack, char *command) {
    char *str = NULL;
    if (command == NULL) {
        print_directory_path(p_directoryTree, p_directoryStack);
    } else if (command[0] == '-') {
        if (strcmp(command, "--help") == 0) {
            printf("사용법: pwd\n");
            printf("  Print the name of the current working directory.\n\n");
            printf("  Options:\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("pwd: 잘못된 연산자\n");
                printf("Try 'cd --help' for more information.\n");
                return -1;
            } else {
                printf("pwd: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'pwd --help' for more information.\n");
                return -1;
            }
        }
    }

    return 0;
}

int ls(DirectoryTree *p_directoryTree, char *command) {
    DirectoryNode *tmpNode = NULL;
    char *str;
    int isDirectoryExist;

    if (command == NULL) {
        list_directory(p_directoryTree, 0, 0);
        return 0;
    }

    if (command[0] == '-') {
        if (strcmp(command, "-al") == 0 || strcmp(command, "-la") == 0) {
            str = strtok(NULL, " ");
            if (str != NULL) {
                tmpNode = p_directoryTree->current;
                isDirectoryExist = move_directory_path(p_directoryTree, str);
                if (isDirectoryExist != 0)
                    return -1;
            }
            list_directory(p_directoryTree, 1, 1);
        } else if (strcmp(command, "-l") == 0) {
            str = strtok(NULL, " ");
            if (str != NULL) {
                tmpNode = p_directoryTree->current;
                isDirectoryExist = move_directory_path(p_directoryTree, str);
                if (isDirectoryExist != 0)
                    return -1;
            }
            list_directory(p_directoryTree, 0, 1);
        } else if (strcmp(command, "-a") == 0) {
            str = strtok(NULL, " ");
            if (str != NULL) {
                tmpNode = p_directoryTree->current;
                isDirectoryExist = move_directory_path(p_directoryTree, str);
                if (isDirectoryExist != 0)
                    return -1;
            }
            list_directory(p_directoryTree, 1, 0);
        } else if (strcmp(command, "--help") == 0) {
            printf("사용법: ls [<옵션>]... [<파일>]...\n");
            printf("  List information about the FILEs (the current directory by default).\n\n");
            printf("  Options:\n");
            printf("    -a, --all\t do not ignore entries starting with .\n");
            printf("    -l       \t use a long listing format\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("ls: 잘못된 연산자\n");
                printf("Try 'ls --help' for more information.\n");
                return -1;
            } else {
                printf("ls: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'ls --help' for more information.\n");
                return -1;
            }
        }
    } else {
        tmpNode = p_directoryTree->current;
        isDirectoryExist = move_directory_path(p_directoryTree, command);
        if (isDirectoryExist != 0)
            return -1;
        list_directory(p_directoryTree, 0, 0);
        p_directoryTree->current = tmpNode;
    }

    if (str != NULL)
        p_directoryTree->current = tmpNode;

    return 0;
}

int cat(DirectoryTree *p_directoryTree, char *command) {
    DirectoryNode *currentNode = NULL;
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;
    char *str;
    char tmp[MAX_DIRECTORY_SIZE];
    char tmp2[MAX_DIRECTORY_SIZE];
    char tmp3[MAX_DIRECTORY_SIZE];
    int isDirectoryExist;

    /**
        cat0: write, EOF to save
        cat1: read
        cat2: read w/ line number
    **/

    if (command == NULL) {
        printf("cat: 잘못된 연산자\n");
        return -1;
    }
    currentNode = p_directoryTree->current;

    if (strcmp(command, ">") == 0) {
        str = strtok(NULL, " ");
        if (str == NULL) {
            printf("cat: 잘못된 연산자\n");
            printf("Try 'cat --help' for more information.\n");
            return -1;
        }
        strncpy(tmp, str, MAX_DIRECTORY_SIZE);
        if (strstr(str, "/") == NULL) {
            if (is_node_has_permission(p_directoryTree->current, 'w') != 0) {
                printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                return -1;
            }
            tmpNode = is_exist_directory(p_directoryTree, str, 'd');
            if (tmpNode != NULL) {
                printf("cat: '%s': 디렉터리입니다\n", str);
                return -1;
            } else {
                concatenate(p_directoryTree, str, 0);
            }
        } else {
            strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
            isDirectoryExist = move_directory_path(p_directoryTree, tmp2);
            if (isDirectoryExist != 0) {
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME_SIZE);
                str = strtok(NULL, "/");
            }
            if (is_node_has_permission(p_directoryTree->current, 'w') != 0) {
                printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                p_directoryTree->current = currentNode;
                return -1;
            }
            tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
            if (tmpNode != NULL) {
                printf("cat: '%s': 디렉터리입니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            } else {
                concatenate(p_directoryTree, tmp3, 0);
            }
            p_directoryTree->current = currentNode;
        }
        return 0;
    } else if (command[0] == '-') {
        if (strcmp(command, "-n") == 0) {
            str = strtok(NULL, " ");
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL) {
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0) {
                    printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                    return -1;
                }
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                tmpNode2 = is_exist_directory(p_directoryTree, str, 'f');

                if (tmpNode == NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", str);
                    return -1;
                } else if (tmpNode != NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': 디렉터리입니다\n", str);
                    return -1;
                } else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0) {
                    printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                    return -1;
                } else {
                    concatenate(p_directoryTree, str, 2);
                }
            } else {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                isDirectoryExist = move_directory_path(p_directoryTree, tmp2);
                if (isDirectoryExist != 0) {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
                tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'f');

                if (tmpNode == NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                } else if (tmpNode != NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': 디렉터리입니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                } else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0) {
                    printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                    p_directoryTree->current = currentNode;
                    return -1;
                } else {
                    concatenate(p_directoryTree, tmp3, 2);
                }
                p_directoryTree->current = currentNode;
            }
        } else if (strcmp(command, "-b") == 0) {
            str = strtok(NULL, " ");
            strncpy(tmp, str, MAX_DIRECTORY_SIZE);
            if (strstr(str, "/") == NULL) {
                if (is_node_has_permission(p_directoryTree->current, 'w') != 0) {
                    printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                    return -1;
                }
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                tmpNode2 = is_exist_directory(p_directoryTree, str, 'f');
                if (tmpNode == NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", str);
                    return -1;
                } else if (tmpNode != NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': 디렉터리입니다\n", str);
                    return -1;
                } else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0) {
                    printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                    return -1;
                } else {
                    concatenate(p_directoryTree, str, 3);
                }
            } else {
                strncpy(tmp2, get_directory(str), MAX_DIRECTORY_SIZE);
                isDirectoryExist = move_directory_path(p_directoryTree, tmp2);
                if (isDirectoryExist != 0) {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while (str != NULL) {
                    strncpy(tmp3, str, MAX_NAME_SIZE);
                    str = strtok(NULL, "/");
                }
                tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
                tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'f');
                if (tmpNode == NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                } else if (tmpNode != NULL && tmpNode2 == NULL) {
                    printf("cat: '%s': 디렉터리입니다\n", tmp3);
                    p_directoryTree->current = currentNode;
                    return -1;
                } else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0) {
                    printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                    p_directoryTree->current = currentNode;
                    return -1;
                } else {
                    concatenate(p_directoryTree, tmp3, 3);
                }
                p_directoryTree->current = currentNode;
            }
        } else if (strcmp(command, "--help") == 0) {
            printf("사용법: cat [<옵션>]... [<파일>]...\n");
            printf("  FILE(들)을 합쳐서 표준 출력으로 보낸다.\n\n");
            printf("  Options:\n");
            printf("    -n, --number         \t number all output line\n");
            printf("    -b, --number-nonblank\t number nonempty output line\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("cat: 잘못된 연산자\n");
                printf("Try 'cat --help' for more information.\n");
                return -1;
            } else {
                printf("cat: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'cat --help' for more information.\n");
                return -1;
            }
        }
    } else {
        if (strcmp(command, "/etc/passwd") == 0) {
            concatenate(p_directoryTree, command, 4);
            return 0;
        }

        strncpy(tmp, command, MAX_DIRECTORY_SIZE);
        if (strstr(command, "/") == NULL) {
            if (is_node_has_permission(p_directoryTree->current, 'w') != 0) {
                printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", p_directoryTree->current->name);
                return -1;
            }
            tmpNode = is_exist_directory(p_directoryTree, command, 'd');
            tmpNode2 = is_exist_directory(p_directoryTree, command, 'f');
            if (tmpNode == NULL && tmpNode2 == NULL) {
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", command);
                return -1;
            } else if (tmpNode != NULL && tmpNode2 == NULL) {
                printf("cat: '%s': 디렉터리입니다\n", command);
                return -1;
            } else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0) {
                printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                return -1;
            } else {
                concatenate(p_directoryTree, command, 1);
            }
        } else {
            strncpy(tmp2, get_directory(command), MAX_DIRECTORY_SIZE);
            isDirectoryExist = move_directory_path(p_directoryTree, tmp2);
            if (isDirectoryExist != 0) {
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME_SIZE);
                str = strtok(NULL, "/");
            }
            tmpNode = is_exist_directory(p_directoryTree, tmp3, 'd');
            tmpNode2 = is_exist_directory(p_directoryTree, tmp3, 'f');
            if (tmpNode == NULL && tmpNode2 == NULL) {
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            } else if (tmpNode != NULL && tmpNode2 == NULL) {
                printf("cat: '%s': 디렉터리입니다\n", tmp3);
                p_directoryTree->current = currentNode;
                return -1;
            } else if (tmpNode2 != NULL && is_node_has_permission(tmpNode2, 'r') != 0) {
                printf("cat: '%s'파일을 열 수 없음: 허가거부\n", tmpNode2->name);
                p_directoryTree->current = currentNode;
                return -1;
            } else {
                concatenate(p_directoryTree, tmp3, 1);
            }

            p_directoryTree->current = currentNode;
        }
    }
    return 1;
}

int chmod(DirectoryTree *p_directoryTree, char *command) {
    DirectoryNode *tmpNode = NULL;
    char *str;
    int tmp;

    if (command == NULL) {
        printf("chmod: 잘못된 연산자\n");
        printf("Try 'chmod --help' for more information.\n");
        return -1;
    }
    if (command[0] == '-') {
        if (strcmp(command, "-R") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("chmod: 잘못된 연산자\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            if (str[0] - '0' < 8 && str[1] - '0' < 8 && str[2] - '0' < 8 && strlen(str) == 3) {
                tmp = atoi(str);
                str = strtok(NULL, " ");
                if (str == NULL) {
                    printf("chmod: 잘못된 연산자\n");
                    printf("Try 'chmod --help' for more information.\n");
                    return -1;
                }
                tmpNode = is_exist_directory(p_directoryTree, str, 'd');
                if (tmpNode != NULL) {
                    if (tmpNode->LeftChild == NULL)
                        change_mode(p_directoryTree, tmp, str);
                    else {
                        change_mode(p_directoryTree, tmp, str);
                        change_all_mode(tmpNode->LeftChild, tmp);
                    }
                } else {
                    printf("chmod: '%s': 그런 파일이나 디렉터리가 없습니다\n", str);
                    return -1;
                }
            } else {
                printf("chmod: 잘못된 모드: '%s'\n", str);
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
        } else if (strcmp(command, "--help") == 0) {
            printf("사용법: chmod [옵션]... 8진수-MODE... 디렉터리...\n");
            printf("  Change the mode of each FILE to MODE.\n\n");
            printf("  Options:\n");
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("chmod: 잘못된 연산자\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            } else {
                printf("chmod: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
        }
    } else {
        if (command[0] - '0' < 8 && command[1] - '0' < 8 && command[2] - '0' < 8 && strlen(command) == 3) {
            tmp = atoi(command);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("chmod: 잘못된 연산자\n");
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            change_mode(p_directoryTree, tmp, str);
        } else {
            printf("chmod: 잘못된 모드: '%s'\n", command);
            printf("Try 'chmod --help' for more information.\n");
            return -1;
        }
    }
    return 0;
}

int find_(DirectoryTree *p_directoryTree, char *command) {
    char *str;
    if (command == NULL) {
        find_directory(p_directoryTree, p_directoryTree->current->name, 1);
        return 0;
    } else if (command[0] == '-') {
        if (strcmp(command, "-name") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("find: 잘못된 연산자\n");
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
            find_directory(p_directoryTree, str, 0);
        } else if (strcmp(command, "--help") == 0) {
            printf("사용법: find [<옵션>]... [<파일>]...\n");
            printf("\n");
            printf("  Options:\n");
            printf("    -name\t finds file by name\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        } else {
            str = strtok(command, "-");
            if (str == NULL) {
                printf("find: 잘못된 연산자\n");
                printf("Try 'find --help' for more information.\n");
                return -1;
            } else {
                printf("find: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
        }
    } else {
        find_directory(p_directoryTree, command, 1);
    }

    return 0;
}