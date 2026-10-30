#include <Arduino.h>
// Librerías.

// Protocolo de transferencia.
#include <OneWire.h>

// Registro de temperatura.
#include <DallasTemperature.h>


// Pin de entrada y salida e intervalo de medición.

#define PIN_BUS 4

const unsigned long intervalo = 5000;


// Clase.

class SensorTemperatura {

    public:

    // Atributos.

    DeviceAddress direccion;
    float posicion;


    // Constructor (inicializa los atributos de la clase).

    SensorTemperatura(DeviceAddress direccionSensor, float posicionSensor) {

        // Recorre dirección (64 bits).

        for (int i = 0; i < 8; i++) {
            direccion[i] = direccionSensor[i];
        }

        posicion = posicionSensor;
    }


    // Métodos.

    // Función que obtiene la posición de los sensores.

    float obtenerPosicion() {
        return posicion;
    }


    // Función que lee temperaturas por dirección.

    float leerTemperatura(DallasTemperature &sensores) {
        return sensores.getTempC(direccion);
    }
};


// Configuración del Bus OneWire.

OneWire oneWire(PIN_BUS);

// Recibe la dirección en memoria del objeto oneWire
// para saber a qué pin enviar los datos.

DallasTemperature sensoresTemperatura(&oneWire);


// Direcciones.
DeviceAddress direccion1 = {
    0x28, 
    0xF2, 
    0x75, 
    0x58, 
    0xD4, 
    0xE1, 
    0x3C, 
    0x52
};

DeviceAddress direccion2 = {
    0x28,
    0x2F, 
    0xA9, 
    0x58,
    0xD4,
    0xE1,
    0x3C, 
    0xA3 
};

DeviceAddress direccion3 = {
     0x28,
     0xD4,
     0x89,
     0x58,
     0xD4,
     0xE1,
     0x3C,
     0xD1
};


// Sensores.

SensorTemperatura sensor1(direccion1, 0.05);
SensorTemperatura sensor2(direccion2, 0.10);
SensorTemperatura sensor3(direccion3, 0.15);


SensorTemperatura sensores[] = {
    sensor1,
    sensor2,
    sensor3
};


const int cantidadSensores = 3;


// Variables del experimento.

bool experimentoIniciado = false;
bool experimentoPausado = false;

unsigned long tiempoInicio = 0;
unsigned long ultimaMedicion = 0;


// Función para imprimir en pantalla.

void imprimirDatos(float tiempo, float posicion, float temperatura) {

    Serial.print(tiempo, 2);
    Serial.print(",");

    Serial.print(posicion, 2);
    Serial.print(",");

    Serial.println(temperatura, 2);
}


// Función para almacenar la posición y la temperatura.

void medirSensores(float tiempo) {

    sensoresTemperatura.requestTemperatures();

    for (int i = 0; i < cantidadSensores; i++) {

        float T = sensores[i].leerTemperatura(sensoresTemperatura);
        float X = sensores[i].obtenerPosicion();

        imprimirDatos(tiempo, X, T);
    }
}


// Función de inicio del experimento.

void iniciarExperimento() {

    experimentoIniciado = true;
    experimentoPausado = false;

    tiempoInicio = millis();
    ultimaMedicion = tiempoInicio;

    Serial.println("# Experimento iniciado");

    // Condiciones iniciales.

    medirSensores(0.0);
}


// Función para pausar.

void pausarExperimento() {

    if (experimentoIniciado && !experimentoPausado) {

        experimentoPausado = true;

        Serial.println("# Experimento pausado");
    }
}


// Función para reanudar.

void reanudarExperimento() {

    if (experimentoIniciado && experimentoPausado) {

        experimentoPausado = false;

        // Reinicia la referencia de tiempo
        // para evitar una medición acumulada durante la pausa.

        ultimaMedicion = millis();

        Serial.println("# Experimento reanudado");
    }
}


// Función para reiniciar.

void reiniciarExperimento() {

    experimentoIniciado = false;
    experimentoPausado = false;

    tiempoInicio = 0;
    ultimaMedicion = 0;

    Serial.println("# Experimento reiniciado");
    Serial.println("# Presionar S para comenzar");
}


// Función para finalizar.

void finalizarExperimento() {

    if (experimentoIniciado) {

        experimentoIniciado = false;
        experimentoPausado = false;

        Serial.println("# Experimento finalizado");
    }
}


// Función para procesar comandos.

void procesarComando() {

    if (Serial.available() > 0) {

        char comando = Serial.read();


        // E,e= Empezar.

        if (comando == 'E' || comando == 'e') {

            if (!experimentoIniciado) {
                iniciarExperimento();
            }
        }


        // P,p = Pausar,reanudar.

        else if (comando == 'P' || comando == 'p') {

            if (experimentoIniciado && !experimentoPausado) {
                pausarExperimento();
            }

            else if (experimentoIniciado && experimentoPausado) {
                reanudarExperimento();
            }
        }


        // R,r = Reiniciar.

        else if (comando == 'R' || comando == 'r') {

            reiniciarExperimento();
        }


        // T,t = Terminar.

        else if (comando == 'T' || comando == 't') {

            finalizarExperimento();
        }
    }
}


// Configuración.

void setup() {

    // Velocidad de transferencia.

    Serial.begin(9600);

    sensoresTemperatura.begin();


    // Mensajes iniciales.

    Serial.println("# Sistema listo");
    Serial.println("# E = Empezar");
    Serial.println("# P = Pausar/reanudar");
    Serial.println("# R = Reiniciar");
    Serial.println("# T = Terminar");
    Serial.println("# tiempo,posicion,temperatura");
}


// Bucle.

void loop() {

    // Revisa si se recibió algún comando.

    procesarComando();


    // Si el experimento no comenzó,
    // no realiza mediciones.

    if (!experimentoIniciado) {
        return;
    }


    // Si está pausado,
    // no realiza mediciones.

    if (experimentoPausado) {
        return;
    }


    //Mediciones siguientes a la inicial.

    unsigned long ahora = millis();


    if (ahora - ultimaMedicion >= intervalo) {

        ultimaMedicion = ahora;

        float tiempo = (ahora - tiempoInicio) / 1000.0;

        medirSensores(tiempo);
    }
}