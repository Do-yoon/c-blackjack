#include "2015118696_FINAL_global.h"
#include "2015118696_FINAL_server.h"

void menu(Connection* c){
	char a;

	memset(c->buffer, 0, sizeof(c->buffer));
	
	while(1){
		read(c->sock, c->buffer, sizeof(c->buffer));
		switch((Opcode)c->buffer[0]){
			case STARTGAME:
				printf("received message: startgame\n");
				do_game(c);
				memset(c->u.card, 0, sizeof(c->u.card));
				memset(c->dealer.card, 0, sizeof(c->dealer.card));
				break;
			case SHOWINFO:
				show_info(c);
				break;
			
			case QUIT:
				return;
		}
	}
}

void game_main(int sock){
	Connection c;
	c.sock = sock;
	/*for test*/
//	strcpy(c.u.name, "abc");
//	strcpy(c.u.pw, "abc");
	c.gamestate = CONNECTED;
	init_cardbox(&c.cardbox);
	debug_cardbox(&c.cardbox);
	
	while(1){
		switch(c.gamestate){
			case CONNECTED:
				printf("connected\n");
				printf("please login\n");
				get_login(&c);
				break;
			case LOGGEDIN:
				printf("login success\n");
				menu(&c);
				break;
			case SHOWINFO:
			default: //error
				break;
		}
	}
}

int main(){
	
// 	int temp1[6] = {1, 2, 3, 4, 5, 0}; // 15
// 	int temp2[5] = {1, 2, 3, 4, 0}; // 20
// 	int temp3[4] = {1, 2, 3, 0}; // 16
// 	int temp4[3] = {1, 2, 0}; // 13
//	int temp5[4] = {48, 14, 13, 0}; // 20
	
// 	printf("15 - %d\n", get_score(temp1));
// 	printf("20 - %d\n", get_score(temp2));
// 	printf("16 - %d\n", get_score(temp3));
// 	printf("13 - %d\n", get_score(temp4));
//	printf("20 - %d\n", get_score(temp5));
	
//	return 0;
	
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
//	char buffer[BUFSIZE];
	int fd;
	Connection c;
	
	/*generate server socket*/
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	if(serv_sock == -1){
		perror("socket() error!");
		exit(1);
	}

	
	/*initialize address information*/
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORTNUM);

	/*allocate address information*/
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		perror("bind() error!");
		exit(1);
	}
	/*allow connection request*/
	if(listen(serv_sock, 5) == -1){
		perror("listen() error!");
		exit(1);
	}
	
	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size); //accept connection request
	if(clnt_sock == -1){
		perror("accept() error!");
		exit(1);
	}
	
	
	/*connected, do game*/
		game_main(clnt_sock);

	
	close(serv_sock);
	close(clnt_sock);
	
	return 0;
}


