#include "SixWheelDrive.h"
//#include "../RobotMap.h"
#include "Driverstation.h"
#include <cstdio>


SixWheelDrive::SixWheelDrive(Constants *LucielleBall) :
		Subsystem("SixWheeelDrive")
{
	constants = LucielleBall;
	leftBack = new VictorSP(constants->Get("backLeftDrive"));
	rightBack  = new VictorSP(constants->Get("backRightDrive"));
	leftFront  = new VictorSP(constants->Get("frontLeftDrive"));
	rightFront  = new VictorSP(constants->Get("frontRightDrive"));
	leftMid = new VictorSP(constants->Get("leftMidDrive"));
	rightMid = new VictorSP(constants->Get("rightMidDrive"));
	leftBack->SetInverted(constants->Get("leftBackInvert")== 1);
	leftMid->SetInverted(constants->Get("leftMidInvert")== 1);
	leftFront->SetInverted(constants->Get("leftFrontInvert")== 1);
	rightBack->SetInverted(constants->Get("rightBackInvert")== 1);
	rightMid->SetInverted(constants->Get("rightMidInvert")== 1);
	rightFront->SetInverted(constants->Get("rightFrontInvert")== 1);
	Drive = new RobotDrive (leftFront, leftBack, rightFront, rightBack);
	shifter = new Solenoid(constants->Get("ShifterSolenoid"));
	ShifterTest = false;
	RightWheelEncoder = new Encoder(constants->Get("RightEncoderA"), constants->Get("RightEncoderB"));
	LeftWheelEncoder = new Encoder(constants->Get("LeftEncoderA"), constants->Get("LeftEncoderB"));
	Drive->SetSafetyEnabled(false);
	try {
		/* Communicate w/navX-MXP via the MXP SPI Bus.                                       */
		/* Alternatively:  I2C::Port::kMXP, SerialPort::Port::kMXP or SerialPort::Port::kUSB */
		/* See http://navx-mxp.kauailabs.com/guidance/selecting-an-interface/ for details.   */
		ahrs = new AHRS(SPI::Port::kMXP);
	} catch (std::exception ex ) {
		std::string err_string = "Error instantiating navX-MXP:  ";
		err_string += ex.what();
		DriverStation::GetInstance().ReportError(err_string.c_str());
	if (ahrs){
		LiveWindow::GetInstance()->AddSensor("IMU", "Angle", ahrs);
	}
	}

}

void SixWheelDrive::InitDefaultCommand()
{
	// Set the default command for a subsystem here.
	//SetDefaultCommand(new MySpecialCommand());
	//precision speed
}

long SixWheelDrive::LeftEncoder(){



	return LeftWheelEncoder->Get();
}
long SixWheelDrive::RightEncoder(){


	return RightWheelEncoder->Get();
}
void SixWheelDrive::arcadeDrive(float Y, float X, bool isHighGear, bool isLowGear){

	if (isHighGear)//high gear
		ShifterTest = true;

	if (isLowGear)//low gear
		ShifterTest = false;

	if (ShifterTest){
		X=X*constants->Get("HighGearXGain");
		Y=Y*constants->Get("HighGearYGain");
	}
	else{
		X=X*constants->Get("LowGearXGain");
		Y=Y*constants->Get("LowGearYGain");
	}

	arcadeDrive(Y,X,ShifterTest);

}
void SixWheelDrive::arcadeDrive(float Y, float X, bool isHighGear){
	char c[100];
	sprintf(c,"%f", rightFront->Get());
	DriverStation::GetInstance().ReportError("Channel of rightBack ");
	DriverStation::GetInstance().ReportError(c);
	Drive->ArcadeDrive(Y, X, false);
	rightMid->Set(rightFront->Get());
	leftMid->Set(leftFront->Get());

	ShifterTest=isHighGear;

	shifter->Set(ShifterTest);
	ahrs->GetAngle();
}

