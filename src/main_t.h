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

#define MAX_BUFFER_SIZE 512
#define MAX_LENGTH_SIZE 200
#define MAX_DIRECTORY_SIZE 50
#define MAX_NAME_SIZE 20

// f_user
typedef struct tagUserNode
{
    char name[MAX_NAME_SIZE];
    char dir[MAX_DIRECTORY_SIZE];
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
    char name[MAX_NAME_SIZE];
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
    char name[MAX_NAME_SIZE];
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
void instruction(DirectoryTree *dirTree, char *cmd);
void print_start();
void print_head(DirectoryTree *dirTree, Stack *dirStack);

// directory.c
// utility
int mode_to_permission(DirectoryNode *dirNode);
void print_to_permission(DirectoryNode *dirNode);
void destory_node(DirectoryNode *dirNode);
void destory_directory(DirectoryNode *dirNode);
DirectoryNode *is_exist_directory(DirectoryTree *dirTree, char *dirName, char type);
char *get_directory(char *dirPath);

// save & load
void get_directory_path(DirectoryTree *dirTree, DirectoryNode *dirNode, Stack *dirStack);
void write_directory_node(DirectoryTree *dirTree, DirectoryNode *dirNode, Stack *dirStack);
void save_directory(DirectoryTree *dirTree, Stack *dirStack);
int read_directory_node(DirectoryTree *dirTree, char *tmp);
DirectoryTree *load_directory();

// mkdir
DirectoryTree *initialize_directory_tree();
int make_directory(DirectoryTree *dirTree, char *dirName, char type);
// rm
int remove_directory(DirectoryTree *dirTree, char *dirName);
// cd
int move_current_path(DirectoryTree *dirTree, char *dirPath);
int move_directory_path(DirectoryTree *dirTree, char *dirPath);
// pwd
int print_directory_path(DirectoryTree *dirTree, Stack *dirStack);
// ls
int list_directory(DirectoryTree *dirTree, int a, int l);
// cat
int concatenate(DirectoryTree *dirTree, char *fName, int o);
// chmod
int change_mode(DirectoryTree *dirTree, int mode, char *dirName);
void change_all_mode(DirectoryNode *dirNode, int mode);
// chown
int change_owner(DirectoryTree *dirTree, char *userName, char *dirName);
void change_all_owner(DirectoryNode *dirNode, char *userName);
// find
int read_directory(DirectoryTree *dirTree, char *tmp, char *dirName, int o);
void find_directory(DirectoryTree *dirTree, char *dirName, int o);

// user.c
UserList *initialize_user();
void write_user(UserList *userList, UserNode *userNode);
void save_user_list(UserList *userList);
int read_user(UserList *userList, char *tmp);
UserList *load_user_list();
UserNode *is_exist_user(UserList *userList, char *userName);
char *get_UID(DirectoryNode *dirNode);
char *get_GID(DirectoryNode *dirNode);
int is_node_has_permission(DirectoryNode *dirNode, char o);
void login(UserList *userList, DirectoryTree *dirTree);

// stack.c
int is_empty(Stack *dirStack);
Stack *initialize_stack();
int push(Stack *dirStack, char *dirName);
char *pop(Stack *dirStack);

// time.c
void get_month(int i);
void get_weekday(int i);

// global pointer variable
DirectoryTree *gp_directory;
Stack *gp_directoryStack;
UserList *gp_userList;
FILE *f_directory;
FILE *f_user;

// global variable
sem_t semp;

#endif // MAIN_H_
