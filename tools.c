#include "tools.h"
#include <stdio.h>
#include <string.h>
/*
工具函数定义
--LINE_INDEX 链存储操作
*/
L_HEAD * L_create(void);//创建列表
int L_insert(L_HEAD*, unsigned long cur,unsigned long lines,char *);//尾插法插入数据
L_NODE * L_fetch(L_HEAD *, unsigned long id);//读取数据,返回读取到的数据内容
int L_delete(L_HEAD * );//链删除,包括头自身

int fileWrite(const char * title, const char * data,size_t size);//数据写入文件

//实现
//获取文件路径,获取后防盗path指向的空间
int GetOpenFilePath(char *path)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn); // 结构大小
    ofn.lpstrFile = path; // 路径
    ofn.nMaxFile = MAX_PATH; // 路径大小
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0"; // 文件类型
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	return GetOpenFileName(&ofn);
}

//打开目标文件，并将打开后的文件
int openFile(LDR_FILE * fp)
{
    printf("File Opening...");
    if(GetOpenFilePath(fp->path))
    {
        if(fileFormatCheck(fp) == FILE_FORMAT_ERROR)
        {
            system("cls");
            printf("Error: Unsupported File Format! \n");
            printf("Press Enter to keep Going");
            getchar();
            ZeroMemory(fp->path,sizeof(fp->path));
        }
        else  
        {
            fp->fp=fopen(fp->path,"r");
            if(fp==NULL) 
            {
                printf("\nFail to load the file!Try Again Please\n");
                return FILE_OPEN_FAILED;
            }
            else 
            {
                fp->filestate = FILE_STATE_OPEN;
                fp->head = L_create();
                fileAna(fp);
            }
        }
    }
    else
    {
        printf("Error to open the file, try again!");
        return FILE_OPEN_FAILED;
    }
    showManu(fp);
    return FILE_OPEN_SUCCESS;
}

void showManu(LDR_FILE const * ldrFile)
{
    system("cls");
    printf("|============================================|\n");
    printf("|        LDraw Rubber Band Exractor          |\n");
    printf("|============================================|\n");
    printf("|           *Current Opening File*           |\n");
    printf("|           ----------------------           |\n");
    printFileName(ldrFile);
    printf("|--------------------------------------------|\n");
    printf("|                 *Manus*                    |\n");
    printf("|                 -------                    |\n");
    printf("|1. Open a New File                          |\n");
    printf("|2. Show Subfile list in current file.       |\n");
    printf("|q. Quit the Program                         |\n");
    printf("|--------------------------------------------|\n");
    printf("|This program is designed for exract subfiles|\n");
    printf("|in an LDraw by LDcad                        |\n");
    printf("|--------------------------------------------|\n");
}

char getCommand(void)
{
    char tmp;
    tmp = getchar();
    fflush(stdin);
    return(tmp);
}

void printFileName(LDR_FILE const * ldrFile)
{
    int i,j,k;    //用于确定如何对文件名进行输出
    const int len = 45;
    char str[45]={0};
    char const * tmp_pos = ldrFile->path;
    if(ldrFile == NULL || ldrFile->fp == NULL)
    {
        printf("| No File is Opening!                        |\n");
    }
    else //格式调整
    {
        i = strlen(ldrFile->path);
        j=i/(len-1);
        for(i=0;i<j;i++)
        {
            strncpy(str,tmp_pos+(len-1)*i,len-1);
            printf("|%s|\n",str);
        }
        //处理最后一行
        i = strlen(ldrFile->path);
        i = i-(len-1)*j;
        for(k=0;k<i;k++)
        {
            str[k]=ldrFile->path[(len-1)*j+k];
        }
        for(k;k<(len-1);k++) str[k]=' ';
        printf("|%s|\n",str);
    }
}
int fileWrite(const char * title, const char * data,size_t  size)
{
    FILE * fp = NULL;
    if(fopen(title,"r") == NULL)    //文件不存在时的处理
    {
        fp = fopen(title,"w");
        fwrite(data,sizeof(char),size,fp);
    }
    else                //同名文件已存在
    {
        int msg;
        msg = MessageBox(NULL,"File Existed, Overwrite?","Warning",MB_YESNO);
        if(msg == IDYES)
        {
            fp = fopen(title,"w");
            fwrite(data,sizeof(char),size,fp);
        }
        else 
            MessageBox(NULL,"File exported Failure because the existence of same name file","Warning",MB_OK);
    }
    fclose(fp);
    fp=NULL;
}

int fileFormatCheck(LDR_FILE const * fp)
{
    char suffix[4]={0};
    int i;
    int len = strlen(fp->path);
    //截取最后三个字母
    for (i=3;i>0;i--)
    {
        suffix[3-i]=fp->path[len-i];
    }
    suffix[3]='\0';
    if(strstr(FORMAT_LIST,suffix)) 
        return FILE_FORMAT_ACCEPTED;
    else return FILE_FORMAT_ERROR;
}

void exportRubberBand(LDR_FILE *fp)
{

}
//清空已经打开文件里的文件分析数据,对应空间也释放
int emptyAnaData(LDR_FILE * fp)
{
    L_HEAD * head = fp->head;
    L_delete(fp->head);
    fp->head = NULL;
}
//关闭文件
LDR_FILE * fileClose(LDR_FILE * fp)
{
    fclose(fp->fp);//关闭文件
    //数据清空
    emptyAnaData(fp);
    ZeroMemory(fp,sizeof(LDR_FILE));
    free(fp);
    return NULL;
}
//获取子文件文件名
char * getSubfileName(FILE*fp,unsigned long offset)
{
    unsigned long ori_pos = ftell(fp);//程序结束后数据还原使用
    unsigned int count=0;
    char ch;
    char * str;
    fseek(fp,offset+6,SEEK_SET);
    while(1)
    {
        ch = fgetc(fp);
        count++;
        if(ch=='\n') break;
    }
    str = malloc(count*sizeof(char));
    if(str == NULL) return NULL;
    fseek(fp,offset+6,SEEK_SET);
    fscanf(fp,"%s",str);
    fseek(fp,ori_pos,SEEK_SET);
    return str;
}

int fileAnaFindMeta(LDR_FILE * fp, char * meta)
{
    int meta_len = strlen(meta)+1;  //
    char * string = malloc(meta_len);
    unsigned long pos = ftell(fp->fp);
    char *flag;// 记录是否找到目标数据
    fgets(string,meta_len,fp->fp);
    flag = strstr(string,meta);
    //现场复原
    fseek(fp->fp,pos,SEEK_SET);
    free(string);//内存清理

    return (flag!=NULL);
}

int fileAna(LDR_FILE * fp)
{
    unsigned char ch='A';
    unsigned long line = 0;
    unsigned long cur = 0;
    int flag = 0;
    int i = 0;
    char * tmpStr = NULL;
    if(fp->filestate == FILE_STATE_CLOSED) 
        return FILE_STATE_CLOSED;
    system("cls");
    printf("File: %s\n",fp->path);
    printf("Analyzing...\n");
    while(!feof(fp->fp))
    {
        cur = ftell(fp->fp);
        ch = fgetc(fp->fp);

        if(cur == 0 && ch =='0')
        {
            //printf("%p\t", ftell(fp->fp));
            fseek(fp->fp,-1,SEEK_CUR);
            flag = fileAnaFindMeta(fp, "0 FILE");
            fseek(fp->fp,1,SEEK_CUR);
        }
        else if(ch == '\n')
        {
            line++;
            //printf("%p\t",ftell(fp->fp));
            flag = fileAnaFindMeta(fp,"0 FILE");
        }
        if(flag)
        {
            i++;
            tmpStr = getSubfileName(fp->fp,ftell(fp->fp));
            fileAnaAppendRecord(fp,ftell(fp->fp),line,tmpStr);
            printf("%-2d:%s\n",i,tmpStr);
            free(tmpStr);
            tmpStr = NULL;
            flag = 0;
        }
    }
    fp->lines = line;
    printf("Hit enter to keep going!");
    getchar();
    fflush(stdin);
}
int fileAnaAppendRecord(LDR_FILE * fp, unsigned long cur,unsigned long lines,char * fileName)
{
    L_insert(fp->head,cur,lines,fileName);
    //printf("%7d:",fp->head->node->prev->cur);
    //printf("%s\n",fp->head->node->prev->fileName);
}
void showSubfileList(LDR_FILE * fp)
{
    unsigned long i;
    L_NODE * tmp_node;
    for(i=0;i<fp->head->node_amount;i++)
    {
        tmp_node = L_fetch(fp->head,i);
        if(tmp_node == NULL) break;
        printf("%3d:%s\n",i+1,tmp_node->fileName);
    }

    #if 0
    getCommand();
    fflush(stdin);
    #endif 
}
//功能函数
unsigned long str2unlong(char const * str)
{
    int length = strlen(str);
    unsigned long tmp=0;
    int i=0,j=0;
    for(i=0;i<length;i++)
    {
        if(str[i]<'0'||str[i]>'9') break;
    }
    for(i;i>0;i--)
    {
        tmp=tmp*10+(str[j]-'0');
        j++;
    }
    putchar('\n');
    return tmp;
}

unsigned long OP_getFileSize(LDR_FILE* fp,unsigned long id)
{   
    L_NODE * node = NULL;
    unsigned long size=0;
    unsigned long ori_pos = ftell(fp->fp);//记录文件位置
    node = L_fetch(fp->head,id);
    if(node == NULL) return 0;  //读取失败
    if (node->next == fp->head->node)
    {
        fseek(fp->fp,0,SEEK_END);
        size = ftell(fp->fp)-node->cur - fp->lines + node->lines;
        fseek(fp->fp,ori_pos,SEEK_SET);
    }
    else
        size = node->next->cur - node->cur - node->next->lines + node->lines;  //位置差-行数差（不同系统之间的换行符处理方式不一样）
    return size;
}
int OP_exportSub(LDR_FILE * fp,unsigned long id)
{
    L_NODE * tmp_node = NULL;
    char cmds[255] = {0};
    char *data = NULL;
    char * sub_title = NULL;
    size_t title_size = 0;
    unsigned long cmdNo = id-1; //减1以保证最终输出的结果不错位
    size_t file_size=0;
    unsigned long ori_pos = ftell(fp->fp);
    FILE * write = NULL;    //用于将文件写入本地

    //目标数据移动到内存
    file_size = OP_getFileSize(fp,cmdNo);   //获取文件大小
    title_size = 0;
    if(file_size == 0) 
    {
        printf("ERROR:Size of Target Subfile is 0\n");
        return 0;    //目标子文件大小为0
    }
    //获取内容
    data = malloc(file_size+1);   //用于存放数据的内存空间
    if(data == NULL) return 0;// 内存空间不够，失败
    tmp_node = L_fetch(fp->head,cmdNo); //获取目标节点信息
    fseek(fp->fp,tmp_node->cur,SEEK_SET);
    fread(data,1,file_size,fp->fp);
    *(data+file_size) = '\0';
    //获取标题
    for(title_size = 0; *(data+title_size)!='\n';title_size++);
    sub_title = malloc(title_size+1);
    strncpy(sub_title,data,title_size);
    *(sub_title+title_size)='\0';
    //文件输出
    fileWrite(sub_title,data,file_size);
    //空间回收
    free(data);
    data = NULL;
    free(sub_title);
    sub_title = NULL;
    getchar();
}

//列表操作
L_HEAD * L_create(void)
{
    L_HEAD * tmp_point = malloc(sizeof(L_HEAD));
    if(tmp_point == NULL) return NULL;
    tmp_point -> node = malloc(sizeof(L_NODE));
    if(tmp_point->node==NULL)
    {
        free(tmp_point);
        return NULL;
    }
    tmp_point->node_amount = 0;//0代表自己带的那个数字
    tmp_point->node->cur = 0;
    tmp_point->node->fileName = NULL;
    tmp_point->node->next = tmp_point->node;
    tmp_point->node->prev = tmp_point->node;
    return tmp_point;
}

int L_insert(L_HEAD * h,unsigned long cur,unsigned long lines,char * fileName)
{
    L_NODE * new_node = malloc(sizeof(L_NODE));
    if(new_node == NULL) return 0;
    //数据初始化
    h->node_amount++;
    new_node->cur = cur; 
    new_node->lines = lines;
    new_node->fileName = malloc(strlen(fileName)+1);
    strcpy(new_node->fileName,fileName);
    //printf("|--%7d%7d\t%s\t%s\n",cur,new_node->cur,fileName,new_node->fileName);
    free(fileName);
    //指针换绑
    new_node->next = h->node;
    new_node->prev = h->node->prev;
    h->node->prev->next = new_node;
    h->node->prev = new_node;
    return 1;
}

int L_delete(L_HEAD * h)
{
    L_NODE * tmp_cur;
    L_NODE * tmp_del;
    if(h->node == NULL) return 1;
    tmp_cur = h->node;
    while(tmp_cur->next != h->node)
    {
        tmp_del = tmp_cur;
        tmp_cur = tmp_cur->next;
        ZeroMemory(tmp_del,sizeof(L_NODE));
        free(tmp_del);
    }
    ZeroMemory(h,sizeof(L_HEAD));
    free(h);
    return 1;
}
L_NODE * L_fetch(L_HEAD * h, unsigned long id)
{
    int i;
    L_NODE * tmp_node = NULL;
    //判断是否超过文件实际包含的文件数量
    if(id > h->node_amount) 
    {
        system("cls");
        printf("line number should range \n");
        printf("from 1 to %d, try again!\n",h->node_amount);
        return NULL;
    }
    tmp_node = h->node;
    for (i=0;i<id+1;i++)
    {
        tmp_node = tmp_node->next;
    }
    return tmp_node;
}