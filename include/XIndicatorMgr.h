#ifndef _H_CALC_H_
#define _H_CALC_H_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <bitset>
#include <boost/variant.hpp>

#include "XCalcMgr.h"

namespace XCalc
{
template <class T, class Info = IndicatorInfo, class DataInfo>
class XIndicatorMgr
{
  public:
	//
	UpdateData(DataInfo& info);

  	//Symbol
		int OnSymbolsFind(const char* key, unsigned int flags) { return 0; }
		int OnSymbolsTotal(const char* market) { return 0; }
		const char* OnSymbolSelect(int index, char* str, int len) { return str; }

		//Calc Data
		voidptr OnRefData(DataInfo& info) { return 0; }
		void OnReleaseData(voidptr dataptr) { }
		const char* OnGetSymbol(voidptr dataptr, char* str, int len) { return str; }
		int OnGetCalcCount(voidptr dataptr) { return 0; }
		double OnGetCalcValue(voidptr dataptr, int field, int offset) { return 0.; }

	XIndicator *OnRefIndicator(IndicatorInfo& info);
	long OnReleaseIndicator(XIndicator *handle); //返回引用计数
};

} // namespace XCalc

#endif //_H_MARKETAPP_H_
