/*
 * project5.c
 *
 * Created: 6/5/2016 7:06:17 PM
 * Author : Carl
 */ 

#include "avr.h"
#include "lcd.h"
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>

unsigned int time = 0;

ISR(TIMER1_COMPA_vect)
{
	time++;
	if(time > 65000){
		time = 0;
	}
}


float tempo = 1.5;
float volume = 0;

unsigned long Alow = 227;
unsigned long C = 190;
unsigned long Csharp = 180;
unsigned long D = 170;
unsigned long Dsharp = 160;
unsigned long E = 151;
unsigned long F = 143;
unsigned long Fsharp = 135;
unsigned long G = 127;
unsigned long Gsharp = 120;
unsigned long A = 113;
unsigned long Asharp =107;
unsigned long B = 101;
unsigned long Chigh = 95;
unsigned long Csharphigh = 90;
unsigned long Dhigh = 85;
unsigned long Dsharphigh = 80;
unsigned long Ehigh = 75;
unsigned long Fhigh = 71;
unsigned long Fsharphigh = 67;
unsigned long Ghigh = 63;
unsigned long Gsharphigh = 60;
unsigned long Chighhigh = 47;
unsigned long Dhighhigh = 42;
unsigned long Ehighhigh = 37;
unsigned long Ghighhigh = 31;


unsigned long eighth_note = 12500;
unsigned long quarter_note = 25000;
unsigned long half_note = 50000;
unsigned long whole_note = 100000;

void playNote(unsigned long freq, unsigned long duration)
{
	//char buffer [50];
	//puts_lcd1(buffer);
	
	duration = duration * tempo;
	unsigned long decr = (freq * volume);
	unsigned long freq1 = freq - decr;
	unsigned long freq2 = freq + decr;
	
	//itoa(freq1, buffer, 10);
	
	

	SET_BIT(DDRA, 0);
	int i;
	for (i = 0; i<duration/(freq * 2); ++i)
	{
		SET_BIT(PORTA, 0);
		wait_avr2(freq1);
		CLR_BIT(PORTA, 0);
		wait_avr2(freq2);
	}
	
}

void wrongMusic(void){
	playNote(A, eighth_note);
	playNote(E, eighth_note);
	playNote(Alow, eighth_note);
}
void correctMusic(void)
{
	playNote(Ehigh, eighth_note);
	playNote(Ghigh, eighth_note);
	playNote(Ehighhigh, eighth_note);
	playNote(Chighhigh, eighth_note);
	playNote(Dhighhigh, eighth_note);
	playNote(Ghighhigh, eighth_note);
}

void youWinMusic(void)
{
	playNote(Fhigh, eighth_note);
	playNote(Fhigh, eighth_note);
	playNote(Fhigh, eighth_note);
	playNote(Fhigh, quarter_note);
	playNote(Csharphigh, quarter_note);
	playNote(Dsharphigh, quarter_note);
	playNote(Fhigh, quarter_note);
	playNote(Dsharphigh, eighth_note);
	playNote(Fhigh, quarter_note + half_note);
}

void youLostMusic(void)
{
	playNote(G, quarter_note + eighth_note);
	playNote(G, quarter_note + eighth_note);
	playNote(G, quarter_note + eighth_note);
	playNote(E, quarter_note + eighth_note);
	playNote(B, eighth_note);
	playNote(G, quarter_note + eighth_note);
	playNote(E, quarter_note + eighth_note);
	playNote(B, eighth_note);
	playNote(G, half_note);
}

void getWord(char word_to_guess[]);
void printCorrectLetters(char word_to_guess[], int letters_right[]);
void printTitle();
int checkLetterInWord(char word_to_guess[], char letter_guess, int letters_right[]);
char getLetterGuess();
unsigned char pressed(unsigned char r,unsigned char c);
unsigned char get_key();

int main(void)
{
	ini_lcd();
	sei();
	TCCR1B |=(1<<CS12)|(1<<WGM12);
	TIMSK |= 1<<OCIE1A;
	
	OCR1A=15625;
	char list_of_words[][20] = {
		"APPLE"
		"HAMBURGER",
		"OREOS",
		"SALAD",
		"CHIPS",
		"PIZZA",
		"DONUTS",
		"GUACAMOLE",
		"CHICKEN",
		"SUSHI",
		"RAVIOLI"
	};
	
	int array_size = sizeof(list_of_words) / sizeof(list_of_words[0]);
	
	char word_to_guess[20];
	char letter_guess;
	
	int num_guess;
	int current_num_guess;
	int lives;
	int random_index;
	int word_length;
	
	char word[81];
	
	printTitle();
	
	while(1){
		srand(time);
		random_index = rand() % array_size;
		strcpy(word_to_guess, list_of_words[random_index]);
		word_length = strlen(word_to_guess);
		
		int letters_right[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		num_guess = 0;
		current_num_guess = 0;
		lives = 9;
		
		while(num_guess < word_length){
		
			printCorrectLetters(word_to_guess, letters_right);
			sprintf(word, "Lives:%d", lives);
			pos_lcd(0,9);
			puts_lcd2(word);
			letter_guess = getLetterGuess();
		
			current_num_guess = checkLetterInWord(word_to_guess, letter_guess, letters_right);
		
			if(current_num_guess == 0){
				lives--;
				clr_lcd();
				puts_lcd2("Wrong Guess");
				wrongMusic();
				if (lives == 0) {
					break;
				}
			}
			else if(current_num_guess < 0){
				clr_lcd();
				puts_lcd2("Already Guess");
				playNote(Alow, half_note);
			}
			else{
				clr_lcd();
				puts_lcd2("Correct Guess");
				correctMusic();
				num_guess += current_num_guess;
			}
		}
		wait_avr(1000);
		if (lives == 0) {
			clr_lcd();
			puts_lcd2("You lose! It was");
			pos_lcd(1,0);
			puts_lcd2(word_to_guess);
			youLostMusic();
		} 
		else {
			clr_lcd();
			puts_lcd2("YOU WIN!!");
			youWinMusic();
		}
	}
}

void printTitle(){
	unsigned char keypadButton = 0;

	clr_lcd();
	puts_lcd2("  Word Hunter!");
	pos_lcd(1,0);
	puts_lcd2("Press Any Button");
	
	while(!keypadButton){
		keypadButton = get_key();
	}
	clr_lcd();
}

void printCorrectLetters(char word_to_guess[], int letters_right[]){
	int index = 0;
	int word_length = strlen(word_to_guess);
	clr_lcd();
	pos_lcd(1,0);
	for( index = 0; index < word_length; index++) {
		if(letters_right[index] == 1) {
			put_lcd(word_to_guess[index]);
		} 
		else {
			put_lcd('_');
		}
	}
}

int checkLetterInWord(char word_to_guess[], char letter_guess, int letters_right[]){
	//Returns -1 if already guess, return number of correct letters otherwise
	int i = 0;
	int word_length = strlen(word_to_guess);
	int number_of_correct_letters = 0;
	
	for(i = 0; i < word_length; i++) {
		if(letters_right[i] == 1) {
			if(word_to_guess[i] == letter_guess) {
				number_of_correct_letters = -1;
				break;
			}
			continue;
		}
		if( letter_guess == word_to_guess[i] ) {
			letters_right[i] = 1;
			number_of_correct_letters++;
		}
	}
	return number_of_correct_letters;
}

char getLetterGuess(){
	unsigned char keypadButton;
	int keypadPressed = 1;
	char letter = 'A';
	
	pos_lcd(0,0);
	puts_lcd2("Pick:");
	
	while(1){
		keypadButton = get_key();
		if(!keypadPressed && keypadButton){
			keypadPressed = 1;
			if(keypadButton == 16){
				break;
			}
			else if(keypadButton == 1){
				if(letter < 65 || letter >= 90)
					letter = 'A';
				else
					letter++;
			}
			else if(keypadButton == 2){
				if(letter < 65 || letter >= 67)
					letter = 'A';
				else
					letter++;
			}
			else if(keypadButton == 3){
				if(letter < 68 || letter >= 70)
					letter = 'D';
				else
					letter++;
			}
			else if(keypadButton == 5){
				if(letter < 71 || letter >= 73)
					letter = 'G';
				else
					letter++;
			}
			else if(keypadButton == 6){
				if(letter < 74 || letter >= 76)
					letter = 'J';
				else
					letter++;
			}
			else if(keypadButton == 7){
				if(letter < 77 || letter >= 79)
					letter = 'M';
				else
					letter++;
			}
			else if(keypadButton == 9){
				if(letter < 80 || letter >= 83)
					letter = 'P';
				else
					letter++;
			}
			else if(keypadButton == 10){
				if(letter < 84 || letter >= 86)
					letter = 'T';
				else
					letter++;
			}
			else if(keypadButton == 11){
				if(letter < 87 || letter >= 90)
					letter = 'W';
				else
					letter++;
			}
		}
		else if(!keypadButton){
			pos_lcd(0,5);
			put_lcd(letter);		
			keypadPressed = 0;
		}
	}
	return letter;
}
unsigned char pressed(unsigned char r,unsigned char c){
	DDRC  = 0X0f;
	PORTC = 0xff;
	
	SET_BIT(DDRC, r);
	CLR_BIT(PORTC, r);

	wait_avr(1);
	
	if (GET_BIT(PINC, c+4))
	{
		return 0;
	}
	return 1;
}

unsigned char get_key(){
	unsigned char r, c;
	for (r = 0; r < 4; r++)
	{
		for (c = 0; c < 4; c++)
		{
			if(pressed(r, c))
			{
				return (r * 4) + c + 1;
			}
		}
	}
	return 0;
}