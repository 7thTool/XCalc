#include <XEvent.h>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include "sourceplatform.h"
#include "sourceserver.h"
#include <Base/XBase.h>

namespace XTrade {
	//实现源转码传输，数据不落地
	class App
		: public XBoost::XBoostApp
	{
	private:
		SourcePlatform platform_;
		SourceServer server_;
	public:
		App() {}

		bool start(int argc, char* argv[])
		{
			XBoostApp::init(argv[argc - 1], XBoost::XML_FLAG_JSON_FILE);
			PRINTFL("source=%s work_path=%s data_path_=%s"
				, name_.c_str(), work_path_.string().c_str(), data_path_.string().c_str());

			int i;
			uint32_t Date = 20181109;
			Date = PrevWorkDay(Date);
			Date = NextWorkDay(Date);

			EXCHANGEINFO exchange = { 0 };
			PRODUCTINFO product = { 0 };
			COMMODITYINFO commodity = { 0 };
			product.PeriodsCount = 4;
			product.Periods[0][0] = 21 * 60 + 0; product.Periods[0][1] = 2 * 60 + 30;
			product.Periods[1][0] = 9 * 60 + 0; product.Periods[1][1] = 10 * 60 + 15;
			product.Periods[2][0] = 10 * 60 + 30; product.Periods[2][1] = 11 * 60 + 30;
			product.Periods[3][0] = 13 * 60 + 0; product.Periods[3][1] = 15 * 60 + 0;
			commodity.PeriodsCount = 2;
			commodity.Periods[0][0] = 9 * 60 + 15; commodity.Periods[0][1] = 11 * 60 + 30;
			commodity.Periods[1][0] = 13 * 60 + 0; commodity.Periods[1][1] = 15 * 60 + 15;
			UpdateProductPeriod(&product, commodity.Periods, commodity.PeriodsCount);
			UpdateExchangePeriod(&exchange, commodity.Periods, commodity.PeriodsCount);
			UpdateExchangePeriod(&exchange, product.Periods, product.PeriodsCount);
			UpdateProductPeriod(&product, commodity.Periods, commodity.PeriodsCount);
			UpdateExchangePeriod(&exchange, commodity.Periods, commodity.PeriodsCount);
			UpdateExchangePeriod(&exchange, product.Periods, product.PeriodsCount);
			for (i = 0; i < exchange.PeriodsCount; i++)
			{
				printf("%03d %06d-%06d\n", i, MINUTE_TO_HHMMSS(exchange.Periods[i][0]), MINUTE_TO_HHMMSS(exchange.Periods[i][1]));
			}
			printf("%03d\n", GetTimePointByHHMMSS(80000, exchange.Periods, exchange.PeriodsCount));
			printf("%03d\n", GetTimePointByHHMMSS(120000, exchange.Periods, exchange.PeriodsCount));
			printf("%03d\n", GetTimePointByHHMMSS(180000, exchange.Periods, exchange.PeriodsCount));
			uint16_t MaxTimePoint = GetTimePointCount(exchange.Periods, exchange.PeriodsCount);
			for (i=0; i < MaxTimePoint; i++)
			{
				uint32_t HHMMSS = GetHHMMSSByTimePoint(i, MaxTimePoint, exchange.Periods, exchange.PeriodsCount);
				uint16_t POS = GetTimePointByHHMMSS(HHMMSS, exchange.Periods, exchange.PeriodsCount);
				printf("%03d %06d\n", POS, HHMMSS);
			}

			boost::property_tree::ptree & app_cfg = cfg();
			if (platform_.start(app_cfg)) {
				auto opt_server = app_cfg.get_child_optional("server");
				ASSERT(opt_server);
				return server_.start(opt_server.get());
			}
			return false;
		}

		void stop()
		{
			server_.stop();
			platform_.stop();
			term();
		}

		void run()
		{
			platform_.run();
		}
	};
}

int main(int argc, char* argv[])
{
	XTrade::App app;
	app.start(argc, argv);

	boost::asio::io_service io_service;
	//boost::asio::io_service::work work(io_service);
	std::shared_ptr<std::thread> thread_ptr = std::make_shared<std::thread>(
		//boost::bind(&boost::asio::io_service::run, &io_service)
		[&]() {
		std::cout << "start" << std::endl;
		XBoost::EvtDispatcher dispatcher(io_service);
		io_service.run();
		std::cout << "stop" << std::endl;
	}
	);
	XBoost::EvtThread evt_thread;
	evt_thread.start();
	app.run();
	evt_thread.stop();
	io_service.stop();
	thread_ptr->join();
	io_service.reset();
	thread_ptr.reset();

	app.stop();
	return 0;
}

