#include <sys/socket.h> //socketのためのモジュール
#include <netinet/in.h> //ソケットの作成や接続、データの送受信などに必要な、関数やでデータの型が含まれたモジュール
#include <arpa/inet.h>  //IPアドレスを表すための方や構造体が含まれているモジュール
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "offense.h"

void assume(char *assume_number, char defense_message[], int int_digit, char *numbers[],int count,int npr,int size, int *last_array_num_pointer){
    int graveCountSum=0;
    int graveCount;
    int eats = atoi(&defense_message[0]);
    int byts = atoi(&defense_message[2]);
    if ((eats == 0) && (byts ==0)){
        for (int i = 0; i < int_digit; i++){
            graveCount = 0;
            for (int j = count; j < *last_array_num_pointer; j++){
                if (strchr(numbers[j], assume_number[i]) == NULL) {
                    memcpy(numbers[count+graveCount], numbers[j],int_digit);
                    ++graveCount;
                }
            }
        *last_array_num_pointer =count + graveCount;
        }
    } else if ((eats == 0) && (byts != 0)){
        int eatsZeroCount = 0;
        char** tmp_array = (char**)malloc(sizeof(char*)*(npr));
        for(int i = 0; i < npr; i++) {
            tmp_array[i] = (char*)malloc(sizeof(char)*(int_digit + 1));
            memset(tmp_array[i], '\0', sizeof(char)*(int_digit + 1));
        }
        for (int i = count; i < *last_array_num_pointer; i++){
        int check =0;
            for (int j = 0; j < int_digit; j++){
                if (numbers[i][j] == assume_number[j]) {
                    ++check;
                }
            }
            if (check == 0){
                memcpy(tmp_array[eatsZeroCount], numbers[i], int_digit);
                ++eatsZeroCount;
            }
        }
        *last_array_num_pointer = count + eatsZeroCount;
        for (int i = count; i < count+eatsZeroCount; i++){
            memcpy(numbers[i], tmp_array[i-count], int_digit);
        }


        int bytsCount = 0;
        for (int i = count; i < *last_array_num_pointer; i++){
        int check =0;
            for (int digit_position = 0; digit_position < int_digit; digit_position++) {
                if (strchr(numbers[i], assume_number[digit_position]) != NULL){
                    ++check;
                }
            }
            if (check == byts){
                memcpy(tmp_array[bytsCount], numbers[i], int_digit);
                ++bytsCount;
            }
        }
        *last_array_num_pointer = count + bytsCount;
        for (int i = count; i < count+bytsCount; i++){
            memcpy(numbers[i], tmp_array[i-count], int_digit);
        }
        for(int i = 0; i < npr; i++) {
            free(tmp_array[i]);
        }
        free(tmp_array);
        



    } else if(eats <= int_digit){
        int eatsbytsCount = 0;
        char** tmp_array = (char**)malloc(sizeof(char*)*(npr));
        for(int i = 0; i < npr; i++) {
            tmp_array[i] = (char*)malloc(sizeof(char)*(int_digit + 1));
            memset(tmp_array[i], '\0', sizeof(char)*(int_digit + 1));
        }
        for (int i = count; i < *last_array_num_pointer; i++){
        int check =0;
            for (int digit_position = 0; digit_position < int_digit; digit_position++) {
                if (strchr(numbers[i], assume_number[digit_position]) != NULL){
                    ++check;
                }
            }
            if (check == (eats+byts)){
                memcpy(tmp_array[eatsbytsCount], numbers[i], int_digit);
                ++eatsbytsCount;
            }
        }
        *last_array_num_pointer = count + eatsbytsCount;
        for (int i = count; i < count+eatsbytsCount; i++){
            memcpy(numbers[i], tmp_array[i-count], int_digit);
        }
    
        int eatsCount = 0;
        for (int i = count; i < *last_array_num_pointer; i++){
        int check =0;
            for (int digit_position = 0; digit_position < int_digit; digit_position++) {
                if (numbers[i][digit_position] == assume_number[digit_position]){
                    ++check;
                }
            }
            if (check == eats){
                memcpy(tmp_array[eatsCount], numbers[i], int_digit);
                ++eatsCount;
            }
        }
        *last_array_num_pointer = count + eatsCount;
        for (int i = count; i < count+eatsCount; i++){
            memcpy(numbers[i], tmp_array[i-count], int_digit);
        }
        for(int i = 0; i < npr; i++) {
            free(tmp_array[i]);
        }
        free(tmp_array);
    }
    strcpy(assume_number,numbers[count]);
}

int transmission(int sock){
    // defense側から桁数を受け取る
    char digit[2];
    char defense_message[7] = "999999";
    if(read(sock, digit, sizeof(digit)) == -1){
        printf("Failed to receive digits\n");
    }
    printf("受け取った桁数：%s\n",digit);
    // 候補となるnumberを作る
    int n = 9;
    int int_digit = atoi(digit);
    int npr = factorial(int_digit, n);

    char **numbers = (char**)malloc(sizeof(char*)*(npr));
    for(int i = 0; i < npr; i++) {
        numbers[i] = (char*)malloc(sizeof(char)*(int_digit + 1));
        memset(numbers[i], '\0', sizeof(char)*(int_digit + 1));
    }
    int size = sizeof(numbers) / sizeof(numbers[0]); 
    
    createPermutationList(int_digit, n, numbers);
    
    // 受け取った桁数から重複のないn桁の数字の配列を作成
    char *assume_number = (char*)malloc(sizeof(char)*(int_digit+1));
    int count = 0;
    int last_array_num;
    int *last_array_num_pointer;
    last_array_num_pointer = &last_array_num;
    *last_array_num_pointer = npr;
    while (1){
        // 数字を出す関数をここに
        assume(assume_number,defense_message,int_digit,numbers,count,npr,size,last_array_num_pointer);
        count++;
        // 桁数分の数列をdefense側に送る

        int write_bytes = write(sock, assume_number, sizeof(assume_number));
        if (write_bytes == -1){
            printf("write error\n");
            break;
        }
        int read_bytes = read(sock, defense_message, sizeof(defense_message));
        if (read_bytes == -1){
            printf("defense_message:%s\n",defense_message);
            printf("read error(%s)\n",strerror(errno));
            break;
        }
        if (read_bytes == 5){
            printf("%s\n",defense_message);
        }
        else{
            printf("correct! %s times\n",defense_message);
            break;
        }
    }
    free(assume_number);
    for(int i = 0; i < npr; i++) {
        free(numbers[i]);
    }
    free(numbers);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3){
        perror("command line error\n");
        exit(EXIT_FAILURE);
    }
    /*ソケットを作成*/
    int sock;   //ここでsocket用のメモリ領域を確保
    struct sockaddr_in s_addr;  //s_addrにエイリアス
    sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock == -1) {
        perror("socket error\n");
        exit(EXIT_FAILURE);
    }

    /*初期化!構造体はすっかり前のことを忘れた！*/
    memset(&s_addr, 0, sizeof(struct sockaddr_in));

    /*設定！構造体にデータが入った！*/
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons((unsigned short)atoi(argv[2]));
    s_addr.sin_addr.s_addr = inet_addr(argv[1]);

    /*コネクト！クライアントはサーバに接続要求をしてる！*/
    printf("Connecting wait...\n");
    if (connect(sock, (struct sockaddr*)&s_addr, sizeof(struct sockaddr_in)) == -1) {
        printf("connect error\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Connection success!\n");
    printf("File descriptor: %d\n", sock);

    /*データのやり取り！ソケットは接続済みだ！*/
    transmission(sock);
    close(sock);
}
