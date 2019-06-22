*file
[user] : directory
<username>.statics

*server
[dealer.c]
void get_betting(user*) : 금액 배팅 받기
void deal_cards(user*) : 소켓을 통해 플레이어 클라이언트로 카드를 나누어 준다.
int is_blackjack(user) : 랜덤으로 추출한 처음 2장으로 21을 만들 수 있을 때, 블랙잭으로 배팅 금액을 1.5배 받는다. 딜러와 유저 모두 블랙잭일 경우 비긴다.

void hit(user*)
void stand(user*)
int check_sum_user(user)
-블랙잭을 받지 않았을 경우, 21과 가까운 숫자를 만들기 위해 계속 카드를 받을지(hit), 받지 않을지(stand) 선택할 수 있다. 하지만 받은 패의 총합이 21이 넘으면 플레이어는 죽는다.

int check_sum_dealer()
void burst(int sock, user* p) : 플레이어가 죽었다고 표시하고, 결과값을 계산하여 출력하는 프로그램 호출
-플레이어가 카드를 다 뽑은 후 딜러는 카드의 총합이 17 이상이 될 때까지 무조건 카드를 받는다. 17 이상일 경우 카드를 받을 수 없다. 즉 총합을 17~21을 가지게 되는데 총합이 21이 넘으면 플레이어는 죽는다.(Burst)

void get_result()
void print_result()
-위 과정을 모두 끝낸 후, 점수 계산할 때, 모은 카드의 모든 합으로 계산한다. 카드 각각의 가격은 (K, Q, J, 10 = 10), (A = 1 or 11, 유리한 쪽으로 계산) 나머지는 자신의 숫자 그대로 계산합니다.
-딜러보다 점수가 높으면 승리(배팅 금액만큼 더 받음), 딜러와 점수가 같으면 무승부(배팅 금액 그대로 돌려받는다), 딜러보다 점수가 낮으면 패배(배팅 금액을 잃음)

[operating_game.c]
void pay_dividend(int sock, user*)
-처음에 참가하는 참가자의 돈을 1000달러로 배당
void notice_card_state();
-참가자들의 카드 상태를 알려주는 기능
void notice_money();
-참가자들의 돈을 알려주는 기능

-게임 참가자의 게임 결과 정보(승률, 따간 돈, 최근 접속시간, ...) 는 파일을 통해 관리. 향후 참가자들이 다시 접속하더라도 그 참가자들의 통계 정보를 제공할 수 있도록 함.

*client
[client.c]
-서버에서 보내는 본인과 참가자들의 카드상태를 수신
-카드 요청을 서버에 보냄

<통신 방식>
-다중 클라이언트 접속, 대결가능
