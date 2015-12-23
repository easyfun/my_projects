#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int GetCharCount(const char* str, char c) {
    int n=0;
    while (*str) {
        if (*str==c)
            n++;
        str++;
    }
    return n;
}

//from 1
void GetStrValue(const char* str, char c,int index, char* value) {
    int n=0;
    const char* head;
    const char* tail;
    head=str;
    tail=NULL;
    value[0]='\0';
    while (*str) {
        if (*str++ != c) {
            continue;
        }
        n++;
        if (n==index-1) {
            head=str;
        } else if (n==index) {
            tail=str-1;
            break;
        } else if (n>index) {
            break;
        }
    }
    if (0==n) {
        tail=str;
    } else if (n < index) {
        if (n < index-1)
            head=str;
        tail=str;
    }
    if (tail) {
        memcpy(value,head,tail-head);
        value[tail-head]='\0';
    }
}

int main(int argc,char* argv[]) {
    if (2!=argc) {
        printf("exe path\n");
        return 1;
    }
    const char* folder_name=argv[1];
        int file_n=0;
    int num=GetCharCount(folder_name, '/')+1;
    printf("num=%d\n", num);
    char file_name[200];
    char value[200];
    int i=0;

    for (i=0;i<10;i++) {
        GetStrValue(folder_name, '/', num+i, value);
        printf("i=%d, value=%s\n", num+i, value);
    }

    for (i=0;i<num;i++) {
        GetStrValue(folder_name, '/', i+1, value);
        printf("i=%d, value=%s\n", i,value);
        if (value[0]=='\0')
            continue;
        strcat(file_name,value);
        file_n++;
    }
    printf("file_n=%d\n", file_n);

    return 0;
}