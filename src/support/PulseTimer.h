#ifndef PULSE_TIMER_H
#define PULSE_TIMER_H

#include <vector>
#include <Magnum/Timeline.h>

namespace supp {
	class PulseTimer {
		static constexpr int SIZE = 10;
	public:

		class Pulsee {
		public:
			//virtual ~Pulsee();
			virtual void react() = 0;
		};

		explicit PulseTimer(Magnum::Timeline& timer);
		int setInterval(float delta);
		int getInterval(float delta) const;
		bool registerParty(Pulsee& observer, int id);
		void pollAndNotify();
	private:

		Magnum::Timeline& timer_;
		float intervals_[SIZE];
		float timeElapsed_[SIZE];
		float lastTime_[SIZE];
		float nextInterval_[SIZE];

		int numIntervals_;
		std::vector<Pulsee*> parties_[SIZE];


	};
}

#endif // PULSE_TIMER_H
