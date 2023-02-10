/**
   @file seguidor_de_linea_v1.ino
   @author Saúl Ruiz Fernández (ruizfernandezsaul@gmail.com)
   @brief Primera versión de la implantación del algoritmo para seguir una línea en la
          competición EUROBOT2024
   @version 0.1
   @date 2023-03-06
   @copyright Copyright (c) 2023
*/

/**
   @brief Pasa un valor a un intervalo determinado
   @param valor_introducido, intervalo_origen1, intervalo_origen2, intervalo_destino1, intervalo_destino2
   @return el valor transformado al nuevo intervalo en punto flotate de doble precisión
*/
double mapeo (double valor_introducido, double intervalo_origen1, double intervalo_origen2,
              double intervalo_destino1, double intervalo_destino2) {
  double valor_de_retorno = (((intervalo_destino1 - intervalo_destino2) / (intervalo_origen1 - intervalo_origen2)) *
                             (valor_introducido - intervalo_origen1)) + intervalo_destino1;
  if (valor_de_retorno >= intervalo_destino2) {
    return intervalo_destino2;
  } else if (valor_de_retorno <= intervalo_destino1) {
    return intervalo_destino1;
  }
  return valor_de_retorno;
}

// Declaración de variables
int lectura_analog_s1 = 0; // Sensor 1
int lectura_analog_s2 = 0; // Sensor 2
int lectura_analog_s3 = 0; // Sensor 3
int lectura_analog_s4 = 0; // Sensor 4
int lectura_analog_s5 = 0; // Sensor 5
int lectura_analog_s6 = 0; // Sensor 6
int lectura_analog_s7 = 0; // Sensor 7
int lectura_analog_s8 = 0; // Sensor 8

const double kPesoS18 = 1;   // Pesos sensores 1 y 8
const double kPesoS27 = 0.5;   // Pesos sensores 2 y 7
const double kPesoS36 = 0.2;  // Pesos sensores 3 y 6
const double kPeso45 = 0.1;    // Pesos sensores 4 y 5

double suma_sector_izquierdo = 0;
double suma_sector_derecho = 0;

const double kVelocidadBase = 30; // RPM para el motor stepper que se utilize en el proyecto

double factor_multiplicativo1;  // Factor multiplicativo sector izquierdo
double factor_multiplicativo2;  // Factor multiplicativo sector derecho

int velocidad_motor_derecho;
int velocidad_motor_izquierdo;

void setup() {

  Serial.begin(9600);

  pinMode(A0, INPUT);  // Sensor 1
  pinMode(A1, INPUT);  // Sensor 2
  pinMode(A2, INPUT);  // Sensor 3
  pinMode(A3, INPUT);  // Sensor 4
  pinMode(A4, INPUT);  // Sensor 5
  pinMode(A5, INPUT);  // Sensor 6
  pinMode(A6, INPUT);  // Sensor 7
  pinMode(A7, INPUT);  // Sensor 8

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {

  // Mapeamos las lecturas analógicas para pasarlas a un intervalo de [0,100]
  lectura_analog_s1 = mapeo(analogRead(A0), 0, 1023, 0, 100);
  lectura_analog_s2 = mapeo(analogRead(A1), 0, 1023, 0, 100);
  lectura_analog_s3 = mapeo(analogRead(A2), 0, 1023, 0, 100);
  lectura_analog_s4 = mapeo(analogRead(A3), 0, 1023, 0, 100);
  lectura_analog_s5 = mapeo(analogRead(A4), 0, 1023, 0, 100);
  lectura_analog_s6 = mapeo(analogRead(A5), 0, 1023, 0, 100);
  lectura_analog_s7 = mapeo(analogRead(A6), 0, 1023, 0, 100);
  lectura_analog_s8 = mapeo(analogRead(A7), 0, 1023, 0, 100);

  suma_sector_derecho = (lectura_analog_s1 * kPesoS18) + (lectura_analog_s2 * kPesoS27) +
                        (lectura_analog_s3 * kPesoS36) + (lectura_analog_s4 * kPeso45);
                        
  suma_sector_izquierdo = (lectura_analog_s5 * kPeso45) + (lectura_analog_s6 * kPesoS36) +
                          (lectura_analog_s7 * kPesoS27) + (lectura_analog_s8 * kPesoS18);
                          
  // Serial.println(suma_sector_derecho);

  // Obtenemos los factores multiplicativos de las velocidades de cada motor
  factor_multiplicativo1 = suma_sector_derecho;
  factor_multiplicativo2 = suma_sector_izquierdo;

  // Serial.println(factor_multiplicativo1);

  // Actualizamos la velocidad en cada motor
  velocidad_motor_derecho = kVelocidadBase + (kVelocidadBase * (factor_multiplicativo1 / 100));
  velocidad_motor_izquierdo = kVelocidadBase + (kVelocidadBase * (factor_multiplicativo2 / 100));
  
  velocidad_motor_derecho = mapeo(velocidad_motor_derecho, 0, 60, 0, 255);
  velocidad_motor_izquierdo = mapeo(velocidad_motor_izquierdo, 0, 60, 0, 255);

  // Serial.println(velocidad_motor_derecho);

  analogWrite(5, velocidad_motor_derecho);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);

  analogWrite(6, velocidad_motor_izquierdo);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
}
