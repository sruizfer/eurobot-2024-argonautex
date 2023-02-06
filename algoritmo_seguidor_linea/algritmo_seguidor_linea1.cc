/**
 * @file algritmo_seguidor_linea1.cc
 * @author Saúl Ruiz Fernández (ruizfernandezsaul@gmail.com)
 * @brief Primera versión de la implantación del algoritmo para seguir una línea en la
 *        competición EUROBOT2024
 * @version 0.1
 * @date 2023-02-06

 * @copyright Copyright (c) 2023
 */

#include <iostream>
#include <cmath>

/**
 * @brief Pasa un valor a un intervalo determinado
 * 
 * @param valor_introducido, intervalo_origen1, intervalo_origen2, intervalo_destino1, intervalo_destino2
 * 
 * @return el valor transformado al nuevo intervalo en punto flotate de doble precisión
 */
double mapeo (double valor_introducido, double intervalo_origen1, double intervalo_origen2,
              double intervalo_destino1, double intervalo_destino2) {
  double valor_de_retorno = (((intervalo_destino1 - intervalo_destino2) / (intervalo_origen1 - intervalo_origen2)) * 
                            (valor_introducido - intervalo_origen1)) + intervalo_destino1;
  if (valor_de_retorno >= intervalo_destino2) {
    return intervalo_destino2;
  } else
    if (valor_de_retorno <= intervalo_destino1) {
      return intervalo_destino1;
    }
  return valor_de_retorno;
}

int main() {

  // Consideramos que las lecturas analógicas son de 5V (Arduino UNO) [0,1023]
  int lectura_analog_s1{0}; // Sensor 1
  int lectura_analog_s2{0}; // Sensor 2
  int lectura_analog_s3{0}; // Sensor 3
  int lectura_analog_s4{0}; // Sensor 4
  int lectura_analog_s5{0}; // Sensor 5
  int lectura_analog_s6{0}; // Sensor 6

  double kPesoS16{1.0};     // Pesos sensores 1 y 6
  double kPesoS25{0.5};     // Pesos sensores 2 y 5
  double kPesoS34{0.25};    // Pesos sensores 3 y 4

  double suma_sector_izquierdo{0.0};
  double suma_sector_derecho{0.0};

  double kVelocidadBase{100.0};

  double velocidad_motor_1{0.0};
  double velocidad_motor_2{0.0};

  double factor_multiplicativo1{0.0}; // Factor multiplicativo sector izquierdo
  double factor_multiplicativo2{0.0}; // Factor multiplicativo sector derecho

  double error_cometido{0.0};
  double error_anterior_cometido{0.0};
  double factor_correccion_errores1{0.0};
  double factor_correccion_errores2{0.0};

  int contador_iteraciones{0};

  while (true) {
    // Mapeamos las lecturas analógicas para pasarlas a un intervalo de [0,100]
    lectura_analog_s1 = mapeo(lectura_analog_s1, 0, 1023, 0, 100);
    lectura_analog_s2 = mapeo(lectura_analog_s2, 0, 1023, 0, 100);
    lectura_analog_s3 = mapeo(lectura_analog_s3, 0, 1023, 0, 100);
    lectura_analog_s4 = mapeo(lectura_analog_s4, 0, 1023, 0, 100);
    lectura_analog_s5 = mapeo(lectura_analog_s5, 0, 1023, 0, 100);
    lectura_analog_s6 = mapeo(lectura_analog_s6, 0, 1023, 0, 100);

    suma_sector_izquierdo = (lectura_analog_s1 * kPesoS16) + (lectura_analog_s2 * kPesoS25) +
                            (lectura_analog_s3 * kPesoS34);
    suma_sector_derecho = (lectura_analog_s4 * kPesoS34) + (lectura_analog_s5 * kPesoS25) +
                          (lectura_analog_s6 * kPesoS16);
    
    // Obtenemos los factores multiplicativos de las velocidades de cada motor
    factor_multiplicativo1 = mapeo(suma_sector_izquierdo, 0, 175, 1, 1.5);
    factor_multiplicativo2 = mapeo(suma_sector_derecho, 0, 175, 1, 1.5);

    // Obtenemos el factor de corrección de errores
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
    velocidad_motor_1 = kVelocidadBase * factor_multiplicativo1 * factor_correccion_errores1;
    velocidad_motor_2 = kVelocidadBase * factor_multiplicativo2 * factor_correccion_errores2;

    error_anterior_cometido = error_cometido;
  }
  return 0;
}