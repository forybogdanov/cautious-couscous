#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <string.h>
#include <Servo.h>

fef'e.[f

const byte relayPin = 13;

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] =
{
{ '1','2','3','A' },
{ '4','5','6','B' },
{ '7','8','9','C' },
{ '*','0','#','D' }
};
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };

int pos = 0;
bool changePassword = false;
int profileId = -1;
char passwords[4][6] =
{
	{'1', '2', '3', '4', '5', '6'},
	{'1', '2', '3', '4', '5', '6'},
	{'1', '2', '3', '4', '5', '6'},
	{'1', '2', '3', '4', '5', '6'}
};
char inputCode[6] = { '-', '-', '-', '-', '-', '-' };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x20, 16, 2);
Servo motor;

void setup()
{
	motor.attach(10);
	motor.write(0);
	Serial.begin(9600);
	lcd.init();
	lcd.backlight();
	pinMode(relayPin, OUTPUT);
	lcd.setCursor(0, 0);
	lcd.print("PLEASE SELECT");
	lcd.setCursor(0, 1);
	lcd.print("PROFILE: ");
	delay(2000);
}

void displayMessage(String message) {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(message);
}

void loop()
{
	readKey();
}

void readKey()
{
	int correct = 0;
	int i;
	char customKey = customKeypad.getKey();
	if (customKey)
	{
		switch (customKey)
		{
		case 'A':
			profileId = 0;
			displayMessage("ENTER PASSWORD:)");
			break;
		case 'B':
			profileId = 1;
			displayMessage("ENTER PASSWORD:)");
			break;
		case 'C':
			profileId = 2;
			displayMessage("ENTER PASSWORD:)");
			break;
		case 'D':
			profileId = 3;
			displayMessage("ENTER PASSWORD:)");
			break;
		case '*':
			changePassword = true;
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("OLD PASSWORD:");
			break;
		case '#':
			for (i = 0; i < 6; i++)
			{
				if (inputCode[i] == passwords[profileId][i])
				{
					correct++;
				}
			}
			if (correct == 6)
			{
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Input correct!");
				delay(1500);
				if (changePassword)
				{
					delay(1000);
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print("NEW PASSWORD:");
					i = 0;
					while (i < 6)
					{
						char secretKey = customKeypad.getKey();
						if (secretKey)
						{
							passwords[profileId][i] = secretKey;
							lcd.setCursor(i, 1);
							lcd.print(passwords[profileId][i]);
							i++;
						}
					}
					changePassword = false;
					displayMessage("Successful!");
					delay(2000);
				}
				else
				{
					lcd.setCursor(0, 0);
					lcd.print(" Please Come In ");
					digitalWrite(relayPin, HIGH);
					motor.write(90);
					delay(2000);
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print("TIME OUT!");
					motor.write(0);
					delay(600);
					digitalWrite(relayPin, LOW);
					delay(2000);
					inputCode[0]++;
				}
			}
			else
			{
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Locked:( ");
				lcd.setCursor(0, 1);
				lcd.print(" Try Again ");
				digitalWrite(relayPin, LOW);
				delay(2000);
			}
			pos = 0;
			profileId = -1;
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("PLEASE SELECT");
			lcd.setCursor(0, 1);
			lcd.print("PROFILE: ");
			break;
		default:
			inputCode[pos] = customKey;
			lcd.setCursor(pos, 1);
			lcd.print('*');
			pos++;
		}
	}
}
