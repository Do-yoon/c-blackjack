#include "2015118696_FINAL_global.h"
#include "2015118696_FINAL_client.h"

void sendmsg_(int sock, Opcode o, char* buffer, char* msg){
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = (char)o;
	if(msg != NULL)
		strcpy(buffer+1, msg);
	write(sock, buffer, strlen(buffer));
}

void get_login(Connection* c){
	char id[20];
	char pw[20];
	
	while(1){
		read(c->sock, c->buffer, sizeof(c->buffer));
		if((Opcode)c->buffer[0] == ID){
			memset(id, 0, sizeof(id));
			memset(pw, 0, sizeof(pw));
			printf("on request ID\n");
			memset(c->buffer, 0, sizeof(c->buffer));
			gets(id);
			sendmsg_(c->sock, ID, c->buffer, id);
			strcpy(c->u.name, id);
			read(c->sock, c->buffer, sizeof(c->buffer));
			if((Opcode)c->buffer[0] == PW){
				printf("on request PW\n");
				gets(pw);
				
				sendmsg_(c->sock, PW, c->buffer, pw);
				read(c->sock, c->buffer, sizeof(c->buffer));
				if((Opcode)c->buffer[0] != WRONGPW){
					c->gamestate = LOGGEDIN;
					return;
				}
				else{
					printf("wrong password!\n");
					sendmsg_(c->sock, WRONGPW, c->buffer, NULL);
					continue;
				}
				
			}
		}
	}
}

void get_betting(Connection* c){
	char money[20];
	printf("INPUT betting money:");
	gets(money);
	sendmsg_(c->sock, BET, c->buffer, money);
	c->u.bet_money = atoi(money);
}

void deal_cards(Connection* c){
	char temp[3], temp2[3];
	c->u.card[0] = c->buffer[1];
	c->u.card[1] = c->buffer[2];
	c->dealer.card[0] = c->buffer[3];
	printf("user got card: %s %s\n", int2card((int)c->u.card[0], temp), int2card((int)c->u.card[1], temp2));
	printf("dealer got card: %s\n", int2card((int)c->dealer.card[0], temp));
	sendmsg_(c->sock, ISBLACKJACK, c->buffer, NULL); // for getting blackjack state
}

void do_game(Connection* c){
	int state = -10;
	int hitorstand;
	int user_card_idx = 0;
	int i;
	char temp[3];
	char str[10];
	
	memset(c->u.card, 0, sizeof(c->u.card));
	memset(c->dealer.card, 0, sizeof(c->dealer.card));

	printf("do game client\n");
	while(1){
		read(c->sock, c->buffer, sizeof(c->buffer));
		switch(c->gamestate = (Opcode)c->buffer[0]){
			case BET:
				get_betting(c);
				break;
			case DEAL:
				deal_cards(c);
				user_card_idx = 2;
				break;
			case HITORSTAND:
				c->u.card[user_card_idx++] = (int)c->buffer[1];
				printf("user card:");
				for(i = 0; i < user_card_idx; i++){
					printf(" %s", int2card(c->u.card[i], temp));
				}
				printf("\n");
			case NOBLACKJACK:
				printf("1.hit 2.stand");
				gets(str);
				hitorstand = atoi(str);
				if(hitorstand == 1){
					sendmsg_(c->sock, HIT, c->buffer, NULL);
				}
				else{
					sendmsg_(c->sock, STAND, c->buffer, NULL);
				}
				break;
			case USERBLACKJACK:
				set_dealer_card(c, c->buffer);
				print_dealer_card(c);
				printf("User BlackJack\n");
				return;
			case DRAWBLACKJACK:
				set_dealer_card(c, c->buffer);
				print_dealer_card(c);
				printf("Draw BlackJack\n");
				return;
			case USERWIN:
				set_dealer_card(c, c->buffer);
				print_dealer_card(c);
				printf("User Win\n");
				return;
			case USERLOSE:
				set_dealer_card(c, c->buffer);
				print_dealer_card(c);
				printf("User Lose\n");
				return;
			case USERBURST:
				c->u.card[user_card_idx++] = (int)c->buffer[1];
				printf("user card:");
				for(i = 0; i < user_card_idx; i++){
					printf(" %s", int2card(c->u.card[i], temp));
				}
				printf("\n");
				printf("User Burst\n");
				return;
			case DRAW:
				set_dealer_card(c, c->buffer);
				print_dealer_card(c);
				printf("Draw\n");
				return;
		}
	}
}

void show_info(Connection* c){
	read(c->sock, c->buffer, sizeof(c->buffer));
	sscanf((c->buffer)+1, "%s\n%s\n%d\n%d\n%d\n", c->u.name, c->u.pw, &c->u.money, &c->u.win, &c->u.lose);
	printf("username: %s\n", c->u.name);
	printf("password: %s\n", c->u.pw);
	printf("your money: %d\n", c->u.money);
	printf("win: %d\n", c->u.win);
	printf("lose: %d\n", c->u.lose);
	
}
