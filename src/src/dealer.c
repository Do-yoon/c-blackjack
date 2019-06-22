#include "2015118696_FINAL_global.h"
#include "2015118696_FINAL_server.h"

void do_game(Connection* c){
	int state = -10;
	int fd;
	
	memset(c->u.card, 0, sizeof(c->u.card));
	memset(c->dealer.card, 0, sizeof(c->dealer.card));

	printf("do game server\n");
	
	sendmsg_(c->sock, BET, c->buffer, NULL);
	get_betting(c);
	printf("end get_betting\n");
	
//	sendmsg_(c->sock, DEAL, c->buffer, NULL);
	deal_cards(c);
	printf("end deal cards\n");
	
	read(c->sock, c->buffer, sizeof(c->buffer));
	if((Opcode)c->buffer[0] == ISBLACKJACK){
		state = is_blackjack(c);
		if(state == 1){
			//게임 state를 USERBLACKJACK으로 만든다
			c->gamestate = USERBLACKJACK;
			sendmsg_(c->sock, USERBLACKJACK, c->buffer, NULL);
			c->u.win ++;
			c->u.money += (c->u.bet_money * 1.5);
			save_user(c);
			return;
		}
		else if(state == -1){
			c->gamestate = DRAWBLACKJACK;
			sendmsg_(c->sock, DRAWBLACKJACK, c->buffer, NULL);
			save_user(c);
			return;
		}
		else if(state == 0){
			c->gamestate = NOBLACKJACK;
			sendmsg_(c->sock, NOBLACKJACK, c->buffer, NULL);
			printf("no blackjack\n");
			while(c->gamestate == NOBLACKJACK){
				memset(c->buffer, 0, sizeof(c->buffer));
				read(c->sock, c->buffer, sizeof(c->buffer));
				
				switch((Opcode)c->buffer[0]){
					case HIT:
						hit(c);
						break;
					case STAND:
						stand(c);
						break;
				}
			}
		}
		switch(c->gamestate){
			case USERBURST:
			case DEALERBURST:
			case GAMEOVER:
			case GAMECONTINUE:
			case GAMEEXIT:
				return;
			case USERWIN:
			case USERLOSE:
			case DRAW:
				break;
		}
	}
	
	

	
}

void save_user(Connection* c){
	int fd = open(c->u.name, O_WRONLY, 0644);
	memset(c->buffer, 0, sizeof(c->buffer));
	sprintf(c->buffer, "%s\n%s\n%d\n%d\n%d\n", c->u.name, c->u.pw, c->u.money, c->u.win, c->u.lose);
	write(fd, c->buffer, strlen(c->buffer));
	close(fd);
}

void get_login(Connection* c){
	int fd;
	user u;
	
	while(1){
		memset(&u, 0, sizeof(u));
		sendmsg_(c->sock, ID, c->buffer, NULL);
		memset(c->buffer, 0, sizeof(c->buffer));
		read(c->sock, c->buffer, sizeof(c->buffer));
		if((Opcode)c->buffer[0] == ID){
			printf("on response ID\n");
			strcpy(u.name, (c->buffer)+1);
			
			sendmsg_(c->sock, PW, c->buffer, NULL);
			read(c->sock, c->buffer, sizeof(c->buffer));
			if((Opcode)c->buffer[0] == PW){
				printf("on response PW\n");
				strcpy(u.pw, (c->buffer)+1);
				fd = open(u.name, O_CREAT|O_RDWR, 0644);
				if(read(fd, c->buffer, sizeof(c->buffer)) == 0){
					strcpy(c->u.name, u.name);
					strcpy(c->u.pw, u.pw);
					c->u.money = 1000;
					c->u.win = 0;
					c->u.lose = 0;
					sprintf(c->buffer, "%s\n%s\n1000\n0\n0\n", u.name, u.pw);
					write(fd, c->buffer, strlen(c->buffer));
				}
				else{
					sscanf(c->buffer,"%s\n%s\n%d\n%d\n%d\n", c->u.name, c->u.pw, &c->u.money, &c->u.win, &c->u.lose);
					if(strcmp(c->u.pw, u.pw)){
						sendmsg_(c->sock, WRONGPW, c->buffer, NULL);
						printf("wrong password!\n");
						read(c->sock, c->buffer, sizeof(c->buffer));
						close(fd);
						continue;
					}
				}
				sendmsg_(c->sock, LOGGEDIN, c->buffer, NULL);
				c->gamestate = LOGGEDIN;
				close(fd);
				break;
			}/*
			else{
				sendmsg_(c->sock, WRONGPW, c->buffer, NULL);
			}*/
		}
		/*else { //new user
			sendmsg_(c->sock, SETPW, c->buffer, NULL);
			read(c->sock, c->buffer, sizeof(c->buffer));
			//int fd = open();
			break;
		}*/
	}
}

void sendmsg_(int sock, Opcode o, char* buffer, char* msg){
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = (char)o;
	if(msg != NULL)
		strcpy(buffer+1, msg);
	write(sock, buffer, strlen(buffer));
}

void get_betting(Connection* c){
	c->gamestate = BET;
	while(1){
		memset(c->buffer, 0, sizeof(c->buffer));
		read(c->sock, c->buffer, sizeof(c->buffer));
		if((Opcode)c->buffer[0] == BET){
			c->u.bet_money = atoi((c->buffer)+1);
			printf("bet_money:%d\n",c->u.bet_money);
			return;
		}
		printf("wrong opcode(bet)");
	}
}
void deal_cards(Connection* c){
	char card[3];
	int i;
	
	for(i = 0; i < 2; i++){
		card[i] = (char)get_card((CardBox*)(&(c->cardbox)));
		c->u.card[i] = card[i];
	}
	card[2] = (char)get_card((CardBox*)(&(c->cardbox)));
	c->dealer.card[0] = card[2];
	printf("user got card: %d, %d\n", (int)card[0], (int)card[1]);
	printf("dealer got card: %d\n", (int)card[2]);
	sendmsg_(c->sock, DEAL, c->buffer, card);
	printf("deal_cards\n");
}

int get_card(CardBox *cardbox) {
	if (cardbox->index >= 52) {
		init_cardbox(cardbox);
	}
	return cardbox->cardbox[cardbox->index++];
}

void init_cardbox(CardBox *cardbox) {
	int i;
	int rand_idx1, rand_idx2, temp;
	
	srand(time(NULL));
	
	memset(cardbox, 0x00, sizeof(CardBox));
	
	for (i = 1; i <= 52; i++) {
		cardbox->cardbox[i - 1] = i;
	}

	for (i = 0; i < 100; i++) {
		rand_idx1 = rand() % 52;
		rand_idx2 = rand() % 52;
		temp = cardbox->cardbox[rand_idx1];
		cardbox->cardbox[rand_idx1] = cardbox->cardbox[rand_idx2];
		cardbox->cardbox[rand_idx2] = temp;
	}
}

void debug_cardbox(CardBox *cardbox) {
	for (int i = 1; i <= 52; i++) {
		printf("%2d ", cardbox->cardbox[i - 1]);
		
		if (i > 1 && i % 10 == 0) {
			printf("\n");
		}
	}
	
	printf("\n");
}
int is_blackjack(Connection* c){
	int score_u = get_score(c->u.card);
	int score_d;
	
	/*유저가 블랙잭일 경우 is_blackjack return 1, 아닐 경우 return 0, 딜러만 블랙잭일 경우 return 2, 둘 다 블랙잭일 경우 비겼다는 표시로 -1*/
	if(score_u == 21) {
		c->dealer.card[1] = get_card((CardBox*)(&c->cardbox));
		score_d = get_score(c->dealer.card);
		if (score_d == 21) {
			c->gamestate = DRAWBLACKJACK;
			return -1;
		}
		else {
			c->gamestate = USERBLACKJACK;
			return 1;
		}
	}
	else {
		c->gamestate = NOBLACKJACK;
		return 0;
	}
}

int get_score(int* card){
	int score = 0;
	int i = 0;
	int ace[4]; // 한 유저가 가질 수 있는 ace 갯수 최대 4개.
	int count = 0;
	int temp;
	int temp_card;
	int j = 0;
	
	/*A를 11로 계산하여 버스트가 될 경우 A를 1로 친다.*/
	while(card[i] != 0){
		temp_card = ((card[i] - 1) % 13) + 1;
		if(temp_card >= 10){
			score += 10;
		}
		else if(temp_card == 1){
			ace[count++] = 11;

		}
		else{
			score += temp_card; 
		}
		i++;
	}

	if (count == 0){
		if (score > 21) return -1;
		else return score;
	}
	
	while(j < count){
		temp = 0;

		for(i=0; i<count; i++){
			temp += ace[i];
		}
		if(temp + score > 21){
			if (ace[j] != 1) {
				ace[j] = 1;
			}
			else {
				j++;
			}
		}
		else {
			return (temp + score);
		}
	}

	if (j >= count) return -1; // burst;

	return temp + score;
}

/*
void hit_or_stand(Connection* c){
	memset(c->buffer, 0, sizeof(c->buffer));
	sendmsg_(c->sock, HITORSTAND, c->buffer, NULL);
	read(c->sock, c->buffer, sizeof(c->buffer));
	if((Opcode)c->buffer[0] == HIT)
		hit(c);
	else if((Opcode)c->buffer[0] == STAND){
		c->gamestate = STAND;
		stand(c);
	}
	//else //exception
}*/

void hit(Connection* c){
	int card = get_card((CardBox*)(&(c->cardbox)));
	int score;
	char buf[2];
	
	memset(c->buffer, 0, sizeof(c->buffer));
	
	int i = 0;
	while (c->u.card[i] != 0) {
		i++;
	}
	c->u.card[i] = card;
	
	score = get_score(c->u.card);
	buf[0] = (char)card;
	buf[1] = (char)score;
	
	printf("server send card: %d\n", card);
	printf("user score: %d\n", score);
	if(score == -1){
		sendmsg_(c->sock, USERBURST, c->buffer, buf);
		c->gamestate = USERBURST;
		c->u.money -= c->u.bet_money;
		c->u.lose++;
		save_user(c);
		return;
	}
	sendmsg_(c->sock, HITORSTAND, c->buffer, buf);
}
void stand(Connection* c){
	int i = 1;
	int temp = get_score(c->dealer.card);
	while(temp < 17){
		c->dealer.card[i] = get_card((CardBox*)(&(c->cardbox)));
		temp = get_score(c->dealer.card);
		if(temp == -1){
			break;
		}
		i++;
	}
	get_result(c);
	save_user(c);
}
int get_result(Connection* c){
	/*유저가 이겼을 경우 return 1, 딜러가 이겼을 경우 return 0, 비겼을 경우 -1*/
	char buf[21] = {0};
	int dealer_card_idx = 0;
	while (c->dealer.card[dealer_card_idx] != 0) {
		buf[dealer_card_idx] = (char)c->dealer.card[dealer_card_idx];
		dealer_card_idx++;
	}

	int dealer_score = get_score(c->dealer.card);

	if (dealer_score == 21 && dealer_card_idx == 2) {
		c->gamestate = USERLOSE;
		sendmsg_(c->sock, USERLOSE, c->buffer, buf);
		c->u.money -= c->u.bet_money;
		c->u.lose++;
		return 0;
	}
	else if (dealer_score == -1) {
		c->gamestate = USERWIN;
		sendmsg_(c->sock, USERWIN, c->buffer, buf);
		c->u.money += c->u.bet_money;
		c->u.win++;
		return 1;
	}
	else if(get_score(c->u.card) > get_score(c->dealer.card)){// user wins
		c->gamestate = USERWIN;
		sendmsg_(c->sock, USERWIN, c->buffer, buf);
		c->u.money += c->u.bet_money;
		c->u.win++;
		return 1;
	}
	else if(get_score(c->u.card) < get_score(c->dealer.card)){
		c->gamestate = USERLOSE;
		sendmsg_(c->sock, USERLOSE, c->buffer, buf);
		c->u.money -= c->u.bet_money;
		c->u.lose++;
		return 0;
	}
	else if(get_score(c->u.card) == get_score(c->dealer.card)){
		c->gamestate = DRAW;
		sendmsg_(c->sock, DRAW, c->buffer, buf);
		return -1;
	}
}

char* int2card(int card, char* output) {
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

void show_info(Connection* c){
	char buf[BUFSIZE];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s\n%s\n%d\n%d\n%d\n", c->u.name, c->u.pw, c->u.money, c->u.win, c->u.lose);
	sendmsg_(c->sock, SHOWINFO, c->buffer, buf);
}
