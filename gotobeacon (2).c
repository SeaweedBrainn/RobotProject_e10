#pragma config(Sensor, in1,    analog1,        sensorAnalog)
#pragma config(Sensor, dgtl1,  limitSwitch,    sensorTouch)
#pragma config(Sensor, dgtl10, digital10,      sensorDigitalOut)
#pragma config(Sensor, dgtl11, digital11,      sensorDigitalOut)
#pragma config(Sensor, dgtl12, digital12,      sensorDigitalOut)
#pragma config(Motor,  port1,           rightMotor,    tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           leftMotor,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           armMotor,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Sensor, dgtl3,  rightSonar,     sensorSONAR_inch)
#pragma config(Sensor, dgtl4,  leftSonar,      sensorSONAR_inch)
#pragma config(Sensor, dgtl5,  backSonar,     sensorSONAR_inch)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Global Variables
int freq, ambient_level, slow_level, stop_level, expose_time, steer_sensitivity, forward_speed, slow_speed, spin_speed;

// initialize PD values for the global uses - will be modified at the appropriate timing in ReadPD function.
int PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD_sum;
// initialize max values
int max_val, max_no = 0;

/*The accumulator accumulates(or integrates) the rectified signal over a period of time (set by the expose and read)
The accumulated voltage read by SensorValue[analog1](an analog voltage) is read by the controller.
The expose_time period essentially set the ?gain? or ?sensitivity? of the overall circuit.*/
int Expose_and_read(){
	SensorValue[digital11] = 1; // close the shutter, clear the film, and increment the counter
	delay(5); // Wait for 5ms for things to settle
	SensorValue[digital11] = 0; // open shutter for exposure
	delay(expose_time); // expose time = 3ms to 8ms (can be adjusted)
	int intensity = SensorValue[analog1]; // get the IR intensity reading
	return intensity; // return the analog input 1
}

/*Read_PD calculates PD_sum which is the sum of all 8 photo diodes?outputs.
PD_sum is a measure of the distance between the robot and the beacon. The sensing
sensitivity is set by expose_time*/

void ReadPD(){
	SensorValue[digital11] = 1; // close the shutter, clear the film, and increment the counter
	SensorValue[digital12] = 1; // initialize counter value to '0'
	SensorValue[digital12] = 0; // allow counter to count
	delay(5); // 5ms wait for things settling down
	SensorValue[digital11] = 0; // open shutter and expose film
	delay(expose_time); // expose time = 3ms to 8ms (can be adjusted)
	PD0 = SensorValue[analog1];
	PD1 = Expose_and_read();
	PD2 = Expose_and_read();
	PD3 = Expose_and_read();
	PD4 = Expose_and_read();
	PD5 = Expose_and_read();
	PD6 = Expose_and_read();
	PD7 = Expose_and_read();
	PD_sum = PD0 + PD1 + PD2 + PD3 + PD4 + PD5 + PD6 + PD7;
}

/*This function compares the magnitude of the variables PD0~PD7 and stores the maximum value
in max_val and the number of the photo diode that has the maximum value in max_no.*/

void Find_max(){
	max_val = PD0; // default = PD0;
	max_no = 0;

	if(PD1 > max_val){
		max_val = PD1;
		max_no = 1;
	}
	if(PD2 > max_val){
		max_val = PD2;
		max_no = 2;
	}
	if(PD3 > max_val){
		max_val = PD3;
		max_no = 3;
	}
	if(PD4 > max_val){
		max_val = PD4;
		max_no = 4;
	}
	if(PD5 > max_val){
		max_val = PD5;
		max_no = 5;
	}
	if(PD6 > max_val){
		max_val = PD6;
		max_no = 6;
	}
	if(PD7 > max_val){
		max_val = PD7;
		max_no = 7;
	}
}

/*
Limits results of move function to max speed	and output to the motors*/
int limit_pwm(int temp){
	int limited = 0;

	if(temp > 127){
		limited = 127;
	} else if (temp < -127){
		limited = -127;
	} else{
		limited = temp;
	}

	return limited;

}

/*Move function helps to check if beacon is found or otherwise it will be search mode	 looking for the beacon.
This function also calculates the speed, steering sensitivity and the heading direction so the robot moves to beacon*/

void Move(){
	int tempSpeed = 0;
	int error = 4 - max_no;//heading direction error , if PD4 == max_no, then no error
	int steer = error * steer_sensitivity;//steering effort is proportioinal to heading error
	int speed = forward_speed;//forward speed (normal speed)

	if(PD_sum < ambient_level){ // looking for the beacon if background noise is
		speed = 0;//search mode => no forward motion
		steer = -spin_speed;//search mode => spin
	}
	if(PD_sum > slow_level){//Beacon is near!
		speed = slow_speed;//slow down
	}
	// deleted the stop statement when the beacon is found, because we'll	 use the limit switch to stop the motors	which is more reliable.


	tempSpeed = limit_pwm(steer + speed);
	motor[port1] = tempSpeed; // right, port 1
	tempSpeed = limit_pwm(steer - speed);
	motor[port10] = -tempSpeed; // left, port	 10
}

void moveMotors(int n)
{
	motor[rightMotor] = n;
	motor[leftMotor] = n;
}

void stopMoving()
{
	motor[rightMotor] = 0;
	motor[leftMotor]= 0;
}


void operateArm(int n)
{
	motor[armMotor] = n;
}

void backUp(float time)
{
	motor[rightMotor] = -127;
	motor[leftMotor] = -127;
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


/*
The GOBEACON main program essentially sets up all the configuration variables and repeatedly
execute the three routines: Read_PD, find_max, and move.*/
task main(){
	freq = 0; // 0 = 1khz (red) 1 = 10khz (green)
	ambient_level = 200; // used in 'move'
	slow_level = 5000;// used in move
	stop_level = 6000;//used in move
	expose_time = 5; // expose time was changed from 3ms to 5ms (3ms in easyC -> 5ms in RobotC)
	steer_sensitivity = 20;//used in move
	forward_speed = 35;//forward speed , used in move
	slow_speed = 25;//slow speed , used in move
	spin_speed = 50;//spin speed (for searching mode),used in move
	SensorValue[digital10] = freq;// turn to 1KHz(red beacon)
	int backValue;
	int leftValue;
	int rightValue;
	int difference;

	int state = 1;

	while(state == 1){
		ReadPD();
		Find_max();
		Move();

		int lmtSwitch = SensorValue[limitSwitch];

		if (lmtSwitch == true){
			stopMoving();
			state = 2;
		}

	}

	while(state == 2){
		//Turn off red button
		ReadPD();

		operateArm(12.7);
		wait1Msec(3000);
		operateArm(-12.7);
		wait1Msec(3000);
		operateArm(0);

		if(PD_sum < 3000){
			moveMotors(-127);
			wait1Msec(1000);
			stopMoving();
			state = 3;
		}
	}

		freq = 1;
		SensorValue[digital10] = freq;// turn to 10KHz(green beacon)

	while(state == 3){
		//Go to green
		ReadPD();
		Find_max();
		Move();

		int lmtSwitch = SensorValue[limitSwitch];

		if (lmtSwitch == true){
			stopMoving();
			state = 4;
		}
	}

	while(state == 4){
		//Capture green
		ReadPD();

		operateArm(12.7);
		wait1Msec(3000);
		operateArm(0);

		if(PD_sum < 3000){
			moveMotors(-127);
			wait1Msec(1000);
			stopMoving();
			state = 5;
		}
	}

	while(state == 5){
		//Exit arena
		ReadPD();

		backValue = SensorValue(backSonar);
		rightValue = SensorValue(rightSonar);
		if (backValue < 5)
			{
				stopMoving();
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
					backValue = 0;
					leftValue = 0;
					rightValue = 0;
				}
			}

		}
}
