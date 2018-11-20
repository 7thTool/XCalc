#ifndef _H_MARKETAPP_H_
#define _H_MARKETAPP_H_

#include "marketplatform.h"
#include "marketserver.h"
#include <XEvent.h>
#include <string>
#include <vector>
#include <memory>
#include <atomic>

namespace XTrade {

	class MarketApp;

	class MarketApp
		: public XBoost::XBoostApp
	{
	private:
		MarketPlatform platform_;
		MarketServer server_;
	public:
		MarketApp();

		bool start(int argc, char* argv[]);
		void stop();
	};

	extern MarketApp theApp;
}

#endif//_H_MARKETAPP_H_
