#ifndef CAR_ENGINE_H_
#define CAR_ENGINE_H_

// See http://www.asawicki.info/Mirror/Car%20Physics%20for%20Games/Car%20Physics%20for%20Games.html

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

class CarEngine {
public:
	CarEngine() : m_v(0.0), m_rpm(0.0), m_gear(0) {
	}

	void update(double time_inc /*s*/, double acc_pedal /*0..1*/, double brk_pedal /*0..1*/) {
		if (acc_pedal < 0.0) {
			acc_pedal= 0.0;
		}
		if (acc_pedal > 1.0) {
			acc_pedal= 1.0;
		}
		if (brk_pedal < 0.0) {
			brk_pedal= 0.0;
		}
		if (brk_pedal > 1.0) {
			brk_pedal= 1.0;
		}

		double rpm = m_rpm; /*rev/m*/
		if( rpm < 1000 ) {
			rpm = 1000;
		}

		double v = fabs(m_v); /*km/h*/
		double f_drag = F_drag(v); /*N*/
		double f_rr = F_rr(v); /*N*/
		double f_traction = acc_pedal * Max_F_traction(rpm, m_gear); /*N*/
		double f_brake = brk_pedal * Max_F_brake; /*N*/
		double force = f_traction - f_drag - f_rr - f_brake; /*N*/
		double acceleration = force / Mass; /*m/s^2*/

		v = v /*km/h*/ + (acceleration /*m/s^2*/ * time_inc /*s*/ * 3600.0 /*s/h*/ / 1000.0 /*km/m*/);
		if (v < 0.0) {
			v = 0.0;
		}

		m_rpm = Rpm(v, m_gear);
		if (m_gear == 7) {
			m_v = -v;
		} else {
			m_v = v;
		}
	}

	bool setGear(int gear) {
		if (m_gear != gear) {
			if (gear < 0 || gear > 7) { /*invalid*/
				m_gear = 0;
			} else if (m_v >= 0 && gear <= 6) { /*direct*/
				m_gear = gear;
			} else if (m_v <= 0 && gear == 7) { /*backwards*/
				m_gear = gear;
			} else {
				m_gear = 0;
			}
			m_rpm = Rpm(fabs(m_v), m_gear);
		}
		return (gear != 0);
	}

	double getV() {
		return fabs(m_v);
	}

	double getRpm() {
		return m_rpm;
	}

	int getGear() {
		return m_gear;
	}

private:
	double m_v;
	double m_rpm;
	int m_gear;

	static const double R_wheel     = 0.34;  // Wheel Radius [m]
	static const double Mass        = 1500;  // Mass [Kg]
	static const double Max_F_brake = 4.0 * 7350;  // Max Brake [N]

	/*
	 * Flong =   Ftraction - Fdrag - Frr
	 *
	 * Fdrag =  0.5 * Cd * A * rho * v^2
	 *    where Cd = coefficient of friction 
	 *    A is frontal area of car 
	 *    rho (Greek symbol )= density of air 
	 *    v = speed of the car
	 *
	 * Air density (rho) is 1.29 kg/m3 (0.0801 lb-mass/ft3)
	 * frontal area is approx. 2.2 m2 (20 sq. feet)
	 * Cd depends on the shape of the car and determined via wind tunnel tests.
	 *    Approximate value for a Corvette: 0.30
	 *
	 * Frr = - Crr * v 
	 *    where Crr is a constant and v is the velocity vector.
	 *
	 * At low speeds the rolling resistance is the main resistance force
	 * At high speeds the drag takes over in magnitude.
	 * At approx. 100 km/h (60 mph, 30 m/s) they are equal ([Zuvich]).
	 * This means Crr must be approximately 30 times the value of Cdrag
	 */

	static const double C_drag = (0.5 * 0.30 * 2.2 /*m^2*/ * 1.29 /*kg/m^3*/);
	static inline double F_drag(double v /*km/h*/) {
		v = v * 1000.0 /*km/m*/ / 3600.0 /*s/h*/; /*m/s*/
		return (C_drag * v * v);
	}

	static const double C_rr = ((0.5 * 0.30 * 2.2 * 1.29) * 30.0);
	static inline double F_rr(double v /*km/h*/) {
		v = v * 1000.0 /*km/m*/ / 3600.0 /*s/h*/; /*m/s*/
		return (C_rr * v);
	}

	/*
	 * The combination of gear and differential acts as a multiplier
	 * from the torque on the crankshaft to the torque on the rear wheels
	 */
	static const double Gear_1st  = 2.66;
	static const double Gear_2nd  = 1.78;
	static const double Gear_3rd  = 1.30;
	static const double Gear_4th  = 1.0;
	static const double Gear_5th  = 0.74;
	static const double Gear_6th  = 0.50;
	static const double Gear_Rev  = 2.90; // Reverse

	static const double X_d       = 3.42; // Differential Ratio

	static inline double GearTrCoeff(int gear) {
		switch (gear) {
			case 1: return (Gear_1st * X_d);
			case 2: return (Gear_2nd * X_d);
			case 3: return (Gear_3rd * X_d);
			case 4: return (Gear_4th * X_d);
			case 5: return (Gear_5th * X_d);
			case 6: return (Gear_6th * X_d);
			case 7: return (Gear_Rev * X_d);
			default: return 0.0;
		}
	}

	static const double Tr_eff    = 0.7;  // Transmission Efficiency

	/*
	 * in the static situation, the weight on the rear wheels is half the
	 * weight of the car and driver: (1500 kg / 2 ) * 9.8 m/s2 = 7350 N.
	 * This means the maximum amount of traction the rear wheels can provide
	 * if mu = 1.0 is 7350 N
	 */
	static const double Max_Tract = 7350; // Max Traction per Wheel [N]

	/*
	 *   Rev [rpm]  Torque [N.m]
	 *     1000       394
	 *     2000       436
	 *     3000       456
	 *     4000       475
	 *     4400       475
	 *     5000       470
	 *     6000       386
	 */
	static inline double Max_Torque(double rpm) { /*N.m*/
		if (rpm < 0.0) {
			return 0.0;
		} else if (rpm > 7000.0) {
			return 0.0;
		}
		return (
			- 2.294544012e-12 * pow(rpm,4)
			+ 2.628558309e-8  * pow(rpm,3)
			- 1.045311629e-4  * pow(rpm,2)
			+ 1.890208069e-1  * rpm
			+ 296.4565158
		);
	}

	static inline double Max_F_traction(double rpm /*rev/m*/ , int gear) { /*N = m.kg/s^2*/
		double max_torque = Max_Torque(rpm); /*N.m*/
		double gear_coeff = GearTrCoeff(gear);
		double force = max_torque /*N.m*/ * gear_coeff * Tr_eff / R_wheel /*m*/; /*N*/
		return force; /*N*/
	}

	static inline double Rpm(double v /*km/h*/, int gear) {
		double wheel_rot /*rad/s*/ = (v /*km/h*/ * 1000.0 /*km/m*/ / 3600.0 /*s/h*/) / R_wheel /*m*/; /*rad /sec*/
		double rpm /*rev/min*/ = wheel_rot /*rad/s*/ * GearTrCoeff(gear) * 60.0 /*s/m*/ / (2 * M_PI) /*rad/rev*/;
		if( rpm < 1000 ) {
			rpm = 1000;
		}
		return rpm;
	}

};

#endif // CAR_ENGINE_H_
