#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include <stdio.h>


#define BAUD 9600
#define F_CPU 7372800UL
#define BAUD_PRESCALE ((F_CPU / (16UL * BAUD)) - 1)

#define BUTTON_INC PC5
#define BUTTON_DEC PC4
#define BUTTON_SET PC3

void serial_init ( unsigned short ubrr ) {
    UBRR0H = (ubrr>>8);
    UBRR0L = (ubrr);
    UCSR0B |= (1<<TXEN0); /* Enable  transmitter                 */
    UCSR0B |= (1 << RXEN0 ); // Turn on receiver
    UCSR0C = (3 << UCSZ00 );
// one stop bit , 8 data bits
}
void serial_out ( char ch )
{
while (( UCSR0A & (1 << UDRE0 )) == 0);
UDR0 = ch ;
}
char serial_in ()
{
while ( !( UCSR0A & (1 << RXC0 )) );
return UDR0 ;
}
void adc_init() {
    ADMUX = (1 << REFS0); // AVCC as reference voltage
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, prescaler 128
}

// Function to read the ADC value from a specified channel
uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Set the desired channel
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    return ADC;
}

void pushbutton_init() {
    DDRC &= ~((1 << BUTTON_INC) | (1 << BUTTON_DEC) | (1 << BUTTON_SET)); // Set PC3, PC4, and PC5 as inputs
    PORTC |= (1 << BUTTON_INC) | (1 << BUTTON_DEC) | (1 << BUTTON_SET); // Enable pull-up resistors on PC3, PC4, and PC5
}

// Function to read the pushbutton states
uint8_t pushbutton_read() {
    return (PINC & ((1 << BUTTON_INC) | (1 << BUTTON_DEC) | (1 << BUTTON_SET))) ^ ((1 << BUTTON_INC) | (1 << BUTTON_DEC) | (1 << BUTTON_SET));
}

void output_init() {
    // Configure PD6 and PD7 as outputs
    DDRD |= (1 << PD6) | (1 << PD7);
    // Configure PB1 and PB2 as outputs
    DDRB |= (1 << PB1) | (1 << PB2) | (1 << PB3);
}

#define BAUD_RATE_230400_BPS  2 // 230.4kps
 
int main()
{
    // int i = 0;
    // unsigned int ubrr = BAUD_PRESCALE;
    // unsigned char data[] = "Hello from ATmega328p  ";
    // unsigned char d[] = "bb        jjj";
 
    /* Set Baudrate  */
    // serial_init(ubrr);
    lcd_init(LCD_DISP_ON);
    adc_init();

    // Clear the LCD
    lcd_clrscr();

    // Display "Hello, World!" on the first line
    output_init();
    pushbutton_init();
    char buffer[18];
    char buffer2[18];
    char temp_buff[18];
    char mois_buff[18];
    char nutr_buff[20];
    uint16_t set_light = 50;
    uint16_t set_nutr = 1;
    uint16_t set_temp = 50;
    uint16_t set_mois = 100;
    uint8_t is_set = 0;
    uint16_t set_timer = 0;
    uint8_t set_state = 0;
    uint8_t flash_timer = 0;
    uint8_t temp_timer = 0;
    uint8_t mois_timer = 0;
    uint8_t light_timer = 0;
    uint16_t nutr_timer = 0;
    uint16_t nutr_timer_min = 0;
    uint16_t nutr_on_timer = 0;
    uint8_t nutr_unit = 0;
    uint8_t light_on = 0;
    uint8_t nutr_on = 0;
    uint8_t temp_on = 0;
    uint8_t mois_on = 0;
    uint8_t light_on_timer = 0;
    uint8_t mois_on_timer = 0;
    uint8_t temp_on_timer = 0;
    uint8_t nutr_open = 1;

 
    while(1) /* Loop the messsage continously */
    { 
        uint8_t button_states = pushbutton_read();
        if (is_set==1){
            set_timer++;
            flash_timer++;
            if (set_timer >16){
                is_set = 0;
                set_timer = 0;
                set_state = 0;
            }
            flash_timer = flash_timer%4;
            snprintf(buffer2, sizeof(buffer2), "Set Light: %d", set_light);
            snprintf(temp_buff, sizeof(temp_buff), "Set Temp: %u F", set_temp);
            snprintf(mois_buff, sizeof(mois_buff), "Set Mois: %u", set_mois);
            if (nutr_unit == 0){
                snprintf(nutr_buff, sizeof(nutr_buff), "Set Nutr: 1/%umin", set_nutr);
            }
            else if (nutr_unit ==1){
                snprintf(nutr_buff, sizeof(nutr_buff), "Set Nutr: 1/%uh", set_nutr);
            }
            else if (nutr_unit==2)
            {
                snprintf(nutr_buff, sizeof(nutr_buff), "Set Nutr: 1/%ud", set_nutr);
            }
            
            if (set_state==0 && flash_timer>1){
                lcd_goto(0x00);
                lcd_puts("                   ");
            }
            else {
                // Set the cursor to the 1st line
                lcd_goto(0x00);
                // Print the temp sensor value on the LCD
                lcd_puts(temp_buff);
                lcd_puts("   "); // Clear any trailing digits
            }

            
            if (set_state==1 && flash_timer>1){
                lcd_goto(0x40);
                lcd_puts("                   ");
            }
            else {
                // Set the cursor to the 2nd line
                lcd_goto(0x40);
                // Print the temp sensor value on the LCD
                lcd_puts(mois_buff);
                lcd_puts("  "); // Clear any trailing digits
            }

            // Set the cursor to the 3rd line
            lcd_goto(0x14);
            // Print the temp sensor value on the LCD
            lcd_puts(nutr_buff);
            lcd_puts(" "); // Clear any trailing digits
            if (set_state==2 && flash_timer>1){
                lcd_goto(0x14);
                lcd_puts("                   ");
            }
            else {
                // Set the cursor to the 3rd line
                lcd_goto(0x14);
                // Print the temp sensor value on the LCD
                lcd_puts(nutr_buff);
                lcd_puts(" "); // Clear any trailing digits
            }
            
            if (set_state==3 && flash_timer>1){
                lcd_goto(0x54);
                lcd_puts("                   ");
            }
            else {
                // Set the cursor to the last line
                lcd_goto(0x54);
                // Print the value on the LCD
                lcd_puts(buffer2);
                lcd_puts("    ");
            }
            if (button_states & (1 << BUTTON_INC)) {
                // Increase the value if the increase button is pressed
                if (set_state == 0){
                    set_temp++;
                }
                else if (set_state == 1){
                    set_mois = set_mois + 100;
                }
                else if (set_state == 2){
                    set_nutr++;
                    if (set_nutr>=60 && nutr_unit==0){
                        set_nutr = 1;
                        nutr_unit = 1;
                    }
                    else if (set_nutr>=24 && nutr_unit==1){
                        set_nutr = 1;
                        nutr_unit = 2;
                    }
                }
                else if (set_state == 3){
                    set_light = set_light + 10;
                }
                set_timer = 0;
                flash_timer = 0;
                _delay_ms(100); // Add some debounce delay
            }

            if (button_states & (1 << BUTTON_DEC)) {
                // Decrease the value if the decrease button is pressed
                if (set_state == 0){
                    set_temp--;
                }
                else if (set_state == 1){
                    set_mois = set_mois - 100;
                }
                else if (set_state == 2){
                    set_nutr--;
                    if (nutr_unit == 2 && set_nutr < 1){
                        nutr_unit = 1;
                        set_nutr = 23;
                    }
                    else if (nutr_unit == 1 && set_nutr < 1){
                        nutr_unit = 0;
                        set_nutr = 59;
                    }
                    if (set_nutr>=60 && nutr_unit==0){
                        set_nutr = 1;
                        nutr_unit = 1;
                    }
                    else if (set_nutr>=24 && nutr_unit==1){
                        set_nutr = 1;
                        nutr_unit = 2;
                    }
                }
                else if (set_state == 3){
                    set_light = set_light - 10;
                }
                set_timer = 0;
                flash_timer = 0;
                _delay_ms(100); // Add some debounce delay
            }

            // set state
            if (button_states & (1 << BUTTON_SET)) {
                is_set = 1;
                set_timer = 0;
                set_state++;
                set_state=set_state%4;
            }
        }
        else {
            // Read the light sensor value connected to PC2 (ADC2)
            uint16_t light_value = adc_read(2);
            // Read the temp sensor value connected to PC0 (ADC2)
            uint16_t temp_value = adc_read(0);
            float temp = temp_value * 5.0;
            temp /= 1024.0;
            temp = (temp -0.5)*100; // temp in c
            temp = (temp*9.0/5.0)+32.0; // temp in f
            temp_value = (int)temp;
            uint16_t mois_value = adc_read(1);

            const char *light_status = light_on == 1 ? "On" : "Off";
            snprintf(buffer, sizeof(buffer), "Light: %u %s", light_value, light_status);

            const char *temp_status = temp_on == 1 ? "On" : "Off";
            snprintf(temp_buff, sizeof(temp_buff), "Temp: %u F %s", temp_value, temp_status);

            const char *mois_status = mois_on == 1 ? "On" : "Off";
            snprintf(mois_buff, sizeof(mois_buff), "Mois: %u %s", mois_value, mois_status);

            const char *nutr_status;
            if (nutr_on == 1) {
                nutr_status = "On";
            } else {
                nutr_status = "Off";
            }

            if (nutr_unit == 0) {
                snprintf(nutr_buff, sizeof(nutr_buff), "Nutr: 1/%umin %s", set_nutr, nutr_status);
            } else if (nutr_unit == 1) {
                snprintf(nutr_buff, sizeof(nutr_buff), "Nutr: 1/%uh %s", set_nutr, nutr_status);
            } else if (nutr_unit == 2) {
                snprintf(nutr_buff, sizeof(nutr_buff), "Nutr: 1/%ud %s", set_nutr, nutr_status);
            }

            // Set the cursor to the 1st line
            lcd_goto(0x00);
            // Print the temp sensor value on the LCD
            lcd_puts(temp_buff);
            lcd_puts("    "); // Clear any trailing digits

            // Set the cursor to the 2nd line
            lcd_goto(0x40);
            // Print the temp sensor value on the LCD
            lcd_puts(mois_buff);
            lcd_puts("     "); // Clear any trailing digits

            // Set the cursor to the 3rd line
            lcd_goto(0x14);
            // Print the temp sensor value on the LCD
            lcd_puts(nutr_buff);
            lcd_puts(" "); // Clear any trailing digits

            // Set the cursor to the last line
            lcd_goto(0x54);
            // Print the light sensor value on the LCD
            lcd_puts(buffer);
            lcd_puts("      "); // Clear any trailing digits
            // set state
            if (button_states & (1 << BUTTON_SET)) {
                is_set = 1;
            }
            // light
            if (set_light > light_value) {
                light_timer++;
                if (light_timer > 10 && light_on == 0) {
                    PORTB |= (1 << PB1);
                    light_on = 1;
                    light_on_timer = 0;
                    light_timer = 0;
                }
            } else if (light_on == 1 && set_light <= light_value) {
                light_on_timer++;
                if (light_on_timer > 5){
                    PORTB &= ~(1 << PB1);
                    light_on = 0;
                    light_timer = 0;
                    light_on_timer = 0;
                }
                
                // light_on_timer++;
                // if (light_on_timer > 20) {
                //     PORTB &= ~(1 << PB1);
                //     light_on = 0;
                //     light_timer = 0;
                //     light_on_timer = 0;
                // }
            }

            // mois
            if (set_mois > mois_value) {
                mois_timer++;
                if (mois_timer > 10 && mois_on == 0) {
                    PORTD |= (1 << PD6);
                    mois_on = 1;
                    mois_on_timer = 0;
                    mois_timer = 0;
                }
            } else if (mois_on == 1 && set_mois <= mois_value) {
                PORTD &= ~(1 << PD6);
                mois_on = 0;
                mois_timer = 0;
                mois_on_timer = 0;
                
            }
            if (mois_on == 1) {
                mois_on_timer++;
                if (mois_on_timer > 30) {
                    PORTD &= ~(1 << PD6);
                    mois_on = 0;
                    mois_timer = 0;
                    mois_on_timer = 0;
                }
            }
            // temp
            if (set_temp > temp_value) {
                temp_timer++;
                if (temp_timer > 10 && temp_on == 0) {
                    PORTB |= (1 << PB2);
                    temp_on = 1;
                    temp_on_timer = 0;
                    temp_timer = 0;
                }
            } else if (temp_on == 1 && set_temp <= temp_value) {
                temp_on_timer++;
                if (temp_on_timer>5){
                    PORTB &= ~(1 << PB2);
                    temp_on = 0;
                    temp_timer = 0;
                    temp_on_timer = 0;
                }
                
                // if (temp_on_timer > 20) {
                //     PORTB &= ~(1 << PB2);
                //     temp_on = 0;
                //     temp_timer = 0;
                //     temp_on_timer = 0;
                // }
            }
            nutr_timer++;

            if (nutr_timer>=300){
                nutr_timer_min++;
            }
            if (nutr_unit==0 && set_nutr <= nutr_timer_min){
                PORTD |= (1 << PD7);
                nutr_on_timer++;
                nutr_on = 1;
            }
            else if (nutr_unit == 1 && set_nutr*60 <=nutr_timer_min){
                PORTD |= (1 << PD7);
                nutr_on_timer++;
                nutr_on = 1;
            }
            else if (nutr_unit==2 && set_nutr*1440 <= nutr_timer_min){
                PORTD |= (1 << PD7);
                nutr_on_timer++;
                nutr_on = 1;
            }

            if (nutr_on_timer > 30){
                PORTD &= ~(1 << PD7);
                nutr_on_timer = 0;
                nutr_timer_min = 0;
                nutr_on = 0;
                nutr_timer = 0;
            }

        }
        _delay_ms(200);

        
        // snprintf(buffer2, sizeof(buffer2), "Set Light: %d", set_light);
        
        // Set the cursor to the third line
        // lcd_goto(0x14);

        // Print the value on the LCD
        // lcd_puts(buffer2);
        // lcd_puts("    ");
        // i = 0;
        // while(d[i] != 0) /* print the String  "Hello from ATmega328p" */
        // {
        //   serial_out(data[i]);          /* Put data into buffer, sends the data */
        //   i++;                             /* increment counter           */
        // }
        // char c = serial_in();
        // serial_out(c);
        
      }
}//end of main
