#ifndef LIGHT_SYSTEM_H_
#define LIGHT_SYSTEM_H_

#include "os.h"
#include "common/Singleton.h"
#include "common/Delegate.h"

struct IPwmOutput {
	virtual bool start() = 0;
	virtual bool stop() = 0;
	virtual bool setPeriod(uint16_t period) = 0;
	virtual bool setDuty(uint16_t duty) = 0;
};

struct DummyPwmOutput : public IPwmOutput  {
	virtual bool start() { return false; }
	virtual bool stop() { return false; }
	virtual bool setPeriod(uint16_t period) { return false; }
	virtual bool setDuty(uint16_t duty) { return false; }
};

class PwmOutput : public IPwmOutput {
public:
	PwmOutput(PwmDevId id);
	bool init();
	virtual bool start();
	virtual bool stop();
	virtual bool setPeriod(uint16_t period);
	virtual bool setDuty(uint16_t duty);
private:
	PwmDevId m_PwmDevId;
};

class LightSystem : public common::Singleton<LightSystem> {
public:
	LightSystem();
	~LightSystem();
	void init();
	void shutdown();

	enum LightId {
		InteriorLight,
		BrakeLight,
		BackwardsLight,
		LeftHazard,
		RightHazard
	};

	inline IPwmOutput & getPwmOutput(LightId light) {
		switch (light) {
			case InteriorLight:
				return m_IntLgtPwm;
			case BrakeLight:
				return m_BrkLgtPwm;
			case BackwardsLight:
				return m_BckLgtPwm;
			case LeftHazard:
				return m_LHzckLgtPwm;
			case RightHazard:
				return m_RHzckLgtPwm;
			default:
				return m_DummyPwm;
		}
	}

private:
	static LightSystem instance;

	PwmOutput m_IntLgtPwm;
	PwmOutput m_BrkLgtPwm;
	PwmOutput m_BckLgtPwm;
	PwmOutput m_LHzckLgtPwm;
	PwmOutput m_RHzckLgtPwm;
	DummyPwmOutput m_DummyPwm;
};

#endif // LIGHT_SYSTEM_H_
