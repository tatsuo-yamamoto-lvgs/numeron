#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int factorial(int digit, int n){
    if (n < 9-digit+1){
        return 1;
    }
    return (n+1)*factorial(digit,n-1);
}

int permutation(char between0and9[],char number[],char *numbers[],int digit,int n,int *count_pointer, int init_digit){
    digit--;
    n--;
    for (int i= 0; i<=n+1; ++i){
        // 数字をnumberに格納
        number[digit] = between0and9[i];
        // 一旦保存
        char temp_array[11];
        int s;
        for (s=0; s<=10; ++s){
            temp_array[s] = between0and9[s];
        }
        // 格納し終えた数字はもう使わないからしまっちゃおうね
        char temp = between0and9[i];
        between0and9[i] = between0and9[n+1];
        between0and9[n+1] = temp;
        if (digit <= 0){
            // printf("%s\n",number);
            number[init_digit] = '\0';
            strcpy(numbers[*count_pointer],number);
            printf("%s\n",number);
            *count_pointer = *count_pointer+1;
        }else{
            permutation(between0and9,number,numbers,digit,n,count_pointer,init_digit);
        }
        // 復元しようね
        int p;
        for (p=0; p<=10; ++p){
            between0and9[p] = temp_array[p];
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    if (argc != 2){
        perror("command line error\n");
        exit(EXIT_FAILURE);
    }
    
    int digit = atoi(argv[1]);
    int init_digit = atoi(argv[1]);
    int n = 9;
    int count = 0;
    int *count_pointer;
    count_pointer = &count;
    int npr = factorial(digit, n);
    char between0and9[11] = {"0123456789\0"};
    char *number = (char*)malloc(sizeof(char)*(digit + 1));
    memset(number,'\0',sizeof(char)*(digit + 1));
    char **numbers = (char**)malloc(sizeof(char*)*(npr));
    for(int i = 0; i < npr; i++) {
        numbers[i] = (char*)malloc(sizeof(char)*(digit + 1));
        memset(numbers[i], '\0', sizeof(char)*(digit + 1));
    }
    permutation(between0and9,number,numbers,digit,n,count_pointer,init_digit);
    free(number);
    for (int a=0; a<npr; a++){
        printf("%d番目:%s\n",a,numbers[a]);
    }
    for(int i = 0; i < npr; ++i) {
        free(numbers[i]);
    }
    free(numbers);
}
