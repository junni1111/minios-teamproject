#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifndef MAIN_H_
#define MAIN_H_
#define DEFAULT printf("%c[%dm", 0x1B, 0)
#define BOLD printf("%c[%dm", 0x1B, 1)
#define WHITE printf("\x1b[37m")
#define BLUE printf("\x1b[34m")
#define GREEN printf("\x1b[32m")

#define MAX_BUFFER 512
#define MAX_LENGTH 200
#define MAX_DIR 50
#define MAX_NAME 20

// User
typedef struct tagUserNode
{
    char name[MAX_NAME];
    char dir[MAX_DIR];
    int UID;
    int GID;
    int year;
    int month;
    int wday;
    int day;
    int hour;
    int minute;
    int sec;
    struct tagUserNode *LinkNode;
} UserNode;

typedef struct tagUser
{
    int topUID;
    int topGID;
    UserNode *head;
    UserNode *tail;
    UserNode *current;
} UserList;

// Directory
typedef struct tagDirectoryNode
{
    char name[MAX_NAME];
    char type;
    int mode;
    int permission[9];
    int SIZE;
    int UID;
    int GID;
    int month;
    int day;
    int hour;
    int minute;
    struct tagDirectoryNode *Parent;
    struct tagDirectoryNode *LeftChild;
    struct tagDirectoryNode *RightSibling;
} DirectoryNode;

typedef struct tagDirectoryTree
{
    DirectoryNode *root;
    DirectoryNode *current;
} DirectoryTree;

// stack using linked list
typedef struct tagStackNode
{
    char name[MAX_NAME];
    struct tagStackNode *LinkNode;
} StackNode;

typedef struct tagStack
{
    StackNode *TopNode;
    int cnt;
} Stack;

// time
time_t ltime;
struct tm *today;

// instruction.c
int mkdir(DirectoryTree *dirTree, char *cmd);
int rm(DirectoryTree *dirTree, char *cmd);
int cd(DirectoryTree *dirTree, char *cmd);
int pwd(DirectoryTree *dirTree, Stack *dirStack, char *cmd);
int ls(DirectoryTree *dirTree, char *cmd);
int cat(DirectoryTree *dirTree, char *cmd);
int chmod(DirectoryTree *dirTree, char *cmd);
int chown_(DirectoryTree *dirTree, char *cmd);
int find_(DirectoryTree *dirTree, char *cmd);
void Instruction(DirectoryTree *dirTree, char *cmd);
void PrintStart();
void PrintHead(DirectoryTree *dirTree, Stack *dirStack);

// directory.c
// utility
int Mode2Permission(DirectoryNode *dirNode);
void PrintPermission(DirectoryNode *dirNode);
void DestroyNode(DirectoryNode *dirNode);
void DestroyDir(DirectoryNode *dirNode);
DirectoryNode *IsExistDir(DirectoryTree *dirTree, char *dirName, char type);
char *getDir(char *dirPath);

// save & load
void getPath(DirectoryTree *dirTree, DirectoryNode *dirNode, Stack *dirStack);
void WriteNode(DirectoryTree *dirTree, DirectoryNode *dirNode, Stack *dirStack);
void SaveDir(DirectoryTree *dirTree, Stack *dirStack);
int ReadNode(DirectoryTree *dirTree, char *tmp);
DirectoryTree *LoadDir();

// mkdir
DirectoryTree *InitializeTree();
int MakeDir(DirectoryTree *dirTree, char *dirName, char type);
// rm
int RemoveDir(DirectoryTree *dirTree, char *dirName);
// cd
int Movecurrent(DirectoryTree *dirTree, char *dirPath);
int MovePath(DirectoryTree *dirTree, char *dirPath);
// pwd
int PrintPath(DirectoryTree *dirTree, Stack *dirStack);
// ls
int ListDir(DirectoryTree *dirTree, int a, int l);
// cat
int Concatenate(DirectoryTree *dirTree, char *fName, int o);
// chmod
int ChangeMode(DirectoryTree *dirTree, int mode, char *dirName);
void ChangeModeAll(DirectoryNode *dirNode, int mode);
// chown
int ChangeOwner(DirectoryTree *dirTree, char *userName, char *dirName);
void ChangeOwnerAll(DirectoryNode *dirNode, char *userName);
// find
int ReadDir(DirectoryTree *dirTree, char *tmp, char *dirName, int o);
void FindDir(DirectoryTree *dirTree, char *dirName, int o);

// user.c
UserList *InitializeUser();
void WriteUser(UserList *userList, UserNode *userNode);
void SaveUserList(UserList *userList);
int ReadUser(UserList *userList, char *tmp);
UserList *LoadUserList();
UserNode *IsExistUser(UserList *userList, char *userName);
char *GetUID(DirectoryNode *dirNode);
char *GetGID(DirectoryNode *dirNode);
int HasPermission(DirectoryNode *dirNode, char o);
void Login(UserList *userList, DirectoryTree *dirTree);

// stack.c
int IsEmpty(Stack *dirStack);
Stack *InitializeStack();
int Push(Stack *dirStack, char *dirName);
char *Pop(Stack *dirStack);

// time.c
void GetMonth(int i);
void GetWeekday(int i);

// global variable
DirectoryTree *Linux;
Stack *dStack;
UserList *usrList;
FILE *Dir;
FILE *User;
sem_t semp;

#endif // MAIN_H_
