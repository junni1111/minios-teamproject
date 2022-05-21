#include "main.h"
#include <stdio.h>
#include <stdlib.h>

// utility
int mode_to_permission(DirectoryNode *dirNode)
{
    char buf[4];
    int tmp;
    int j;

    for (int i = 0; i < 9; i++)
        dirNode->permission[i] = 0;

    sprintf(buf, "%d", dirNode->mode);

    for (int i = 0; i < 3; i++)
    {
        tmp = buf[i] - '0';
        j = 2;

        while (tmp != 0)
        {
            dirNode->permission[3 * i + j] = tmp % 2;
            tmp /= 2;
            j--;
        }
    }

    return 0;
}

void print_permission(DirectoryNode *dirNode)
{
    char rwx[4] = "rwx";

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (dirNode->permission[3 * i + j] == 1)
                printf("%c", rwx[j]);
            else
                printf("-");
        }
    }
}

void destory_node(DirectoryNode *dirNode)
{
    free(dirNode);
}

void destory_directory(DirectoryNode *dirNode)
{
    if (dirNode->RightSibling != NULL)
    {
        destory_directory(dirNode->RightSibling);
    }
    if (dirNode->LeftChild != NULL)
    {
        destory_directory(dirNode->LeftChild);
    }

    dirNode->LeftChild = NULL;
    dirNode->RightSibling = NULL;

    destory_node(dirNode);
}

DirectoryNode *is_exist_directory(DirectoryTree *dirTree, char *dirName, char type)
{
    // variables
    DirectoryNode *returnNode = NULL;

    returnNode = dirTree->current->LeftChild;

    while (returnNode != NULL)
    {
        if (strcmp(returnNode->name, dirName) == 0 && returnNode->type == type)
            break;
        returnNode = returnNode->RightSibling;
    }

    return returnNode;
}

char *get_directory(char *dirPath)
{
    char *tmpPath = (char *)malloc(MAX_DIRECTORY_SIZE);
    char *str = NULL;
    char tmp[MAX_DIRECTORY_SIZE];
    char tmp2[MAX_DIRECTORY_SIZE];

    strncpy(tmp, dirPath, MAX_DIRECTORY_SIZE);
    str = strtok(dirPath, "/");
    while (str != NULL)
    {
        strncpy(tmp2, str, MAX_DIRECTORY_SIZE);
        str = strtok(NULL, "/");
    }
    strncpy(tmpPath, tmp, strlen(tmp) - strlen(tmp2) - 1);
    tmpPath[strlen(tmp) - strlen(tmp2) - 1] = '\0';

    return tmpPath;
}

// save & load
void get_directory_path(DirectoryTree *dirTree, DirectoryNode *dirNode, Stack *dirStack)
{
    // variables
    DirectoryNode *tmpNode = NULL;
    char tmp[MAX_DIRECTORY_SIZE] = "";

    tmpNode = dirNode->Parent;

    if (tmpNode == dirTree->root)
    {
        strcpy(tmp, "/");
    }
    else
    {
        // until current directory is root, repeat Push
        while (tmpNode->Parent != NULL)
        {
            push(dirStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        // until stack is empty, repeat Pop
        while (is_empty(dirStack) == 0)
        {
            strcat(tmp, "/");
            strcat(tmp, pop(dirStack));
        }
    }
    // return path;
    fprintf(f_directory, " %s\n", tmp);
}

void write_directory_node(DirectoryTree *dirTree, DirectoryNode *dirNode, Stack *dirStack)
{
    fprintf(f_directory, "%s %c %d ", dirNode->name, dirNode->type, dirNode->mode);
    fprintf(f_directory, "%d %d %d %d %d %d %d", dirNode->SIZE, dirNode->UID, dirNode->GID, dirNode->month, dirNode->day, dirNode->hour, dirNode->minute);

    if (dirNode == dirTree->root)
        fprintf(f_directory, "\n");
    else
        get_directory_path(dirTree, dirNode, dirStack);

    if (dirNode->RightSibling != NULL)
    {
        write_directory_node(dirTree, dirNode->RightSibling, dirStack);
    }
    if (dirNode->LeftChild != NULL)
    {
        write_directory_node(dirTree, dirNode->LeftChild, dirStack);
    }
}

void save_directory(DirectoryTree *dirTree, Stack *dirStack)
{

    f_directory = fopen("./resources/Directory.txt", "w");

    write_directory_node(dirTree, dirTree->root, dirStack);

    fclose(f_directory);
}

int read_directory_node(DirectoryTree *dirTree, char *tmp)
{
    DirectoryNode *NewNode = (DirectoryNode *)malloc(sizeof(DirectoryNode));
    DirectoryNode *tmpNode = NULL;
    char *str;

    NewNode->LeftChild = NULL;
    NewNode->RightSibling = NULL;
    NewNode->Parent = NULL;

    str = strtok(tmp, " ");
    strncpy(NewNode->name, str, MAX_NAME_SIZE);
    str = strtok(NULL, " ");
    NewNode->type = str[0];
    str = strtok(NULL, " ");
    NewNode->mode = atoi(str);
    mode_to_permission(NewNode);
    str = strtok(NULL, " ");
    NewNode->SIZE = atoi(str);
    str = strtok(NULL, " ");
    NewNode->UID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->GID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->month = atoi(str);
    str = strtok(NULL, " ");
    NewNode->day = atoi(str);
    str = strtok(NULL, " ");
    NewNode->hour = atoi(str);
    str = strtok(NULL, " ");
    NewNode->minute = atoi(str);

    str = strtok(NULL, " ");
    if (str != NULL)
    {
        str[strlen(str) - 1] = '\0';
        move_directory_path(dirTree, str);
        NewNode->Parent = dirTree->current;

        if (dirTree->current->LeftChild == NULL)
        {
            dirTree->current->LeftChild = NewNode;
        }
        else
        {
            tmpNode = dirTree->current->LeftChild;

            while (tmpNode->RightSibling != NULL)
                tmpNode = tmpNode->RightSibling;

            tmpNode->RightSibling = NewNode;
        }
    }
    else
    {
        dirTree->root = NewNode;
        dirTree->current = dirTree->root;
    }

    return 0;
}

DirectoryTree *load_directory()
{
    DirectoryTree *dirTree = (DirectoryTree *)malloc(sizeof(DirectoryTree));
    char tmp[MAX_LENGTH_SIZE];

    f_directory = fopen("./resources/Directory.txt", "r");

    while (fgets(tmp, MAX_LENGTH_SIZE, f_directory) != NULL)
    {
        read_directory_node(dirTree, tmp);
    }

    fclose(f_directory);

    dirTree->current = dirTree->root;

    return dirTree;
}

// mkdir
DirectoryTree *initialize_directory_tree()
{
    // variables
    DirectoryTree *dirTree = (DirectoryTree *)malloc(sizeof(DirectoryTree));
    DirectoryNode *NewNode = (DirectoryNode *)malloc(sizeof(DirectoryNode));
    // get time
    time(&ltime);
    today = localtime(&ltime);
    // set NewNode
    strncpy(NewNode->name, "/", MAX_NAME_SIZE);
    // rwxr-xr-x
    NewNode->type = 'd';
    NewNode->mode = 755;
    mode_to_permission(NewNode);
    NewNode->UID = gp_userList->head->UID;
    NewNode->GID = gp_userList->head->GID;
    NewNode->SIZE = 4096;
    NewNode->month = today->tm_mon + 1;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->minute = today->tm_min;
    NewNode->Parent = NULL;
    NewNode->LeftChild = NULL;
    NewNode->RightSibling = NULL;

    // set dirTree
    dirTree->root = NewNode;
    dirTree->current = dirTree->root;

    return dirTree;
}
// type==0: folder, type==1: file
int make_directory(DirectoryTree *dirTree, char *dirName, char type)
{
    // variables
    DirectoryNode *NewNode = (DirectoryNode *)malloc(sizeof(DirectoryNode));
    DirectoryNode *tmpNode = NULL;

    if (is_node_has_permission(dirTree->current, 'w') != 0)
    {
        printf("mkdir: '%s' 디렉터리를 만들 수 없습니다: 허가 거부\n", dirName);
        free(NewNode);
        return -1;
    }
    if (strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0)
    {
        printf("mkdir: '%s' 디렉터리를 만들 수 없습니다\n", dirName);
        free(NewNode);
        return -1;
    }
    tmpNode = is_exist_directory(dirTree, dirName, type);
    if (tmpNode != NULL && tmpNode->type == 'd')
    {
        printf("mkdir: '%s' 디렉터리를 만들 수 없습니다: 파일이 존재합니다\n", dirName);
        free(NewNode);
        return -1;
    }
    // get time
    time(&ltime);
    today = localtime(&ltime);

    // initialize NewNode
    NewNode->LeftChild = NULL;
    NewNode->RightSibling = NULL;

    // set NewNode
    strncpy(NewNode->name, dirName, MAX_NAME_SIZE);
    if (dirName[0] == '.')
    {
        NewNode->type = 'd';
        // rwx------
        NewNode->mode = 700;
        NewNode->SIZE = 4096;
    }
    else if (type == 'd')
    {
        NewNode->type = 'd';
        // rwxr-xr-x
        NewNode->mode = 755;
        NewNode->SIZE = 4096;
    }
    else
    {
        NewNode->type = 'f';
        // rw-r--r--
        NewNode->mode = 644;
        NewNode->SIZE = 0;
    }
    mode_to_permission(NewNode);
    NewNode->UID = gp_userList->current->UID;
    NewNode->GID = gp_userList->current->GID;
    NewNode->month = today->tm_mon + 1;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->minute = today->tm_min;
    NewNode->Parent = dirTree->current;

    if (dirTree->current->LeftChild == NULL)
    {
        dirTree->current->LeftChild = NewNode;
    }
    else
    {
        tmpNode = dirTree->current->LeftChild;

        while (tmpNode->RightSibling != NULL)
        {
            tmpNode = tmpNode->RightSibling;
        }
        tmpNode->RightSibling = NewNode;
    }

    return 0;
}

// rm
int remove_directory(DirectoryTree *dirTree, char *dirName)
{
    DirectoryNode *DelNode = NULL;
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *prevNode = NULL;

    tmpNode = dirTree->current->LeftChild;

    if (tmpNode == NULL)
    {
        printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", dirName);
        return -1;
    }

    if (strcmp(tmpNode->name, dirName) == 0)
    {
        dirTree->current->LeftChild = tmpNode->RightSibling;
        DelNode = tmpNode;
        if (DelNode->LeftChild != NULL)
            destory_directory(DelNode->LeftChild);
        destory_node(DelNode);
    }
    else
    {
        while (tmpNode != NULL)
        {
            if (strcmp(tmpNode->name, dirName) == 0)
            {
                DelNode = tmpNode;
                break;
            }
            prevNode = tmpNode;
            tmpNode = tmpNode->RightSibling;
        }
        if (DelNode != NULL)
        {
            prevNode->RightSibling = DelNode->RightSibling;

            if (DelNode->LeftChild != NULL)
                destory_directory(DelNode->LeftChild);
            destory_node(DelNode);
        }
        else
        {
            printf("rm: '%s'를 지울 수 없음: 그런 파일이나 디렉터리가 없습니다\n", dirName);
            return -1;
        }
    }
    return 0;
}

// cd
int move_current_tree(DirectoryTree *dirTree, char *dirPath)
{
    DirectoryNode *tmpNode = NULL;

    if (strcmp(dirPath, ".") == 0)
    {
    }
    else if (strcmp(dirPath, "..") == 0)
    {
        if (dirTree->current != dirTree->root)
        {
            dirTree->current = dirTree->current->Parent;
        }
    }
    else
    {

        // if input path exist
        tmpNode = is_exist_directory(dirTree, dirPath, 'd');
        if (tmpNode != NULL)
        {
            dirTree->current = tmpNode;
        }
        else
            return -1;
    }
    return 0;
}

int move_directory_path(DirectoryTree *dirTree, char *dirPath)
{
    // variables
    DirectoryNode *tmpNode = NULL;
    char tmpPath[MAX_DIRECTORY_SIZE];
    char *str = NULL;
    int val = 0;

    // set tmp
    strncpy(tmpPath, dirPath, MAX_DIRECTORY_SIZE);
    tmpNode = dirTree->current;
    // if input is root
    if (strcmp(dirPath, "/") == 0)
    {
        dirTree->current = dirTree->root;
    }
    else
    {
        // if input is absolute path
        if (strncmp(dirPath, "/", 1) == 0)
        {
            if (strtok(dirPath, "/") == NULL)
            {
                return -1;
            }
            dirTree->current = dirTree->root;
        }
        // if input is relative path
        str = strtok(tmpPath, "/");
        while (str != NULL)
        {
            val = move_current_tree(dirTree, str);
            // if input path doesn't exist
            if (val != 0)
            {
                dirTree->current = tmpNode;
                return -1;
            }
            str = strtok(NULL, "/");
        }
    }
    return 0;
}

// pwd
int print_directory_path(DirectoryTree *dirTree, Stack *dirStack)
{
    // variables
    DirectoryNode *tmpNode = NULL;

    tmpNode = dirTree->current;

    // if current directory is root
    if (tmpNode == dirTree->root)
    {
        printf("/");
    }
    else
    {
        // until current directory is root, repeat Push
        while (tmpNode->Parent != NULL)
        {
            push(dirStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        // until stack is empty, repeat Pop
        while (is_empty(dirStack) == 0)
        {
            printf("/");
            printf("%s", pop(dirStack));
        }
    }
    printf("\n");

    return 0;
}

// ls
int list_directory(DirectoryTree *dirTree, int a, int l)
{
    // variables
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;
    char type;
    int cnt;

    tmpNode = dirTree->current->LeftChild;

    if (is_node_has_permission(dirTree->current, 'r') != 0)
    {
        printf("ls: '%s'디렉터리를 열 수 없음: 허가거부\n", dirTree->current->name);
        return -1;
    }

    if (l == 0)
    {
        if (a == 0)
        {
            if (tmpNode == NULL)
            {
                return -1;
            }
        }
        if (a == 1)
        {
            BOLD;
            BLUE;
            printf(".\t");
            DEFAULT;
            if (dirTree->current != dirTree->root)
            {
                BOLD;
                BLUE;
                printf("..\t");
                DEFAULT;
            }
        }
        while (tmpNode != NULL)
        {
            if (a == 0)
            {
                if (strncmp(tmpNode->name, ".", 1) == 0)
                {
                    tmpNode = tmpNode->RightSibling;
                    continue;
                }
            }
            if (tmpNode->type == 'd')
            {
                BOLD;
                BLUE;
                printf("%s\t", tmpNode->name);
                DEFAULT;
            }
            else
                printf("%s\t", tmpNode->name);

            tmpNode = tmpNode->RightSibling;
        }
        printf("\n");
    }
    else
    {
        if (a == 0)
        {
            if (tmpNode == NULL)
            {
                printf("합계: 0\n");
                return -1;
            }
        }
        if (a == 1)
        {
            tmpNode2 = dirTree->current->LeftChild;
            if (tmpNode2 == NULL)
            {
                cnt = 2;
            }
            else
            {
                if (tmpNode2->type == 'd')
                    cnt = 3;
                else
                    cnt = 2;

                while (tmpNode2->RightSibling != NULL)
                {
                    tmpNode2 = tmpNode2->RightSibling;
                    if (tmpNode2->type == 'd')
                        cnt = cnt + 1;
                }
            }

            printf("%c", dirTree->current->type);
            print_permission(dirTree->current);
            printf("%3d", cnt);
            printf("   ");
            printf("%-5s%-5s", get_UID(dirTree->current), get_GID(dirTree->current));
            printf("%5d ", dirTree->current->SIZE);
            printf("%d월 %2d %02d:%02d ", dirTree->current->month, dirTree->current->day, dirTree->current->hour, dirTree->current->minute);
            BOLD;
            BLUE;
            printf(".\n");
            DEFAULT;

            if (dirTree->current != dirTree->root)
            {
                tmpNode2 = dirTree->current->Parent->LeftChild;
                if (tmpNode2 == NULL)
                {
                    cnt = 2;
                }
                else
                {
                    if (tmpNode2->type == 'd')
                        cnt = 3;
                    else
                        cnt = 2;

                    while (tmpNode2->RightSibling != NULL)
                    {
                        tmpNode2 = tmpNode2->RightSibling;
                        if (tmpNode2->type == 'd')
                            cnt = cnt + 1;
                    }
                }
                printf("%c", dirTree->current->Parent->type);
                print_permission(dirTree->current->Parent);
                printf("%3d", cnt);
                printf("   ");
                printf("%-5s%-5s", get_UID(dirTree->current->Parent), get_GID(dirTree->current->Parent));
                printf("%5d ", dirTree->current->SIZE);
                printf("%d월 %2d %02d:%02d ", dirTree->current->Parent->month, dirTree->current->Parent->day, dirTree->current->Parent->hour, dirTree->current->Parent->minute);
                BOLD;
                BLUE;
                printf("..\n");
                DEFAULT;
            }
        }

        while (tmpNode != NULL)
        {
            if (a == 0)
            {
                if (strncmp(tmpNode->name, ".", 1) == 0)
                {
                    tmpNode = tmpNode->RightSibling;
                    continue;
                }
            }
            tmpNode2 = tmpNode->LeftChild;
            if (tmpNode2 == NULL)
            {
                if (tmpNode->type == 'd')
                    cnt = 2;
                else
                    cnt = 1;
            }
            else
            {
                if (tmpNode2->type == 'd')
                    cnt = 3;
                else
                    cnt = 2;

                while (tmpNode2->RightSibling != NULL)
                {
                    tmpNode2 = tmpNode2->RightSibling;
                    if (tmpNode2->type == 'd')
                        cnt = cnt + 1;
                }
            }
            if (tmpNode->type == 'd')
                type = 'd';
            else if (tmpNode->type == 'f')
                type = '-';
            printf("%c", type);
            print_permission(tmpNode);
            printf("%3d", cnt);
            printf("   ");
            printf("%-5s%-5s", get_UID(tmpNode), get_GID(tmpNode));
            printf("%5d ", tmpNode->SIZE);
            printf("%d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->minute);

            if (tmpNode->type == 'd')
            {
                BOLD;
                BLUE;
                printf("%-15s\n", tmpNode->name);
                DEFAULT;
            }
            else
                printf("%-15s\n", tmpNode->name);

            tmpNode = tmpNode->RightSibling;
        }
    }
    return 0;
}

// cat
int concatenate(DirectoryTree *dirTree, char *fName, int o)
{
    UserNode *tmpUser = NULL;
    DirectoryNode *tmpNode = NULL;
    FILE *fp;
    char buf[MAX_BUFFER_SIZE];
    char tmpName[MAX_NAME_SIZE];
    char *str;
    int tmpSIZE = 0;
    int cnt = 1;

    // file read
    if (o != 0)
    {
        if (o == 4)
        {
            tmpUser = gp_userList->head;
            while (tmpUser != NULL)
            {
                printf("%s:x:%d:%d:%s:%s\n", tmpUser->name, tmpUser->UID, tmpUser->GID, tmpUser->name, tmpUser->dir);
                tmpUser = tmpUser->LinkNode;
            }
            return 0;
        }
        tmpNode = is_exist_directory(dirTree, fName, 'f');

        if (tmpNode == NULL)
        {
            return -1;
        }
        fp = fopen(fName, "r");

        while (feof(fp) == 0)
        {
            fgets(buf, sizeof(buf), fp);
            if (feof(fp) != 0)
            {
                break;
            }
            // w/ line number
            if (o == 2)
            {
                if (buf[strlen(buf) - 1] == '\n')
                {
                    printf("     %d ", cnt);
                    cnt++;
                }
            }
            else if (o == 3)
            {
                if (buf[strlen(buf) - 1] == '\n' && buf[0] != '\n')
                {
                    printf("     %d ", cnt);
                    cnt++;
                }
            }
            fputs(buf, stdout);
        }

        fclose(fp);
    }
    // file write
    else
    {
        fp = fopen(fName, "w");

        while (fgets(buf, sizeof(buf), stdin))
        {
            fputs(buf, fp);
            // get file size
            tmpSIZE += strlen(buf) - 1;
        }

        fclose(fp);

        tmpNode = is_exist_directory(dirTree, fName, 'f');
        // if exist
        if (tmpNode != NULL)
        {
            time(&ltime);
            today = localtime(&ltime);

            tmpNode->month = today->tm_mon + 1;
            tmpNode->day = today->tm_mday;
            tmpNode->hour = today->tm_hour;
            tmpNode->minute = today->tm_min;
        }
        // if file doesn't exist
        else
        {
            make_directory(dirTree, fName, 'f');
        }
        // write size
        tmpNode = is_exist_directory(dirTree, fName, 'f');
        tmpNode->SIZE = tmpSIZE;
    }
    return 0;
}

// chmod
int change_mode(DirectoryTree *dirTree, int mode, char *dirName)
{
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;

    tmpNode = is_exist_directory(dirTree, dirName, 'd');
    tmpNode2 = is_exist_directory(dirTree, dirName, 'f');

    if (tmpNode != NULL)
    {
        if (is_node_has_permission(tmpNode, 'w') != 0)
        {
            printf("chmod: '%s'파일을 수정할 수 없음: 허가거부\n", dirName);
            return -1;
        }
        tmpNode->mode = mode;
        mode_to_permission(tmpNode);
    }
    else if (tmpNode2 != NULL)
    {
        if (is_node_has_permission(tmpNode2, 'w') != 0)
        {
            printf("chmod: '%s'파일을 수정할 수 없음: 허가거부\n", dirName);
            return -1;
        }
        tmpNode2->mode = mode;
        mode_to_permission(tmpNode2);
    }
    else
    {
        printf("chmod: '%s에 접근할 수 없습니다: 그런 파일이나 디렉터리가 없습니다\n", dirName);
        return -1;
    }
    return 0;
}

void change_all_mode(DirectoryNode *dirNode, int mode)
{
    if (dirNode->RightSibling != NULL)
    {
        change_all_mode(dirNode->RightSibling, mode);
    }
    if (dirNode->LeftChild != NULL)
    {
        change_all_mode(dirNode->LeftChild, mode);
    }
    dirNode->mode = mode;
    mode_to_permission(dirNode);
}

// chown
int change_owner(DirectoryTree *dirTree, char *userName, char *dirName)
{
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;
    UserNode *tmpUser = NULL;

    tmpNode = is_exist_directory(dirTree, dirName, 'd');
    tmpNode2 = is_exist_directory(dirTree, dirName, 'f');

    if (tmpNode != NULL)
    {
        if (is_node_has_permission(tmpNode, 'w') != 0)
        {
            printf("chown: '%s'파일을 수정할 수 없음: 허가거부\n", dirName);
            return -1;
        }
        tmpUser = is_exist_user(gp_userList, userName);
        if (tmpUser != NULL)
        {
            tmpNode->UID = tmpUser->UID;
            tmpNode->GID = tmpUser->GID;
        }
        else
        {
            printf("chown: 잘못된 사용자: '%s'\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else if (tmpNode2 != NULL)
    {
        if (is_node_has_permission(tmpNode2, 'w') != 0)
        {
            printf("chown: '%s'파일을 수정할 수 없음: 허가거부\n", dirName);
            return -1;
        }
        tmpUser = is_exist_user(gp_userList, userName);
        if (tmpUser != NULL)
        {
            tmpNode2->UID = tmpUser->UID;
            tmpNode2->GID = tmpUser->GID;
        }
        else
        {
            printf("chown: 잘못된 사용자: '%s'\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else
    {
        printf("chown: '%s'에 접근할 수 없습니다: 그런 파일이나 디렉터리가 없습니다\n", dirName);
        return -1;
    }

    return 0;
}

void change_all_owner(DirectoryNode *dirNode, char *userName)
{
    UserNode *tmpUser = NULL;

    tmpUser = is_exist_user(gp_userList, userName);

    if (dirNode->RightSibling != NULL)
    {
        change_all_owner(dirNode->RightSibling, userName);
    }
    if (dirNode->LeftChild != NULL)
    {
        change_all_owner(dirNode->LeftChild, userName);
    }
    dirNode->UID = tmpUser->UID;
    dirNode->GID = tmpUser->GID;
}

// find
int read_directory(DirectoryTree *dirTree, char *tmp, char *dirName, int o)
{
    char *str;
    char str2[MAX_NAME_SIZE];
    if (o == 0)
    {
        str = strtok(tmp, " ");
        strcpy(str2, str);
        for (int i = 0; i < 10; i++)
        {
            str = strtok(NULL, " ");
        }
        if (str != NULL)
        {
            if (strstr(str2, dirName) != NULL)
            {
                str[strlen(str) - 1] = '\0';
                if (strcmp(str, "/") == 0)
                    printf("/%s\n", str2);
                else
                    printf("%s/%s\n", str, str2);
            }
        }
    }
    else
    {
        str = strtok(tmp, " ");
        strcpy(str2, str);
        for (int i = 0; i < 10; i++)
        {
            str = strtok(NULL, " ");
        }
        if (str != NULL)
        {
            if (strstr(str, dirName) != NULL)
            {
                str[strlen(str) - 1] = '\0';
                if (strcmp(str, "/") == 0)
                    printf("/%s\n", str2);
                else
                    printf("%s/%s\n", str, str2);
            }
        }
    }
    return 0;
}

void find_directory(DirectoryTree *dirTree, char *dirName, int o)
{
    char tmp[MAX_LENGTH_SIZE];

    f_directory = fopen("./resources/Directory.txt", "r");

    while (fgets(tmp, MAX_LENGTH_SIZE, f_directory) != NULL)
    {
        read_directory(dirTree, tmp, dirName, o);
    }

    fclose(f_directory);
}
