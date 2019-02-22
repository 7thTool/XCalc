#pragma once
#ifndef _H_XCALC_MGR_H_
#define _H_XCALC_MGR_H_

#include "XCalcDef.hpp"
#include "XCalcProvider.hpp"

namespace XCalc {

template <class T, class DataSetProvider, class Calculator, class DataSet, class BufferSet>
class XCalcMgr 
: public XCalculatorProvider<T,Calculator>
, public XBufferSetProvider<T,BufferSet>
{
public:
	typedef XCalculatorProvider<T,Calculator> CalculatorProvider;
	typedef XBufferSetProvider<T,BufferSet> BufferSetProvider;
protected:
	DataSetProvider* dataset_provider_;
public:
	XCalcMgr():dataset_provider_(nullptr) {

	}

	~XCalcMgr() {

	}

	inline void SetDataSetProvider(DataSetProvider* dataset_provider) { dataset_provider_ = dataset_provider; }
	inline DataSetProvider* GetDataSetProvider() { return dataset_provider_; }
	inline CalculatorProvider* GetCalculatorProvider() { return this; }
	inline BufferSetProvider* BufferSetProvider() { return this; }
};

} // namespace XCalc

#endif //_H_XCALC_MGR_H_
