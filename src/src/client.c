#include "2015118696_FINAL_global.h"
#include "2015118696_FINAL_client.h"
#define SIZE 30

void error_handling(char *message);
void menu(Connection* c){
	char *a;
	int i = 1;

	while(1){
		printf("*************************************\n");
		printf("|1.Start Blackjack Game with Dealer |\n");
		printf("|2.My Information                   |\n");
		printf("|9.Quit                             |\n");
		printf("*************************************\n");
		printf("input menu:");
		getline((char**)&a, (ssize_t*)&i, stdin);
		switch(a[0]){
			case '1':
				printf("selected 1, send message startgame\n");
				sendmsg_(c->sock, STARTGAME, c->buffer, NULL);
				do_game(c);
				memset(c->u.card, 0, sizeof(c->u.card));
				memset(c->dealer.card, 0, sizeof(c->dealer.card));
				break;
			case '2':
				sendmsg_(c->sock, SHOWINFO, c->buffer, NULL);
				show_info(c);
				break;
			case '9':
				return;
			default:
				printf("default\n");
				break;
		}
	}
}

int main()
{
	int sock;
	struct sockaddr_in serv_addr;
	char username[SIZE];
	char passwd[SIZE];
	char sig[BUFSIZE];
	int read_len = 0;
	int str_len = 0;
	Opcode state = -1;
	Connection c;

	/*generate the socket*/
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	/*initialize address information*/
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORTNUM);

	if(!inet_aton("127.0.0.1", &serv_addr.sin_addr))
		error_handling("Conversion error");

	/*connection request*/
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	c.sock = sock;
	c.gamestate = CONNECTED;
	while(1){
		switch(c.gamestate){
			case CONNECTED:
				printf("connected\n");
				get_login(&c);
				break;
			case LOGGEDIN:
				printf("login success\n");
				menu(&c);
				printf("SEEYA, %s!", c.u.name);
				return 0;
			//case STARTGAME:
				//do_game();
				/*
			case HIT:
				//hit();
				break;
			case STAND:
				//stand();
				break;
			case BET:
				//do_betting();
				
			case DEAL:
			case USERBURST:
			case DEALERBURST:
			case USERBLACKJACK:
			case DEALERBLACKJACK:
			case GAMEOVER:
			case GAMECONTINUE:
			case GAMEEXIT:
			//case SHOWUSERMONEY:
				//게임 결과와 계속 진행할지, 메뉴로 돌아갈지는 do_game 안에서 모두 처리.
			case USERWIN:
				printf("you win!\n");
				printf("press r to restart game\n press q to quit game");
				scanf("%c", &menu);
			case USERLOSE:
				
			case DRAW:
				
			case HITORSTAND:
			case DRAWBLACKJACK:
				
			case ID:
				printf("input username:");
				scanf("%s", username);
				sendmsg_(sock, ID, sig, username);
				break;
			case PW:
				printf("input password:");
				scanf("%s", passwd);
				sendmsg_(sock, PW, sig, passwd);
				break;
			*/
			default: //error
				break;
		}
	}
	close(sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

char* int2card(int card, char *output) {
	int type = (card - 1) / 13;
	int number = (card - 1) % 13 + 1;
	
	switch (type) {
		case 0:
			output[0] = 'S';
			break;
		case 1:
			output[0] = 'C';
			break;
		case 2:
			output[0] = 'H';
			break;
		case 3:
			output[0] = 'D';
			break;
	}
	
	sprintf(output + 1, "%02d", number);
	
	return output;
}

void print_dealer_card(Connection* c) {
	char temp[3];
	int i = 0;
	printf("dealer card:");
	while (c->dealer.card[i] != 0) {
		printf(" %s", int2card(c->dealer.card[i], temp));
		i++;
	}
	printf("\n");
}

void set_dealer_card(Connection* c, char* buf) {
	int len = strlen(buf+1);
	int i = 0;
	for (i = 0; i < len; i++) {
		c->dealer.card[i] = (int)buf[i+1];
	}
}
