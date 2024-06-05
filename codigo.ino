#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

#define pin_sensor A0 
const int numReadings = 10;
int readings[numReadings];      // as leituras da entrada analógica
int readIndex = 0;              // o índice da leitura atual
int total = 0;                  // o total acumulado
int average = 0;                // a média
int leitura_ADC;
int ntu;
int ledpin = 13;
int ledpin2 = 12;
int ledpin3 = 11;
int Contrast = 100;             // Valor de contraste, ajustado para corrente máxima de 20 mA
unsigned long previousMillis = 0; // armazena o último tempo que o display foi atualizado
const long interval = 6000;       // intervalo de tempo de 6 segundos
bool displayTurbidity = true;     // variável para alternar entre turbidez e pH

void setup() {
  Serial.begin(9600);
  analogWrite(6, Contrast);  // Ajusta o contraste

  pinMode(ledpin, OUTPUT);
  pinMode(ledpin2, OUTPUT);
  pinMode(ledpin3, OUTPUT);
  pinMode(pin_sensor, INPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Bem-vindo     ");
  lcd.setCursor(0, 1);
  lcd.print("Sensor Turbidez");
  delay(2000);
  lcd.clear();

  // Inicializa todas as leituras com 0
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Subtrai a última leitura
  total = total - readings[readIndex];
  // Lê o sensor
  readings[readIndex] = analogRead(pin_sensor);
  // Adiciona a leitura ao total
  total = total + readings[readIndex];
  // Avança para a próxima posição no array
  readIndex = readIndex + 1;

  // Se chegarmos ao final do array, voltamos para o início
  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  // Calcula a média
  average = total / numReadings;

  // Calibra a leitura do sensor para NTU (ajuste esses valores com base na calibração)
  if (average > 208) {
    average = 208;
  }
  ntu = map(average, 0, 208, 300, 0);

  // Leitura do valor de pH
  int sensorValue = analogRead(A1);
  float ph = sensorValue * (14.0 / 1023.0);

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    displayTurbidity = !displayTurbidity; // alterna entre turbidez e pH
    lcd.clear();
  }

  if (displayTurbidity) {
    // Exibe os resultados de turbidez no LCD
    lcd.setCursor(0, 0);
    lcd.print("Turbidez: ");
    lcd.print(ntu);
    lcd.print(" NTU   ");
    lcd.setCursor(0, 1);

    if (ntu < 5) {
      lcd.print("Agua Potavel     ");
      digitalWrite(ledpin3, HIGH);
    } else {
      digitalWrite(ledpin3, LOW);
    }

    if (ntu >= 5 && ntu < 30) {
      lcd.print("Agua Normal      ");
      digitalWrite(ledpin2, HIGH);
    } else {
      digitalWrite(ledpin2, LOW);
    }

    if (ntu > 30) {
      lcd.print("Agua suja        ");
      digitalWrite(ledpin, HIGH);
    } else {
      digitalWrite(ledpin, LOW);
    }
  } else {
    // Exibe os resultados de pH no LCD
    lcd.setCursor(0, 0);
    lcd.print("pH:");
    lcd.setCursor(4, 0);
    lcd.print(ph);
    Serial.println(ph);

    // Mensagens sobre o pH para animais
    if (ph > 8.5) {
      lcd.setCursor(0, 1);
      lcd.print("pH Alto: Alcalino ");
    } else if (ph >= 7.5 && ph <= 8.5) {
      lcd.setCursor(0, 1);
      lcd.print("pH Moderado: Neutro ");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("pH Baixo: Acido ");
    }
  }

  delay(200);
}
