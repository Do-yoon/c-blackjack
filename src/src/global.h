#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <time.h>

#define BUFSIZE 1024
#define CARDSIZE 21
#define PORTNUM 9190

typedef struct _CardBox {
	int cardbox[52];
	int index;
} CardBox;

typedef enum {
	CONNECTED, LOGGEDIN, STARTGAME, HIT, STAND, BET, DEAL, USERBURST, DEALERBURST, USERBLACKJACK, DEALERBLACKJACK, GAMEOVER, GAMECONTINUE, GAMEEXIT, USERWIN, USERLOSE, DRAW, ID, PW, HITORSTAND, DRAWBLACKJACK, WRONGPW, SETPW, SHOWINFO, QUIT, NOBLACKJACK, ISBLACKJACK
} Opcode;

typedef struct user {
	char name[20];
	char pw[20];
	int card[CARDSIZE];
	int bet_money;
	int cardcount;
	int money;
	int recent_access_date;
	int win;
	int lose;
} user;

typedef struct _Connection {
	int sock;
	user u;
	user dealer;
	CardBox cardbox;
	Opcode gamestate;
	char buffer[BUFSIZE];
} Connection;

/*game state를 포함한 전반적인 게임의 정보를 포함하는 구조체를 정의하여 connection에 넣는다*/

void sendmsg_(int sock, Opcode o, char* buffer, char* msg);
char* int2card(int, char*);
void print_dealer_card(Connection*);
void set_dealer_card(Connection*, char*);
/*
typedef struct unit {
	message type;
	
} unit;*/
