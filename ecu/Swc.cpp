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
		int v = rte.Out.BrakeLights;
		v += 20;
		if (v > 255) {
			v = 255;
		}
		rte.Out.BrakeLights = v;
	} else {
		int v = rte.Out.BrakeLights;
		v -= 20;
		if (v < 0) {
			v = 0;
		}
		rte.Out.BrakeLights = v;
	}
}

void SwcBackLight::shutdown(Rte & rte) {
}
