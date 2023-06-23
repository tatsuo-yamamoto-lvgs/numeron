#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void judgment(char *str, char offense_message[], char random_number[], int digit){
    int k, l;
    int s = 0;
    int p = 0;
    for (k = 0; k < digit; k++){
        for (l = 0; l < digit; l++){
            if (offense_message[k] == random_number[l] ){
                if(k == l){++s;}
                else {++p;}
            }
        }
    }
    sprintf(str,"%dE%dB", s, p);
    // printf("%s\n",str);
}

void shuffle(char array[], unsigned int size) {
        unsigned int i, j;
        int tmp;

        /* シャッフル範囲の末尾を設定 */
        i = size - 1;

        while (i > 0) {
        /* シャッフル範囲（0〜i）からランダムに１つデータを選択 */
        j = rand() % (i + 1);

        /* ランダムに決めたデータとシャッフル範囲の末尾のデータを交換 */
        tmp = array[j];
        array[j] = array[i];
        array[i] = tmp;

        /* シャッフル範囲を狭める */
        i--;
        } 
}

int transmission(int c_sock, int digit, char char_digit, int repeat_number){
    int i;
    char array[11] = {"0123456789\0"};
    char *random_number = (char*)malloc(sizeof(char)*(digit+1));
    if(random_number == NULL){
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    char str[5] = "9999";//iEjB入れる用


    srand((unsigned int)time(NULL)+repeat_number);

    /* arrayの中身をシャッフル */
    shuffle(array, (sizeof(array) / sizeof(array[0])-1));
    /* 重複なしの乱数を生成（取得）*/
    for (i = 0; i < digit; i++) {
        random_number[i] = array[i];
    }
    random_number[digit] = '\0';
    for (int j = 0; j <= digit; j++){
        printf("%c",random_number[j]);
    }printf("\n");
    char *offense_message = (char*)malloc(sizeof(char)*(digit+1));
    if(offense_message == NULL){
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }

    int trial = 0;
    int trial_digit = 0;
    char goal_str[5];
    sprintf(goal_str, "%dE0B", digit);
    while (1){
        trial ++;
        //offense側からの送信を受け取る
        int read_bytes = read(c_sock, offense_message, sizeof(offense_message));
        if (read_bytes == -1){
            printf("read error\n");
            break;
        }
        printf("offense_messageは:%s\n",offense_message);
        //ここに文字列に対してiEjBを返す関数を書いて
        judgment(str, offense_message, random_number, digit);
        if (strcmp(str, goal_str)==0){
            int tmp = trial;
            while (tmp != 0){
                tmp /= 10;
                ++trial_digit;
                }
            char* correct_message =  (char*)malloc(sizeof(char)*(1+trial_digit));
            // memset(&correct_message, '\0', sizeof(correct_message));
            sprintf(correct_message,  "%d", trial);
            printf("%s\n",correct_message);
            int write_bytes = write(c_sock, correct_message, sizeof(correct_message));
            if (write_bytes == -1){
            printf("write error\n");
            }
            free(correct_message);
            break;
        }else{
            int write_bytes = write(c_sock, str, sizeof(str));
            if (write_bytes == -1){
            printf("write error\n");
            break;
            }
        }
    }
    free(random_number);
    free(offense_message);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 5){
        perror("command line error\n");
        exit(EXIT_FAILURE);
    }
    int digit = atoi(argv[3]);
    /*ソケットを作成*/
    int sock, c_sock;   //ここでsocket用のメモリ領域を確保
    struct sockaddr_in s_addr;  //サーバ用のアドレス準備
    struct sockaddr_in c_addr;  //クライアント側のアドレス準備
    socklen_t c_addr_length = sizeof(c_addr);
    sock = socket(AF_INET,SOCK_STREAM,0);    //IPv4 UDPプロトコルを使うことを設定（0 はSOCK＿DGRAMに対して自動でUDPに設定してくれる）
    if (sock == -1) {
        perror("socket error\n");
        exit(EXIT_FAILURE);
    }

    /*初期化!構造体はすっかり前のことを忘れた！*/
    memset(&s_addr, 0, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons((unsigned short)atoi(argv[2]));
    s_addr.sin_addr.s_addr = inet_addr(argv[1]);

    /*バインド！アドレスが設定された！*/
    if(bind(sock, (struct sockaddr*)&s_addr, sizeof(s_addr)) == -1){
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }
    if(listen(sock, 3) == -1){
        perror("listen error\n");
        exit(EXIT_FAILURE);
    }
    
    while(1){
    printf("Waiting connect...\n");
    c_sock = accept(sock, (struct sockaddr*)&c_addr, &c_addr_length);
    if (c_sock == -1){
        perror("accept error\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    else if(c_sock >=0){
        printf("Connecting success!\n");
        break;
    }
    }
    printf("char_digitの値は:%c\n",*argv[3]);
    if(write(c_sock, argv[3], strlen(argv[3])) == -1){
        printf("Failed to send digits\n");
    }
    /*データのやり取り！ソケットは接続済みだ！*/
    int game_count = atoi(argv[4]);
    for(int repeat_num = 0; repeat_num < game_count;repeat_num++) {
        transmission(c_sock, digit, *argv[3],repeat_num);
    }
    close(sock);
    close(c_sock);
}
