#include "Swc.h"
#include "rte.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

SwcIntLight::SwcIntLight() {
	fprintf(stderr, "SwcIntLight::SwcIntLight\n");
}

void SwcIntLight::init(Rte & rte) {
	fprintf(stderr, "SwcIntLight::init\n");
}

void SwcIntLight::cyclic(Rte & rte) {
	//fprintf(stderr, "SwcIntLight::cyclic\n");
}

void SwcIntLight::shutdown(Rte & rte) {
	fprintf(stderr, "SwcIntLight::shutdown\n");
}
