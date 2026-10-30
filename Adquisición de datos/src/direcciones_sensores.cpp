//Direcciones sensores.
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_BUS 4

OneWire bus(PIN_BUS);

void setup() {

  Serial.begin(9600);

  Serial.println("Buscando sensores...");

  byte direccion[8];

  bus.reset_search();

  while (bus.search(direccion)) {

    Serial.print("Direccion: ");

    for (int i = 0; i < 8; i++) {

      if (direccion[i] < 16) {
        Serial.print("0");
      }

      Serial.print(direccion[i], HEX);
      Serial.print(" ");
    }

    Serial.println();
  }

  Serial.println("Busqueda terminada.");
}

void loop() {
}