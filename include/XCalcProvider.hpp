#pragma once
#ifndef _H_XCALC_PROVIDER_HPP_
#define _H_XCALC_PROVIDER_HPP_

#include "XCalcDef.hpp"

namespace XCalc {
	template<class T, class DataSet, class DataInfo>
	class XDataProvider
	{
	public:
		typedef DataInfo::DataSet DataSet;
		XDataProvider() {};
		virtual ~XDataProvider() {};

		virtual std::shared_ptr<DataSet> RefCalcData(DataInfo& info) { return 0; }
	};

	template<class T, class Calculator, class CalcInfo, class DataInfo, class BufferInfo>
	class XCalculatorProvider
	{
	public:
		XCalculatorProvider() {};
		virtual ~XCalculatorProvider() {};

		std::shared_ptr<Calculator> RefCalculator(CalcInfo* calc_info, DataInfo* data_info);
	};
} // namespace XCalc

#endif //_H_XCALC_PROVIDER_HPP_
