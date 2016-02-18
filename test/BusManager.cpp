#include "os.h"
#include "BusManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

BusManager BusManager::sys; // Make sure that the object instance is created

BusManager::BusManager() : common::Singleton<BusManager>(this) {
	fprintf(stderr, "BusManager::BusManager()\n");
}

BusManager::~BusManager() {
	fprintf(stderr, "~BusManager::BusManager()\n");
}

void BusManager::init() {
	fprintf(stderr, "BusManager::init()\n");
}

void BusManager::shutdown() {
	fprintf(stderr, "BusManager::shutdown()\n");
}
