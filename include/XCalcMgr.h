#ifndef _H_CALC_MGR_H_
#define _H_CALC_MGR_H_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <bitset>
#include <boost/variant.hpp>

namespace XCalc
{
	class XIndicatorSpi
	{
	public:
		//Global
		virtual void OnLog(XLogLevel level, const char* str) { }

		virtual void OnInit() { }
		virtual void OnTerm() { }

		//Data
		virtual void OnUpdateCalcData(const char* symbol) { }

		//Calc
		virtual void OnCalc(iHandle handle) { }
	};

	class XIndicatorDataProvider
	{
	public:
		//Symbol
		virtual int OnSymbolsFind(const char* key, unsigned int flags) { return 0; }
		virtual int OnSymbolsTotal(const char* market) { return 0; }
		virtual const char* OnSymbolSelect(int index, char* str, int len) { return str; }

		//Calc Data
		virtual voidptr OnRefCalcData(const char* symbol) { return 0; }
		virtual void OnReleaseCalcData(voidptr dataptr) { }
		virtual const char* OnGetSymbol(voidptr dataptr, char* str, int len) { return str; }
		virtual int OnGetCalcCount(voidptr dataptr) { return 0; }
		virtual double OnGetCalcValue(voidptr dataptr, int field, int offset) { return 0.; }
	};

	template<class T>
	class XIndicatorApi
	{
	public:
		///初始化
		///@remark 初始化运行环境,只有调用后,接口才开始工作
		//virtual void Init() = 0;
		//virtual void Term() = 0;

		/**
		* @brief 注册回调接口
		* @param[in] pSpi 派生自回调接口类的实例
		* @return 无
		*/
		virtual void RegisterSpi(XIndicatorSpi *pSpi) = 0;

		/**
		* @brief 注册行情回调接口
		* @param[in] pProvider 派生自行情回调接口类的实例
		* @return 无
		*/
		virtual void RegisterDataProvider(XIndicatorDataProvider *pProvider) = 0;

		/**
		* @brief 更新历史数据接口，
		*
		* 如果接口使用者自己提供行情数据，即RegisterQuoteProvider了，就需要每次历史数据变化时调用此接口
		* @return 无
		*/
		virtual void UpdateData(const char* symbol) = 0;

	protected:
		virtual ~XIndicatorApi() {};
	};

} // namespace XCalc

#endif //_H_CALC_MGR_H_
