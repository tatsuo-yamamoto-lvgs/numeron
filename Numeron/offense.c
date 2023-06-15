#include <sys/socket.h> //socketのためのモジュール
#include <netinet/in.h> //ソケットの作成や接続、データの送受信などに必要な、関数やでデータの型が含まれたモジュール
#include <arpa/inet.h>  //IPアドレスを表すための方や構造体が含まれているモジュール
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "offense.h"

void assume(char *assume_number, char diffense_message[], int int_digit, char *numbers[],int count){
    strcpy(assume_number,numbers[count]);
    // diffense_messageを読んで、違うやつを候補群からぬく
        // iEの処理:
            // jBの処理:
    // numbers の先頭から一つ候補をassume_numberに格納する
}

int transmission(int sock){
    // diffense側から桁数を受け取る
    char digit[2];
    char diffense_message[23] = {"0E0B\0"};
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
    
    createPermutationList(int_digit, n, numbers);
    
    // 受け取った桁数から重複のないn桁の数字の配列を作成
    char *assume_number = (char*)malloc(sizeof(char)*(int_digit+1));
    int count = 0;
    while (1){
        // 数字を出す関数をここに
        assume(assume_number,diffense_message,int_digit,numbers,count);
        count++;
        // 桁数分の数列をdefense側に送る

        int write_bytes = write(sock, assume_number, sizeof(assume_number));
        if (write_bytes == -1){
            printf("write error\n");
            break;
        }
        int read_bytes = read(sock, diffense_message, sizeof(diffense_message));
        if (read_bytes == -1){
            printf("diffense_message:%s\n",diffense_message);
            printf("read error(%s)\n",strerror(errno));
            break;
        }
        if (read_bytes == 5){
            printf("%s\n",diffense_message);
        }
        else{
            printf("correct! %s times\n",diffense_message);
            break;
        }
    }
    free(assume_number);
    for(int i = 0; i < npr; ++i) {
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
    }
    printf("Connection success!\n");
    printf("File descriptor: %d\n", sock);

    /*データのやり取り！ソケットは接続済みだ！*/
    transmission(sock);
    close(sock);
}
