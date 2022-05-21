#include "main.h"
#include <stdio.h>
#include <stdlib.h>

UserList *initialize_user()
{
    UserList *returnList = (UserList *)malloc(sizeof(UserList));
    UserNode *NewNode = (UserNode *)malloc(sizeof(UserNode));

    // get time
    time(&ltime);
    today = localtime(&ltime);

    strncpy(NewNode->name, "root", MAX_NAME_SIZE);
    strncpy(NewNode->dir, "/", MAX_NAME_SIZE);
    NewNode->UID = 0;
    NewNode->GID = 0;
    NewNode->year = today->tm_year + 1900;
    NewNode->month = today->tm_mon + 1;
    NewNode->wday = today->tm_wday;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->minute = today->tm_min;
    NewNode->sec = today->tm_sec;
    NewNode->LinkNode = NULL;

    returnList->head = NewNode;
    returnList->tail = NewNode;
    returnList->current = NewNode;
    returnList->topUID = 0;
    returnList->topGID = 0;

    return returnList;
}

void write_user(UserList *userList, UserNode *userNode)
{
    time(&ltime);
    today = localtime(&ltime);

    userList->current->year = today->tm_year + 1900;
    userList->current->month = today->tm_mon + 1;
    userList->current->wday = today->tm_wday;
    userList->current->day = today->tm_mday;
    userList->current->hour = today->tm_hour;
    userList->current->minute = today->tm_min;
    userList->current->sec = today->tm_sec;

    fprintf(f_user, "%s %d %d %d %d %d %d %d %d %d %s\n", userNode->name, userNode->UID, userNode->GID, userNode->year, userNode->month, userNode->wday, userNode->day, userNode->hour, userNode->minute, userNode->sec, userNode->dir);

    if (userNode->LinkNode != NULL)
    {
        write_user(userList, userNode->LinkNode);
    }
}

void save_user_list(UserList *userList)
{
    f_user = fopen("./resources/User.txt", "w");

    write_user(userList, userList->head);

    fclose(f_user);
}

int read_user(UserList *userList, char *tmp)
{
    UserNode *NewNode = (UserNode *)malloc(sizeof(UserNode));
    char *str;

    NewNode->LinkNode = NULL;

    str = strtok(tmp, " ");
    strncpy(NewNode->name, str, MAX_NAME_SIZE);
    str = strtok(NULL, " ");
    NewNode->UID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->GID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->year = atoi(str);
    str = strtok(NULL, " ");
    NewNode->month = atoi(str);
    str = strtok(NULL, " ");
    NewNode->wday = atoi(str);
    str = strtok(NULL, " ");
    NewNode->day = atoi(str);
    str = strtok(NULL, " ");
    NewNode->hour = atoi(str);
    str = strtok(NULL, " ");
    NewNode->minute = atoi(str);
    str = strtok(NULL, " ");
    NewNode->sec = atoi(str);
    str = strtok(NULL, " ");
    str[strlen(str) - 1] = '\0';
    strncpy(NewNode->dir, str, MAX_DIRECTORY_SIZE);

    if (strcmp(NewNode->name, "root") == 0)
    {
        userList->head = NewNode;
        userList->tail = NewNode;
    }
    else
    {
        userList->tail->LinkNode = NewNode;
        userList->tail = NewNode;
    }
    return 0;
}

UserList *load_user_list()
{
    UserList *userList = (UserList *)malloc(sizeof(UserList));
    char tmp[MAX_LENGTH_SIZE];

    f_user = fopen("./resources/User.txt", "r");

    while (fgets(tmp, MAX_LENGTH_SIZE, f_user) != NULL)
    {
        read_user(userList, tmp);
    }

    fclose(f_user);

    userList->current = NULL;

    return userList;
}

UserNode *is_exist_user(UserList *userList, char *userName)
{
    UserNode *returnUser = NULL;

    returnUser = userList->head;

    while (returnUser != NULL)
    {
        if (strcmp(returnUser->name, userName) == 0)
            break;
        returnUser = returnUser->LinkNode;
    }

    return returnUser;
}

char *get_UID(DirectoryNode *dirNode)
{
    UserNode *tmpNode = NULL;

    tmpNode = gp_userList->head;
    while (tmpNode != NULL)
    {
        if (tmpNode->UID == dirNode->UID)
            break;
        tmpNode = tmpNode->LinkNode;
    }
    return tmpNode->name;
}

char *get_GID(DirectoryNode *dirNode)
{
    UserNode *tmpNode = NULL;

    tmpNode = gp_userList->head;
    while (tmpNode != NULL)
    {
        if (tmpNode->GID == dirNode->GID)
            break;
        tmpNode = tmpNode->LinkNode;
    }
    return tmpNode->name;
}

int is_node_has_permission(DirectoryNode *dirNode, char o)
{
    if (gp_userList->current->UID == 0)
        return 0;

    if (gp_userList->current->UID == dirNode->UID)
    {
        if (o == 'r')
        {
            if (dirNode->permission[0] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'w')
        {
            if (dirNode->permission[1] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'x')
        {
            if (dirNode->permission[2] == 0)
                return -1;
            else
                return 0;
        }
    }
    else if (gp_userList->current->GID == dirNode->GID)
    {
        if (o == 'r')
        {
            if (dirNode->permission[3] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'w')
        {
            if (dirNode->permission[4] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'x')
        {
            if (dirNode->permission[5] == 0)
                return -1;
            else
                return 0;
        }
    }
    else
    {
        if (o == 'r')
        {
            if (dirNode->permission[6] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'w')
        {
            if (dirNode->permission[7] == 0)
                return -1;
            else
                return 0;
        }
        if (o == 'x')
        {
            if (dirNode->permission[8] == 0)
                return -1;
            else
                return 0;
        }
    }
    return -1;
}

void login(UserList *userList, DirectoryTree *dirTree)
{
    UserNode *tmpUser = NULL;
    char userName[MAX_NAME_SIZE];
    char tmp[MAX_DIRECTORY_SIZE];

    tmpUser = userList->head;

    printf("Users: ");
    while (tmpUser != NULL)
    {
        printf("%s ", tmpUser->name);
        tmpUser = tmpUser->LinkNode;
    }
    printf("\n");

    while (1)
    {
        printf("Login as: ");
        fgets(userName, sizeof(userName), stdin);
        userName[strlen(userName) - 1] = '\0';
        if (strcmp(userName, "exit") == 0)
        {
            exit(0);
        }
        tmpUser = is_exist_user(userList, userName);
        if (tmpUser != NULL)
        {
            userList->current = tmpUser;
            break;
        }
        printf("'%s' 유저가 존재하지 않습니다\n", userName);
    }

    strcpy(tmp, userList->current->dir);
    move_directory_path(dirTree, tmp);
}
