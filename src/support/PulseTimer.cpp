#include "support/PulseTimer.h"
//#include <algorithm>

supp::PulseTimer::PulseTimer(Magnum::Timeline& timeline) : 
	timer_{ timeline },
	numIntervals_{},
	parties_{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {} }
{
}

int supp::PulseTimer::setInterval(float delta) {
	float currTime = timer_.previousFrameTime();
	float frame_dt = timer_.previousFrameDuration();
	int newId = numIntervals_++;
	intervals_[newId] = delta;
	lastTime_[newId] = currTime;
	nextInterval_[newId] = currTime + frame_dt + intervals_[newId];
	return -1;
}

int supp::PulseTimer::getInterval(float delta) const {
	for (int i = 0; i < numIntervals_; ++i) {
		if (delta == intervals_[i])
			return i;
	}
	return -1;
}

void supp::PulseTimer::pollAndNotify() {
	float currTime = timer_.previousFrameTime();
	for (int i = 0; i < numIntervals_; ++i) {
		if (currTime > nextInterval_[i]) {
			for (auto& participants : parties_[i])
				participants->react();
		}
	}
}

bool supp::PulseTimer::registerParty(supp::PulseTimer::Pulsee& observer, int id) {
	for (auto ptr : parties_[id]) {
		if (ptr == &observer) // is this actually safe??
			return true;
	}
	parties_[id].push_back(&observer);
	return true;
}