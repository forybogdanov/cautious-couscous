#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <string.h>
#include <Servo.h>

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
const int NOT_SELECTED_PROFILE = -1;
int profileId = NOT_SELECTED_PROFILE;
const String CHANGE_PASSWORD_MODE = "change password mode";
const String UNLOCK_DOOR_MODE = "unlock door mode";
String modeSelected = "";
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

/// <summary>
/// initial settings of the device
/// </summary>
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

/// <summary>
/// clears the screen and displays a new message by a given string
/// </summary>
/// <param name="message"></param>
void displayMessage(String message) {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(message);
}

/// <summary>
/// returns true or false based on whether a profile is selected
/// </summary>
/// <returns></returns>
bool isProfileSelected() {
	return profileId != NOT_SELECTED_PROFILE;
}

/// <summary>
/// returns true or false based on whether a mode is selected
/// </summary>
/// <returns></returns>
bool isModeSelected() {
	return modeSelected != "";
}

/// <summary>
/// sets the profile id and prints messages for the two possible mods
/// </summary>
/// <param name="id"></param>
void selectProfile(int id) {
	if (isProfileSelected)
	{
		profileId = id;
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("* - CHANGE PASS");
		lcd.setCursor(0, 1);
		lcd.print("# - UNLOCK DOOR");
	}
}

/// <summary>
/// resets all variables to initial values
/// </summary>
void resetLock() {
	pos = 0;
	profileId = NOT_SELECTED_PROFILE;
	modeSelected = "";
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PLEASE SELECT");
	lcd.setCursor(0, 1);
	lcd.print("PROFILE: ");
	for (int i = 0; i < 6; i++) {
		inputCode[i] = '-';
	}
	changePassword = false;
}

/// <summary>
/// endless function - the heart of the app
/// </summary>
void loop()
{
	int correct = 0;
	int i;
	/// <summary>
	/// reads a key from the keypad
	/// </summary>
	char customKey = customKeypad.getKey();
	if (customKey)
	{
		switch (customKey)
		{
		case 'A':
			selectProfile(0);
			break;
		case 'B':
			selectProfile(1);
			break;
		case 'C':
			selectProfile(2);
			break;
		case 'D':
			selectProfile(3);
			break;
		case '*':
			/// <summary>
			/// if the user has selected a profile and has not selected a mode it selects change pass mode
			/// </summary>
			if (isProfileSelected() && !isModeSelected())
			{
				modeSelected = CHANGE_PASSWORD_MODE;
				changePassword = true;
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("OLD PASSWORD:");
			}
			break;
		case '#':
			/// <summary>
			/// if there is no user we don't go furhter
			/// </summary>
			if (!isProfileSelected()) 
			{
				break;
			}
			/// <summary>
			/// if there is no mode selected we select unlock door mode
			/// </summary>
			if (!isModeSelected())
			{
				modeSelected = UNLOCK_DOOR_MODE;
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("ENTER PASSWORD:");
				break;
			}
			/// <summary>
			/// counts the correct numbers
			/// </summary>
			for (i = 0; i < 6; i++)
			{
				if (inputCode[i] == passwords[profileId][i])
				{
					correct++;
				}
			}
			/// <summary>
			/// if all numbers are correct it checks if the mode is change pass mode or unlock door
			/// if it is change pass mode it asks the user to enter the new one
			/// if it is unlock door mode it unlocks it
			/// </summary>
			if (correct == 6)
			{
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Input correct!");
				delay(1000);
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
				}
			}
			else
			{
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Wrong password");
				lcd.setCursor(0, 1);
				lcd.print(" Try Again ");
				digitalWrite(relayPin, LOW);
				delay(2000);
			}
			resetLock();
			break;
		default:
			if (isModeSelected())
			{
				inputCode[pos] = customKey;
				lcd.setCursor(pos, 1);
				lcd.print('*');
				pos++;
			}
		}
	}
}
