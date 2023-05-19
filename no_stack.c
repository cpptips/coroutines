#include <stdio.h>
int function(void) {
    static int state = 0;
    static int i = 0;
    switch(state){
        case 0: goto LABEL0;
        case 1: goto LABEL1;
        case 2: goto LABEL2;
    }
    LABEL0:
    for(i=0;i<10;++i){
        state = 1;
        return i;
        LABEL1:
        state = 2;
        i+=1;
        return i;
        LABEL2:
        state = 3;
    }
}

int main() {
    int i =function();
    printf("i:%d\n",i);
    i = function();
    printf("i:%d\n",i);
    i = function();
    printf("i:%d\n",i);
    i = function();
    printf("i:%d\n",i);
    i = function();
    printf("i:%d\n",i);
    return 0;
}