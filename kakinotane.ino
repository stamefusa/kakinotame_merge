#include <Servo.h>

#define WAIT 1
#define ROTATE 2
#define STOP 9

#define LOTATE_PORT 8
#define PWM_PORT 9
#define SWITCH_PORT 3

int val1 = 0, pre1 = 0, diff1 = 0, pdiff1 = 0;
int val2 = 0, pre2 = 0, diff2 = 0, pdiff2 = 0;
int switch1 = 1, pre_switch1 = 1;
boolean is_start = false;
int threshold = 800;
int gyaku_start = 0, jun_start = 0;
unsigned long start_time = 0;
int mode = STOP;

char str;

Servo sk, sp;
int wait_k = 0, wait_p = 0, wait_all = 0;

void setup() {

  pinMode(LOTATE_PORT, OUTPUT);
  pinMode(SWITCH_PORT, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.setTimeout(10); // SerialでのString受信のタイムアウト設定（ms）

  // サーボピン初期化
  sk.attach(6);
  sp.attach(7);
  sk.write(170);
  sp.write(90);
}

void loop() {

  if (Serial.available() > 0) {
    //データの読み込み
    //str = Serial.readStringUntil('\r');
    str = Serial.read();
    Serial.println(str);
    if (str == 'a') {
      mode = STOP;
      start_time = millis();
    } else if (str == 'c') {
      mode = STOP;
    }
  }

  wait_k = getWaitTimeForKakinotane();
  wait_p = 10000 - wait_k;
  wait_all = max(wait_k, wait_p);
  Serial.println(wait_k);

  if (mode == STOP) {
    stopMotor();

    is_start = isStart();
    Serial.println(is_start);
    if (is_start) {
      mode = ROTATE;
      start_time = millis();
      Serial.println("Start to rotate.");

      if (wait_k > 0) {
        sk.write(100);
      }
      if (wait_p > 0) {
        sp.write(170);
      }
    }
  } else if (mode == ROTATE) {
    // モータ駆動
    driveMotor(true);
    Serial.println(millis() - start_time);

    if (millis() - start_time > wait_k) {
      Serial.println("kakinotane stop.");
      sk.write(170);
    }
    if (millis() - start_time > wait_p) {
      Serial.println("peanut stop.");
      sp.write(90);
    }

    if (millis() - start_time > wait_all) {
      Serial.println("auto stop.");
      mode = STOP;
    }
  } else {
    stopMotor();
    Serial.println("stop!");
  }

  delay(100);

}

void stopMotor()
{
  digitalWrite(LOTATE_PORT, LOW);
  analogWrite(PWM_PORT, 0);

  sk.write(170);
  sp.write(90);
}

void driveMotor(boolean flag)
{
  digitalWrite(LOTATE_PORT, HIGH);
  if (flag == true) {
    analogWrite(PWM_PORT, 250);
  }
  else {
    analogWrite(PWM_PORT, -250);
  }
}

boolean isStart()
{
  boolean dst = false;

  switch1 = digitalRead(SWITCH_PORT);
  if (pre_switch1 == 1 && switch1 == 0) {
    dst = true;
  }
  pre_switch1 = switch1;

  return dst;
}

int getWaitTimeForKakinotane()
{
  int val = analogRead(A0);
  int result = 0;
  if (val < 25) {
    val = 0;
  } else if (val >= 25 && val < 33) {
    val = 1000;
  } else if (val >= 33 && val < 55) {
    val = 2000;
  } else if (val >= 55 && val < 77) {
    val = 3000;
  } else if (val >= 77 && val < 108) {
    val = 4000;
  } else if (val >= 108 && val < 210) {
    val = 5000;
  } else if (val >= 210 && val < 250) {
    val = 6000;
  } else if (val >= 250 && val < 400) {
    val = 7000;
  } else if (val >= 400 && val < 670) {
    val = 8000;
  } else if (val >= 670 && val < 760) {
    val = 9000;
  } else if (val >= 760) {
    val = 10000;
  }

  return val;
}
