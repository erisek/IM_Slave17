#include <Wire.h>
#include <stdlib.h>
#include <SPI.h>

int cmd;

//  ピン番号定義
#define SS      10  // → CS
#define MOSI    11  // → SDI
#define MISO    12  // ← SDO
#define SCK     13  // → SCK
#define BUSY    9   // ← BUSY
#define ALARM   8   // ← ALARM
#define RESET   7   // → STANDBY RESET
#define LED     4   // → STANDBY RESET

#define BAUD_RATE   9600  //  通信ボーレート
#define MOTOR_NUM   17   //  モーターの個数（連結するので）

#define STALL_TH  8        //  ストール検知の電流しきい値

#define MAKI_MOTOR_INDEX          0x1FFFF

#define ALL_STEPCOUNT         90000
#define STARTUP_UP_STEPCOUNT  10000 //ここ変える
#define MOVE_DURATION         0 //  msec

#define SHORT_STEPCOUNT   100000
#define LONG_STEPCOUNT    80000
#define MEDIUM_STEPCOUNT  70000

int indexMakis[20];

struct MakiParams {
  bool enabled = true;
  bool isMakiMax; //  上にあるときtrue 下にあるときfalse（移動中の場合は移動先の状態になってる）
  unsigned long time;
  float eachMakiSu;
};

struct MakiParams Maki[MOTOR_NUM];

enum ActionMode {
  ActionMode_Random = 0,
  ActionMode_Kisoku,
  ActionMode_Nagare,
  ActionMode_Kougo,
  ActionMode_Manual,
  ActionMode_Wait
};

float downPos = 0;

ActionMode curMode = ActionMode_Manual;
String receivedMessage = "";
//float MakiHeight = 20.0;

void setupMotorDriver() {

  //  ピンの設定
  pinMode(SS, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  digitalWrite(SS, HIGH);

  pinMode(BUSY, INPUT);
  pinMode(ALARM, INPUT);



  pinMode(RESET, OUTPUT);

  //  SPI通信の設定
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE3);

  //  リセットかける
  delay(500);
  L6470_resetWithResetPin();

  //  初期化処理
  SetupMotorParameters();
}

void resetRandomStates() {
  unsigned long offset = millis();

  for(int i=0;i<MOTOR_NUM;i++) {
    Maki[i].isMakiMax = false;
    Maki[i].time = random(20000) + 1000 + offset;
  }
  Maki[0].eachMakiSu = SHORT_STEPCOUNT;
  Maki[1].eachMakiSu = LONG_STEPCOUNT;
  Maki[2].eachMakiSu = LONG_STEPCOUNT;
  Maki[3].eachMakiSu = LONG_STEPCOUNT;
  Maki[4].eachMakiSu = LONG_STEPCOUNT;
  Maki[5].eachMakiSu = LONG_STEPCOUNT;
  Maki[6].eachMakiSu = LONG_STEPCOUNT;
  Maki[7].eachMakiSu = LONG_STEPCOUNT;
  Maki[8].eachMakiSu = LONG_STEPCOUNT;
  Maki[9].eachMakiSu = SHORT_STEPCOUNT;
  Maki[10].eachMakiSu = LONG_STEPCOUNT;
  Maki[11].eachMakiSu = SHORT_STEPCOUNT;
  Maki[12].eachMakiSu = SHORT_STEPCOUNT;
  Maki[13].eachMakiSu = LONG_STEPCOUNT;
  Maki[14].eachMakiSu = LONG_STEPCOUNT;
  Maki[15].eachMakiSu = LONG_STEPCOUNT;
  Maki[16].eachMakiSu = SHORT_STEPCOUNT;

}


void setup() {

  pinMode(LED, OUTPUT);

  // setupMotorDriver();
  //
  //     //  起動時に昇降モジュールを最下部まで移動
  // setZeroPointAll();
  //
  //     //  random
  // randomSeed(analogRead(0));
  //
  //     //  status リセット
  // resetRandomStates();

  // Start the I2C Bus as Slave on address 8
  Wire.begin(8);
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
}


void loop() {

    switch(curMode) {
    case ActionMode_Random:
    RandomModeLoop();
    break;

    case ActionMode_Kisoku:
    KisokuModeLoop();
    break;

    case ActionMode_Nagare:
    NagareLoop();
    break;

    case ActionMode_Kougo:
    KougoLoop();
    break;

    case ActionMode_Manual:
    //  何もしない
    break;

    case ActionMode_Wait:
    //  何もしない
    break;
  }

}


void receiveEvent(int a){

  while(Wire.available()){
    cmd = Wire.read();
  }
  action_motors();
}

void action_motors(void){

  if(cmd == 0){

    digitalWrite(LED, LOW);   // 点灯
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);

       for(int i=0;i<MOTOR_NUM;i++) {
        if(Maki[i].enabled && !Maki[i].isMakiMax) {
        float makimakiSu = Maki[i].eachMakiSu;
        moveToPosition(1<<i, makimakiSu);
       }
      }

      delay(20000);

       L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 20);
       moveToPosition(MAKI_MOTOR_INDEX, downPos);

       delay(5000);

  }else if(cmd == 1){

    digitalWrite(LED, HIGH);   // 点灯
    setupMotorDriver();

        //  起動時に昇降モジュールを最下部まで移動
    setZeroPointAll();

        //  random
    randomSeed(analogRead(0));

        //  status リセット
    resetRandomStates();

  }else if(cmd == 2){

    curMode = ActionMode_Random;

  }else if(cmd == 3){

    L6470_softstop();

    curMode = ActionMode_Wait;

    }else if(cmd == 4){

      curMode = ActionMode_Kisoku;

    }else if(cmd == 5){

      curMode = ActionMode_Nagare;

    }else if(cmd == 6){

      curMode = ActionMode_Kougo;

    }

  delay(20);

  }

  void RandomModeLoop() {

    // Random01Loop();
    // Random02Loop();
    // Random03Loop();

    int rannum = random(21);
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
    moveToPosition(0x52, LONG_STEPCOUNT);
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 5);
    moveToPosition(0x52, downPos);

    delay(random(10000));

    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
    moveToPosition(0x8D69, LONG_STEPCOUNT);
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 5);
    moveToPosition(0x8D69, downPos);

    delay(random(10000));

    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
    moveToPosition(0x44, LONG_STEPCOUNT);
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 5);
    moveToPosition(0x44, downPos);

    delay(random(10000));

    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
    moveToPosition(0x11104, LONG_STEPCOUNT);
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 5);
    moveToPosition(0x11104, downPos);

    delay(random(10000));
//     unsigned long now = millis();
//     L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
// //     巻き取り巻きだし
//
//     for(int i=0;i<MOTOR_NUM;i++) {
//       if(Maki[i].time < now) {
//           Maki[i].isMakiMax = !Maki[i].isMakiMax;
//           Maki[i].time = now + Maki[i].isMakiMax? 0:random(100000);
// //          L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, Maki[i].isMakiMax? 10:5);
//            float makimakiSu = random(Maki[i].eachMakiSu/2,Maki[i].eachMakiSu);
//            moveToPosition(1<<i, Maki[i].isMakiMax? makimakiSu:downPos);
//     //
//       }
//     }

  }

  void Random01Loop() {
    unsigned long now = millis();
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
    //  巻き取り巻きだし

    for(int i=0;i<6;i++) {
      if(Maki[i].time < now) {
          Maki[i].isMakiMax = !Maki[i].isMakiMax;
          Maki[i].time = now + Maki[i].isMakiMax? 0:random(30000,40000);
//          L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, Maki[i].isMakiMax? 10:5);
           float makimakiSu = random(Maki[i].eachMakiSu/2,Maki[i].eachMakiSu);
           moveToPosition(1<<i, Maki[i].isMakiMax? makimakiSu:downPos);
    //
      }
    }
  }

  void Random02Loop() {
    unsigned long now = millis();
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
    //  巻き取り巻きだし

    for(int i=6;i<12;i++) {
      if(Maki[i].time < now) {
          Maki[i].isMakiMax = !Maki[i].isMakiMax;
          Maki[i].time = now + Maki[i].isMakiMax? 0:random(30000,40000);
//          L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, Maki[i].isMakiMax? 10:5);
           float makimakiSu = random(Maki[i].eachMakiSu/2,Maki[i].eachMakiSu);
           moveToPosition(1<<i, Maki[i].isMakiMax? makimakiSu:downPos);
    //
      }
    }
  }

  void Random03Loop() {
    unsigned long now = millis();
    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
    //  巻き取り巻きだし

    for(int i=13;i<17;i++) {
      if(Maki[i].time < now) {
          Maki[i].isMakiMax = !Maki[i].isMakiMax;
          Maki[i].time = now + Maki[i].isMakiMax? 0:random(30000,40000);
//          L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, Maki[i].isMakiMax? 10:5);
           float makimakiSu = random(Maki[i].eachMakiSu/2,Maki[i].eachMakiSu);
           moveToPosition(1<<i, Maki[i].isMakiMax? makimakiSu:downPos);
    //
      }
    }
  }

  void KisokuModeLoop() {

    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 15);

    for(int i=0;i<MOTOR_NUM;i++) {
      if(Maki[i].enabled) {
      float makimakiSu = Maki[i].eachMakiSu;
      moveToPosition(1<<i, LONG_STEPCOUNT);
      }
    }


    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 5);

    downPos = downPos+10;

    for(int i=0;i<MOTOR_NUM;i++) {
      if(Maki[i].enabled) {
      moveToPosition(1<<i, downPos);
      }
    }

    delay(10000);

  }

  void NagareLoop(){

    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 15);

    for(int i=0;i<MOTOR_NUM;i++) {
      if(Maki[i].enabled) {
      float makimakiSu = Maki[i].eachMakiSu;
      moveToPosition(1<<i, makimakiSu);
      delay(1000);
      }
    }


    L6470_softstop();

    L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 5);
    for(int i=0;i<MOTOR_NUM;i++) {
      if(Maki[i].enabled) {
      float makimakiSu = Maki[i].eachMakiSu;
      moveToPosition(1<<i, downPos);
      downPos= downPos+10;
      delay(1000);
      }
    }


  }

void KougoLoop(){

  L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
  moveToPosition(0x15555, LONG_STEPCOUNT);
  L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 5);
  moveToPosition(0x15555, downPos);

  delay(1000);

  L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 10);
  moveToPosition(0xAAAA, LONG_STEPCOUNT);
  L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 5);
  moveToPosition(0xAAAA, downPos);

  delay(5000);



}

void runUntilStallIndex(int index, int dia, unsigned long speed) {

  L6470_setparam_stallth_index(index, STALL_TH);
  L6470_run_index(index, dia, speed);
  delay(5);
  while(!digitalRead(ALARM)) {
    L6470_getStatus();  //  getStatusを呼ばないとflagピンがアップデートされない
  }
  while(digitalRead(ALARM)) {
    L6470_getStatus();  //  getStatusを呼ばないとflagピンがアップデートされない
  }
  L6470_hardstop_index(index);
}

//////////////////////////////////////////////////////////////////////////////
//  ストールするまで回すを、順にすべてのモーターについて実行
void runUntilStallAll(int dia, unsigned long speed) {
  for(int i=1;i<MOTOR_NUM;i++) {
    Serial.println("run : "+String(i+1));
    runUntilStallIndex(1<<i, dia, speed);
    Serial.println("stalled : "+String(i+1));
  }
}

//////////////////////////////////////////////////////////////////////////////
//  指定した位置に動かす 0.0~1.0で、1.0がワイヤーが一番長い、0がワイヤーがない
void moveToPosition(int index, float eachstepcount) {
  unsigned long pos = ALL_STEPCOUNT - eachstepcount;
  L6470_goto_index_noWait(index, pos);
}

//////////////////////////////////////////////////////////////////////////////
//  指定した量動かす
void moveStepsIndex(int index, int dia, unsigned long steps) {
  L6470_move_index(index, dia, steps);
  L6470_softstop();
}

//////////////////////////////////////////////////////////////////////////////
//  速度変える
void moveSpeedIndex(int index, unsigned long speed) {
  L6470_setparam_maxspeed_index(index, speed);
}

//////////////////////////////////////////////////////////////////////////////
//  指定した位置に動かす 0.0~1.0で、1.0がワイヤーが一番長い、0がワイヤーがない
void popping(int index, float eachstepcount) {
  unsigned long pos = ALL_STEPCOUNT - eachstepcount;
  unsigned long posp = ALL_STEPCOUNT;

  L6470_setparam_maxspeed_index(index, 5);
  L6470_goto_index_noWait(index, pos);
  delay(500);
  L6470_setparam_maxspeed_index(index, 15);
  L6470_goto_index_noWait(index, posp);
}


//////////////////////////////////////////////////////////////////////////////
//  起動時に使うすべての昇降ユニットを最下部に移動
void setZeroPointAll() {
  //  最初にロック回避のため上方向に少しだけ回す
  // moveStepsIndex(MAKI_MOTOR_INDEX, 0, STARTUP_UP_STEPCOUNT);

  //  順に下げていく
  // runUntilStallAll(1, 10000);

  //  少しだけ上げる
  // moveStepsIndex(MAKI_MOTOR_INDEX, 0, 1000);

  //  一番下がMaxCountPosになる
    L6470_setparam_abspos_index(MAKI_MOTOR_INDEX, ALL_STEPCOUNT);

}
