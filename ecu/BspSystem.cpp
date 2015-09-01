#include "BspSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

BspSystem BspSystem::instance;

BspSystem::BspSystem() : common::Singleton<BspSystem>(this) {
	fprintf(stderr, "BspSystem::BspSystem()\n");
}

BspSystem::~BspSystem() {
	fprintf(stderr, "~BspSystem::BspSystem()\n");
}

void BspSystem::dispatchCanMessage(CanDevId dev_id, CanMessage * can_msg) {
}
