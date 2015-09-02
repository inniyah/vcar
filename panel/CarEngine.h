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

	void update(double time_inc, double acc_pedal, double brk_pedal) {
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
		double v = fabs(m_v);
		double rpm = Rpm(v, m_gear);
		double f_drag = F_drag(v);
		double f_rr = F_rr(v);
		double f_traction = acc_pedal * Max_F_traction(rpm, m_gear);
		double f_brake = brk_pedal * Max_F_brake;
		double force = f_traction - f_drag - f_rr - f_brake;
		double acceleration = force / Mass;
		v = v + acceleration * time_inc;
		if (v < 0.0) {
			v = 0.0;
		}
		m_rpm = Rpm(v, m_gear);
		if (m_v < 0) {
			m_v = -v;
		} else {
			m_v = v;
		}
	}

	bool setGear(int gear) {
		if (m_gear != gear) {
			if (gear < 0 || gear > 7) {
				m_gear = 0;
			} else if (m_v >= 0 && gear <= 6) {
				m_gear = gear;
			} else if (m_v <= 0 && gear == 7) {
				m_gear = gear;
			} else {
				m_gear = 0;
			}
			m_rpm = Rpm(fabs(m_v), m_gear);
		}
		return (gear != 0);
	}

	double getV() {
		return m_v;
	}

	double getRpm() {
		return m_rpm;
	}

private:
	double m_v;
	double m_rpm;
	int m_gear;

	static const double R_wheel     = 0.34;  // Wheel Radius [m]
	static const double Mass        = 1500;  // Mass [Kg]
	static const double Max_F_brake = 7350;  // Max Brake [N]

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

	static const double C_drag = (0.5 * 0.30 * 2.2 * 1.29);
	static inline double F_drag(double v) {
		return (C_drag * v * v);
	}

	static const double C_rr = (12.8);
	static inline double F_rr(double v) {
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
	static const double Tr_eff    = 0.7;  // Transmission Efficiency

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

	/*
	 * in the static situation, the weight on the rear wheels is half the
	 * weight of the car and driver: (1500 kg / 2 ) * 9.8 m/s2 = 7350 N.
	 * This means the maximum amount of traction the rear wheels can provide
	 * if mu = 1.0 is 7350 N
	 */
	static const double Max_Tract = 7350; // Max Traction  [N]

	/*
	 *   Rev [rpm]  Torque [N.m]
	 *     1000       394
	 *     2000       436
	 *     3000       456
	 *     4000       477
	 *     5000       470
	 *     6000       386
	 */
	static inline double Torque(double rpm) { // In N.m
		return (
			2.666666679e-16   * rpm*rpm*rpm*rpm*rpm
			- 6.166666689e-12 * rpm*rpm*rpm*rpm
			+ 4.816666681e-8  * rpm*rpm*rpm
			- 1.698333338e-4  * rpm*rpm
			+ 2.985666675e-1  * rpm
			+ 222.9999995
		);
	}

	/*
	 *   Rev [rpm]  Power [kW]:
	 *     1000       41
	 *     2000       93
	 *     3000      144
	 *     4000      200
	 *     5000      248
	 *     6000      243
	 */
	static inline double Power(double rpm) { // In kW
		return (
			-1.083333335e-16  * rpm*rpm*rpm*rpm*rpm
			+ 8.333333354e-13 * rpm*rpm*rpm*rpm
			- 2.916666839e-10 * rpm*rpm*rpm
			- 9.83333328e-6   * rpm*rpm
			+ 7.440000005e-2  * rpm
			- 24.00000043
		);
	}

	static inline double Max_F_traction(double rpm, int gear) {
		if (rpm < 1000.0) {
			rpm = 1000.0;
		} else if (rpm > 6000.0) {
			rpm = 6000.0;
		}
		double max_torque = Torque(rpm);
		double gear_coeff = GearTrCoeff(gear);
		double force = max_torque * gear_coeff * Tr_eff / R_wheel;
		if (force > Max_Tract) {
			force = Max_Tract;
		}
		return force;
	}

	static inline double Rpm(double v, int gear) {
		double wheel_rot = (v * 1000.0 / 3600.0) / R_wheel; // in rad /sec
		double rpm = wheel_rot * GearTrCoeff(gear) * 60.0 / (2 * M_PI);
		return rpm;
	}


};

#endif // CAR_ENGINE_H_
