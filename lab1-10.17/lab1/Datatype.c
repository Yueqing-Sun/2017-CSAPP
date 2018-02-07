#include <stdio.h>

typedef unsigned char *byte_pointer;
void show_bytes(byte_pointer start,size_t len);
void show_int(int x);
void show_float(float x);
void show_pointer(void *x);

int *p;
int s[3] = {1, 2, 3};
int num = 1160300901;
short snum = 901;
long lnum = 1160300901;
float fId = 1997;
double dId = 1997;
char cName= 's';

struct myself {
    int number;
    char name;
    float score;
} x = {1160300901, 's', 89};

union test {
    int num;
    char mark;
    float score;
};

enum weekday {
    sun, mon, tue
} ;

int main() {
    int a=sun;
    int b=mon;
    int var = 10;
    p = &var;
    printf("p = %d ,address = =%d  ", *p, p);
    show_pointer( p);

    printf("s[0] = %d ,address = %d  ", s[0], &s[0]);
    show_bytes((byte_pointer)&s[0], sizeof(int));

    printf("s[1] = %d ,address = %d  ", s[1], &s[1]);
    show_bytes((byte_pointer)&s[1], sizeof(int));

    printf("s[2] = %d ,address = %d  ", s[2], &s[2]);
    show_bytes((byte_pointer)&s[2], sizeof(int));

    printf("int num = %d ,address = %d  ", num, &num);
    show_int( num);

    printf("short num = %d ,address = %d  ", snum, &snum);
    show_bytes((byte_pointer)&snum, sizeof(short));

    printf("long num = %d ,address = %d ", lnum, &lnum);
    show_bytes((byte_pointer)&lnum, sizeof(long));

    printf("float Id = %f ,address = %d  ", fId, &fId);
    show_float(fId);

    printf("double Id = %f ,address = %d  ", dId, &dId);
    show_bytes((byte_pointer)&dId, sizeof(double));

    printf("char cName = %c ,address = %d  ", cName, &cName);
    show_bytes((byte_pointer)&cName, sizeof(char));

    printf("x.number = %d ,address = %d  ", x.number, &x.number);
    show_bytes((byte_pointer)&x.number, sizeof(int));

    printf("x.name = %c ,address = %d  ", x.name, &x.name);
    show_bytes((byte_pointer)&x.name, sizeof(char));

    printf("x.score = %f ,address = %d  ", x.score, &x.score);
    show_float(x.score);

    printf("a = %d ,address = %d  ", a, &a);
    show_int(a);
    printf("b = %d ,address = %d  ", b, &b);
    show_int(b);

    printf("main的地址：%p\n", main);
    printf("printf的地址：%p\n", printf);
    union test Union;
    Union.num=1603009;
    Union.mark='s';
    printf("Union.num = %d ,address = %d  ", Union.num,&Union.num);
    show_int(Union.num);
    printf("Union.mark = %c ,address = %d  ", Union.mark,&Union.mark);
    show_bytes((byte_pointer)&Union.mark, sizeof(char));
    return 0;
}
void show_bytes(byte_pointer start,size_t len){
    size_t  i;
    printf("16进制的内存各字节:");
    for(i=0;i<len;i++){
        printf(" %.2x",start[i]);
    }
    printf("\n");
}
void show_int(int x){
    show_bytes((byte_pointer)&x, sizeof(int));
}
void show_float(float x){
    show_bytes((byte_pointer)&x, sizeof(float));
}
void show_pointer(void *x){
    show_bytes((byte_pointer)&x, sizeof(void*));
}