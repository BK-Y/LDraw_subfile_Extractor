#include <stdio.h>
#include "tools.h"

int main(void)
{
    LDR_FILE * ldr_file = NULL;
    char command = 0;
    char flag = 1;
    //初始化ldr_file
    ldr_file = malloc(sizeof(*ldr_file));
    if(ldr_file==NULL)
    {
        printf("Error to create Variable ldr_file, need more space.\n");
    }
    else 
    {
        ZeroMemory(ldr_file,sizeof(*ldr_file));// 数据清空
        //显示初始化菜单
        showManu(ldr_file);
        // 输入命令
        while (flag)
        {
            printf(">>>");
            command = getCommand();
            switch (command)
            {
            case '1':
                openFile(ldr_file);
                break;
            case '2':
                system("cls");
                printf("Subfile List:\n");
                showSubfileList(ldr_file);
                printf("Type the one(ID) you want to export\n");
                printf("or q to go back to main manu.\n>>>");
                char cmds[256] ={0};
                scanf("%s",cmds);
                fflush(stdin);
                unsigned long target = str2unlong(cmds);
                if(target!=0)
                    if(OP_exportSub(ldr_file,target)==0) 
                    {
                        printf("Exporting failed! Please Try again");
                        while(1);
                    }
                showManu(ldr_file);
                break;
            case '3':
                //exportAll(ldr_file);
                break;
            case 'q':case 'Q':
                flag = 0;   //置零，退出循环
                break;
            case 't':
                fileAna(ldr_file);
                break;
            case 'f':
                showManu(ldr_file);
                break;
            default:
                printf("Invalid Input\n");
            }
        }
    }
    return 0;
}