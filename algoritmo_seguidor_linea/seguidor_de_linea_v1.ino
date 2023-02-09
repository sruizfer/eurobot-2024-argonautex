/**
   @file seguidor_de_linea_v1.ino
   @author Saúl Ruiz Fernández (ruizfernandezsaul@gmail.com)
   @brief Primera versión de la implantación del algoritmo para seguir una línea en la
          competición EUROBOT2024
   @version 0.1
   @date 2023-03-06

   @copyright Copyright (c) 2023
*/

#include <Stepper.h>

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
int lectura_analog_s1{0}; // Sensor 1
int lectura_analog_s2{0}; // Sensor 2
int lectura_analog_s3{0}; // Sensor 3
int lectura_analog_s4{0}; // Sensor 4
int lectura_analog_s5{0}; // Sensor 5
int lectura_analog_s6{0}; // Sensor 6

const double kPesoS16{1.0};   // Pesos sensores 1 y 6
const double kPesoS25{0.5};   // Pesos sensores 2 y 5
const double kPesoS34{0.25};  // Pesos sensores 3 y 4

double suma_sector_izquierdo{0.0};
double suma_sector_derecho{0.0};

const double kVelocidadBase{50.0}; // RPM para el motor stepper que se utilize en el proyecto

double factor_multiplicativo1{0.0};  // Factor multiplicativo sector izquierdo
double factor_multiplicativo2{0.0};  // Factor multiplicativo sector derecho

double velocidad_motor_derecho{0.0};
double velocidad_motor_izquierdo{0.0};

double error_cometido{0.0};
double error_anterior_cometido{0.0};
double factor_correccion_errores1{0.0};  // Factor de corrección de errores motor izquierdo
double factor_correccion_errores2{0.0};  // Factor de corrección de errores motor derecho

int contador_iteraciones{0};

Stepper myStepper1(kVelocidadBase, 8, 9, 10, 11); // Configuración de pines del motor izquierdo
Stepper myStepper2(kVelocidadBase, 4, 5, 6, 7); // Configuración de pines del motor derecho

void setup() {

  Serial.begin(9600);

  pinMode(A0, INPUT);  // Sensor 1
  pinMode(A1, INPUT);  // Sensor 2
  pinMode(A2, INPUT);  // Sensor 3
  pinMode(A3, INPUT);  // Sensor 4
  pinMode(A4, INPUT);  // Sensor 5
  pinMode(A5, INPUT);  // Sensor 6
}

void loop() {

  // Mapeamos las lecturas analógicas para pasarlas a un intervalo de [0,100]
  lectura_analog_s1 = mapeo(analogRead(A0), 0, 1023, 0, 100);
  lectura_analog_s2 = mapeo(analogRead(A1), 0, 1023, 0, 100);
  lectura_analog_s3 = mapeo(analogRead(A2), 0, 1023, 0, 100);
  lectura_analog_s4 = mapeo(analogRead(A3), 0, 1023, 0, 100);
  lectura_analog_s5 = mapeo(analogRead(A4), 0, 1023, 0, 100);
  lectura_analog_s6 = mapeo(analogRead(A5), 0, 1023, 0, 100);

  suma_sector_izquierdo = (lectura_analog_s1 * kPesoS16) + (lectura_analog_s2 * kPesoS25) +
                          (lectura_analog_s3 * kPesoS34);
  suma_sector_derecho = (lectura_analog_s4 * kPesoS34) + (lectura_analog_s5 * kPesoS25) +
                        (lectura_analog_s6 * kPesoS16);

  // Obtenemos los factores multiplicativos de las velocidades de cada motor
  factor_multiplicativo1 = mapeo(suma_sector_izquierdo, 0, 175, 1, 1.5);
  factor_multiplicativo2 = mapeo(suma_sector_derecho, 0, 175, 1, 1.5);

  // Obtenemos los factores de corrección de errores
  error_cometido = suma_sector_izquierdo - suma_sector_derecho;
  if (contador_iteraciones == 0) {
    error_anterior_cometido = error_cometido;
    ++contador_iteraciones;
  }
  if ((abs(error_anterior_cometido) - abs(error_cometido)) < 0) {
    if (error_cometido < 0) {
      factor_correccion_errores2 = mapeo(abs(abs(error_anterior_cometido) - abs(error_cometido)),
                                         0, 20, 1, 1.5);
    }
    if (error_cometido > 0) {
      factor_correccion_errores1 = mapeo(abs(abs(error_anterior_cometido) - abs(error_cometido)),
                                         0, 20, 1, 1.5);
    }
  }

  // Actualizamos la velocidad en cada motor
  velocidad_motor_izquierdo = kVelocidadBase * factor_multiplicativo1 * factor_correccion_errores1;
  velocidad_motor_derecho = kVelocidadBase * factor_multiplicativo2 * factor_correccion_errores2;
  myStepper1.setSpeed(velocidad_motor_izquierdo);
  myStepper2.setSpeed(velocidad_motor_derecho);
  myStepper1.step((kVelocidadBase / 500) * velocidad_motor_izquierdo);
  myStepper2.step((kVelocidadBase / 500) * velocidad_motor_derecho);

  error_anterior_cometido = error_cometido;
}
