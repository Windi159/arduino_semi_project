#define SWITCH A4          // 조이스틱 버튼 (눌리면 LOW)
#define AXIS A5            // 조이스틱 가로축
#define CANNON_SPEED 200   // 포탄 속도 (ms)
#define MAX_CANNON 8       // 포탄 최대 개수

int rows[] = {-1, 10, 11, 12, 13, 14, 15, 16, 17};
int cols[] = {-2, 2, 3, 4, 5, 6, 7, 8, 9};

byte cannon[MAX_CANNON][2]; // cannon[i][0] = rows, cannon[i][1] = cols
byte shoot_cannon = 0;      // 발사된 포탄의 수
byte fighter = 3;           // 전투기 제일 왼쪽 도트 (1 ~ 6)

unsigned long latest_move = 0;
unsigned long latest_shot = 0;
unsigned long latest_fighter = 0;

void setup()
{
  for(int i = 1; i < 9; i++)
  {
    pinMode(rows[i], OUTPUT);
    pinMode(cols[i], OUTPUT);
  }
  pinMode(SWITCH, INPUT_PULLUP);
}

void clear() {
  for(int i = 1; i < 9; i++)
  {
    digitalWrite(rows[i], LOW);
    digitalWrite(cols[i], HIGH);
  }
}

void drawing_fighter() // 비행기 그리기
{
  clear();
  digitalWrite(rows[8], HIGH);
  digitalWrite(cols[fighter], LOW);
  digitalWrite(cols[fighter + 1], LOW);
  digitalWrite(cols[fighter + 2], LOW);
  delay(1);

  clear();
  digitalWrite(rows[7], HIGH);
  digitalWrite(cols[fighter + 1], LOW);
  delay(1);
}

void drawing_cannon(byte num_of_cannon)
{
  digitalWrite(rows[cannon[num_of_cannon][0]], HIGH);
  digitalWrite(cols[cannon[num_of_cannon][1]], LOW);
}

void delete_cannon() // 맨 앞(가장 오래된) 포탄 삭제
{
  for(int i = 1; i < shoot_cannon; i++)
  {
    cannon[i - 1][0] = cannon[i][0];
    cannon[i - 1][1] = cannon[i][1];
  }
  shoot_cannon -= 1;
}

void moving_cannon(byte num_of_cannon)
{
  cannon[num_of_cannon][0] -= 1;
}

void cannon_status() // 맨 앞 포탄이 천장에 닿았으면 삭제
{
  if(shoot_cannon > 0 && cannon[0][0] == 0)
    delete_cannon();
}

void loop_cannons(byte types) // 1 : 이동, 2 : 그리기
{
  for(int i = 0; i < shoot_cannon; i++)
  {
    if(types == 1)
      moving_cannon(i);

    else
    {
      clear();
      drawing_cannon(i);
      delay(1);
    }
  }
}

void shot()
{
  cannon[shoot_cannon][0] = 6; // 전투기 머리(7행) 바로 위
  cannon[shoot_cannon][1] = fighter + 1;
  shoot_cannon += 1;
}

void loop() {
  int x_status = analogRead(AXIS);
  bool is_pushed = !digitalRead(SWITCH);

  // 전투기 이동 (방향이 반대면 < 와 > 를 서로 바꾸기)
  if(millis() - latest_fighter >= 120)
  {
    latest_fighter = millis();

    if(x_status < 300 && fighter > 1)
      fighter -= 1;
    else if(x_status > 700 && fighter < 6)
      fighter += 1;
  }

  // 포탄 발사
  if(is_pushed && shoot_cannon < MAX_CANNON && millis() - latest_shot >= 300)
  {
    latest_shot = millis();
    shot();
  }

  // 포탄 이동
  if(millis() - latest_move >= CANNON_SPEED)
  {
    latest_move = millis();
    loop_cannons(1);
    cannon_status();
  }

  // 그리기
  drawing_fighter();
  loop_cannons(2);
  clear();
}
