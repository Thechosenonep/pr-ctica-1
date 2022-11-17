#include <18F4550.h>
#include <stdbool.h>
#FUSES HS, NOPROTECT, NOWDT, NOBROWNOUT, PUT, NOLVP
#use delay(clock = 48Mhz, crystal)           // Tipo de oscilador y frecuencia dependiendo del microcontrolador
#build(reset = 0x02000, interrupt = 0x02008) // Asignación de los vectores de reset e interrupción
#org 0x0000, 0x1FFF {}
#byte porta = 0xf80                                                   // Identificador para el puerto A.
#byte portb = 0xf81                                                   // Identificador para el puerto B.
#byte portc = 0xf82                                                   // Identificador para el puerto C.
#byte portd = 0xf83                                                   // Identificador para el puerto D.
#USE FAST_IO(D)
#USE FAST_IO(B)
#use i2c(Master, Fast = 100000, sda = PIN_A2, scl = PIN_A3, force_sw) // Se incluye la librería del driver para el manejo del sensor DS18B20
#define ENCENDERBUZZER bit_set(portd, 0)
#define APAGARBUZZER bit_clear(portd, 0)
#define HOTWATERON bit_set(portd, 4)
#define HOTWATEROFF bit_clear(portd, 4)
#define COLDWATERON bit_set(portd, 6)
#define COLDWATEROFF bit_clear(portd, 6)
#define DS1820_PIN PIN_A0 // Se define pin como entrada digital para leer datos del sensor DS18B20
#define ONE_WIRE_PIN DS1820_PIN
#include "i2c_Flex_LCD.c" // Reserva espacio en la memoria para la versión con bootloader
#include <1wire.c>        //Se incluyen las librerías correspondientes para el manejo del protocolo 1WIRE
#include <ds1820.c>
float temperatura;
bool hotwater = false;
bool coldwater = false;
#INT_EXT
void PortB_Interrupt()
{
   ext_int_edge(H_TO_L);
   output_toggle(pin_b0);
   hotwater = !hotwater;
   (hotwater == false) ? HOTWATEROFF : HOTWATERON;
   
   
}

#INT_EXT1
void PORTB2_Interrupt()
{
   ext_int_edge(1,H_TO_L); // Flanco de subida. 
   output_toggle(pin_b1);
   coldwater = !coldwater;
   (coldwater == false) ? COLDWATEROFF : COLDWATERON;

}
void main()
{
   set_tris_d(0b00000000);
   set_tris_b(0b11111111);
   set_tris_c(0b11111111);
   lcd_init(0x4E, 16, 2); // dirección y tamaño de display
   lcd_backlight_led(ON); // bit pin P3
   lcd_clear();
   port_b_pullups(true);
   enable_interrupts(INT_EXT1);
   enable_interrupts(INT_EXT);
   enable_interrupts(global);


   while (1)
   {

      lcd_gotoxy(1, 1);
      temperatura = ds1820_read();
      if (temperatura > 35)
      {
         ENCENDERBUZZER;
      }
      if (temperatura < 35)
      {
         APAGARBUZZER;
      }

     

      // Lee el sensor
      printf(lcd_putc, "\fTemperatura \n%3.1fC", temperatura); // Imprime en display
      delay_ms(1000);
   }
}
