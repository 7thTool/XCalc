#pragma once
#ifndef _H_XCALC_PROVIDER_HPP_
#define _H_XCALC_PROVIDER_HPP_

#include "XCalcDef.hpp"

namespace XCalc {
	template<class T, class DataSet>
	class XDataSetProvider
	{
	public:
		XDataSetProvider() {};
		virtual ~XDataSetProvider() {};

		virtual bool RefDataSet(std::shared_ptr<DataSet>& dataset) { return false; }
	};

	template<class T, class Calculator>
	class XCalculatorProvider
	{
	public:
		XCalculatorProvider() {};
		virtual ~XCalculatorProvider() {};

		virtual bool RefCalculator(std::shared_ptr<Calculator>& calculator) { return false; }
	};

	template<class T, class BufferSet>
	class XBufferSetProvider
	{
	public:
		XBufferSetProvider() {};
		virtual ~XBufferSetProvider() {};

		virtual bool RefBufferSet(std::shared_ptr<BufferSet>& bufferset) { return false; }
	};

} // namespace XCalc

#endif //_H_XCALC_PROVIDER_HPP_
