#pragma once
#ifndef _H_XCALC_MGR_H_
#define _H_XCALC_MGR_H_

#include "XCalcDef.hpp"
#include "XCalcProvider.hpp"

namespace XCalc {

template <class T, class Info = IndicatorInfo, class DataInfo = IndicatorDataInfo, class BufferInfo = IndicatorBufferInfo>
class XCalcMgr : public XHandleProvider
{
protected:
	XDataProvider* data_provider_;
public:
	XCalcMgr():data_provider_(nullptr) {

	}

	inline void SetDataProvider(XDataProvider* data_provider) { data_provider_ = data_provider; }
	inline XDataProvider* GetDataProvider() { return data_provider_; }
	
	//
	UpdateData(DataInfo& info) {};
};

} // namespace XCalc

#endif //_H_XCALC_MGR_H_
