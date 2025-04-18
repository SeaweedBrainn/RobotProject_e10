#pragma config(Sensor, dgtl2,  rightSonar,     sensorSONAR_inch)
#pragma config(Sensor, dgtl4,  leftSonar,      sensorSONAR_inch)
#pragma config(Sensor, dgtl6,  frontSonar,     sensorSONAR_inch)
#pragma config(Motor,  port2,           leftMotor,     tmotorServoContinuousRotation, openLoop, reversed)
#pragma config(Motor,  port9,           rightMotor,    tmotorServoContinuousRotation, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

void backUp(float time)
{
	motor[rightMotor] = -65.4;
	motor[leftMotor] = -63;
	wait1Msec(time);
}

void rotate(float time, int t)  //t = left or right, 1 is TURN right, 2 is TURN left
{
	switch (t)
	{
		case 1:
			motor[rightMotor] = -127;		  // Motor on port2 is run at full (-127) power reverse
			motor[leftMotor]  = 127;
			wait1Msec(time);
		case 2:
			motor[rightMotor] = 127;		  // Motor on port2 is run at full (-127) power reverse
			motor[leftMotor]  = -127;
			wait1Msec(time);
	}
}

void stopBoth()
{
	stopMotor(rightMotor);
	stopMotor(leftMotor);
}


task main()
{
	int frontValue;
	int leftValue;
	int rightValue;
	int difference;


	while(true)
	{
		motor[rightMotor] = 65.4;
		motor[leftMotor] = 63;
		frontValue = SensorValue(frontSonar);
		rightValue = SensorValue(rightSonar);
		if (frontValue < 5)
			{
				stopBoth();
				if (rightValue < 12)   // difference > 2 means the robo should turn RIGHT
				{
				//	backUp(300);
					rotate(1000, 2);
				}
				else if (rightValue > 12) // turn LEFT
				{
				//	backUp(300);
					rotate(1000, 1);
				}
				else
				{
					difference = 0;
					frontValue = 0;
					leftValue = 0;
					rightValue = 0;
				}
			}
	}


}
