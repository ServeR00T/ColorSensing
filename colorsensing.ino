/**
   Microcomputer System Design Project
   Color Sensing with RGB LED and photoresistor
   The team:
   Saleem AlOtaibi
   Saeed AlMalki
   Wael AlAbdali
*/
const int sensorPin = A0;
const int rgbPins[] = {11, 10, 9};      // R,G,B anode pins
const int rgbOutPins[] = {6, 5, 3};     // R,G,B anode pins
const int rTimes  = 5;                  // Reading times
const int delayMs = 20;                 // delay in ms

int rgbValues[] = {0, 0, 0};        // R, G, B
int maxwhite[]  = {381, 461, 409};  // R, G, B
int minblack[]  = {105, 134, 128};  // R, G, B

bool requireSerial = false;
bool calibrated = true;       // To be configured
unsigned long t1, t2 = 0;     // To calculate time

void setup()
{
	pinMode(rgbPins[0], OUTPUT);    // Red anode
	pinMode(rgbPins[1], OUTPUT);    // Green anode
	pinMode(rgbPins[2], OUTPUT);    // Blue anode
	pinMode(rgbOutPins[0], OUTPUT); // Output Red anode
	pinMode(rgbOutPins[1], OUTPUT); // Output Green anode
	pinMode(rgbOutPins[2], OUTPUT); // Output Blue anode
	pinMode(sensorPin, INPUT);      // Photoresistor

	Serial.begin(9600);             // Begin serial with 9600 baud rate
	if (requireSerial) while (!Serial);

	// Calibration if it hasn't
	if (!calibrated)
	{
		calibration();
		calibrated = true;
	}
}

void loop()
{
	setColor(0, 0, 0); // Turn off the RGB LED
	delay(delayMs);

	// Reading from the sensor
	t1 = micros();
	for (int i = 0; i < rTimes; i++)
	{
		setColor(255, 0, 0); // Red color
		delay(delayMs);
		rgbValues[0] += analogRead(sensorPin);

		setColor(0, 255, 0); // Green color
		delay(delayMs);
		rgbValues[1] += analogRead(sensorPin);

		setColor(0, 0, 255); // Blue color
		delay(delayMs);
		rgbValues[2] += analogRead(sensorPin);
	}
	t2 = micros();

	setColor(0, 0, 0); // Turn off the RGB LED

	// Dividing the value of the color by the number
	// of readings to get it in [0-1023] (Average)
	// i for color in RGB, 0 red, 1 green, 2 blue
	for (int i = 0; i < 3; i++)
	{
		rgbValues[i] /= rTimes;
	}

	printResults(1, "");  // Print the result to Serial Monitor

	// scale the value to [0,255], i for color in RGB
	for (int i = 0; i < 3; i++)
	{
		// Cut the value to be between min black and max white
		rgbValues[i] = constrain(rgbValues[i], minblack[i], maxwhite[i]);
		// Get it in 8-bit value [0,255]
		rgbValues[i] = map(rgbValues[i], minblack[i], maxwhite[i], 0, 255);
	}

	printResults(0, "in 255");  // Print the result to Serial Monitor

	// Lighting the RGB with the same color
	for (int i = 0; i < 3; i++)
	{
		analogWrite(rgbOutPins[i], rgbValues[i]);
	}

	// Resetting the read values
	rgbValues = {0, 0, 0};
}

/**
   Calibrate the system in various lighting enviroments
*/
void calibration()
{
	Serial.println("Calibrating ...");
	Serial.println("Read The MAX. White");
	delay(5000);

	setColor(255, 0, 0); // Red color
	delay(delayMs);
	maxwhite[0] = rgbValues[0] = analogRead(sensorPin) * 1.126;

	setColor(0, 255, 0); // Green color
	delay(delayMs);
	maxwhite[1] = rgbValues[1] = analogRead(sensorPin) * 1.126;

	setColor(0, 0, 255); // Blue color
	delay(delayMs);
	maxwhite[2] = rgbValues[2] = analogRead(sensorPin) * 1.126;

	setColor(0, 0, 0);    // Turn off the RGB LED
	printResults(0, "");  // Print results

	Serial.println("Read The MIN. Black");
	delay(5000);

	setColor(255, 0, 0); // Red color
	delay(delayMs);
	minblack[0] = rgbValues[0] = analogRead(sensorPin) * 1.333;

	setColor(0, 255, 0); // Green color
	delay(delayMs);
	minblack[1] = rgbValues[1] = analogRead(sensorPin) * 1.333;

	setColor(0, 0, 255); // Blue color
	delay(delayMs);
	minblack[2] = rgbValues[2] = analogRead(sensorPin) * 1.333;

	setColor(0, 0, 0);    // Turn off the RGB LED
	printResults(0, "");  // Print results
	Serial.println("Calibration done.");
}

/**
   Set the color of RGB LED
*/
void setColor(int r, int g, int b)
{
	analogWrite(rgbPins[0], r);
	analogWrite(rgbPins[1], g);
	analogWrite(rgbPins[2], b);
}

/**
   Print the result on Serial monitor
*/
void printResults(bool withTime, String string)
{
	if (withTime)
	{
		Serial.print(((double)t2 - t1) / 1000000);  // To get the time in seconds
		Serial.print("s\t=> ");
	}
	else
	{
		Serial.print(string);
		Serial.print("\t=> ");
	}

	for (int i = 0; i < 3; i++)
	{
		Serial.print(rgbValues[i]);
		if (i != 2)
			Serial.print(", ");
	}
	Serial.println();
}
