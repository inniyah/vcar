#include "Swc.h"
#include "rte.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

SwcBackLight::SwcBackLight() {
}

void SwcBackLight::init(Rte & rte) {
	fprintf(stderr, "SwcBackLight::init\n");
}

void SwcBackLight::cyclic(Rte & rte) {
	fprintf(stderr, "SwcBackLight::cyclic\n");
	if (rte.In.Brake > 0) {
		rte.Out.BrakeLights = 255;
	} else {
		rte.Out.BrakeLights = 0;
	}
}

void SwcBackLight::shutdown(Rte & rte) {
}
