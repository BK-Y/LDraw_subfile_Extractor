//openFile: open a open-file dialog to select the target file
#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef LDR__
#define LDR__
#define MAX_INDEX 512
#define FORMAT_LIST "mpd,ldr"
#define FILE_STATE_CLOSED 0
#define FILE_STATE_OPEN 1
#define FILE_FORMAT_ACCEPTED   2
#define FILE_FORMAT_ERROR   3
#define FILE_OPEN_FAILED 4
#define FILE_OPEN_SUCCESS  1

//结构体定义
//双链
typedef struct list_node
{
    unsigned long cur;   //记录当前行指针偏移量
    unsigned long lines; //记录文件开头到当前行位置的行数,因为需要扣除不同系统上对换行的处理
    char * fileName;
    struct list_node * next;
    struct list_node * prev;
}L_NODE; 
typedef struct list_head 
{ 
    L_NODE * node;  //头节点位置
    unsigned long node_amount;
}L_HEAD;
//文件记录
typedef struct
{
    char path[MAX_PATH];
    char filestate;
    FILE *fp;
    L_HEAD * head;
    unsigned long lines;
} LDR_FILE;

//方法
//文件操作
LDR_FILE * fileClose(LDR_FILE *);
int GetOpenFilePath(char * path);
int openFile(LDR_FILE *);
char * getSubfileName(FILE*,unsigned long offset);
//UI与程序操作函数
void showManu(LDR_FILE const *);
void printFileName(LDR_FILE const *);
char getCommand(void);
unsigned long str2unlong(char const *);
void showSubfileList(LDR_FILE *);

//文件分析与处理函数
int fileFormatCheck(LDR_FILE const *);
int fileAna(LDR_FILE *);
int fileAnaFindMeta(LDR_FILE *,char *);
void exportRubberBand(LDR_FILE *fp);
int fileAnaAppendRecord(LDR_FILE *,unsigned long cur,unsigned long lines, char * fileName);
int emptyAnaData(LDR_FILE *);
unsigned long  OP_getFileSize(LDR_FILE*,unsigned long id);
int OP_exportSub(LDR_FILE *, unsigned long id);
#endif