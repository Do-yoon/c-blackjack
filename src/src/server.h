void get_betting(Connection*);
void deal_cards(Connection*);
int is_blackjack(Connection*);
void hit(Connection*);
void stand(Connection*);
void get_login(Connection*);
void pay_dividend(int, user*, char*); //int:socket id
void notice_card_state();
void notice_money();
void get_username(int);
void do_game(Connection*);
void init_cardbox(CardBox *);
int get_score(int*);
int get_card(CardBox *);
void game_main(int);
void show_info(Connection*);