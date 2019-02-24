#pragma once
#ifndef _H_XCALC_MGR_H_
#define _H_XCALC_MGR_H_

#include "XCalcDef.hpp"
#include "XCalcProvider.hpp"

namespace XCalc {

template <class T, class DataSetProvider, class CalculatorProvider, class BufferSetProvider>
class XCalcMgr 
: public CalculatorProvider
, public BufferSetProvider
{
public:
	typedef typename DataSetProvider::DataSet DataSet;
	typedef typename DataSetProvider::DataSets DataSets;
	typedef typename CalculatorProvider::Calculator Calculator;
	typedef typename CalculatorProvider::Calculators Calculators;
	typedef typename BufferSetProvider::BufferSet BufferSet;
	typedef typename BufferSetProvider::BufferSets BufferSets;
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
	inline BufferSetProvider* GetBufferSetProvider() { return this; }

	inline void Calc(const Calculator& calcinfo, std::shared_ptr<DataSet> & dataset, std::shared_ptr<BufferSet> & bufferset)
	{
		T* pT = static_cast<T*>(this);
		CalculatorProvider* calculator_provider = pT->GetCalculatorProvider();
		if(calculator_provider) {
			Calculator calculator = calculator_provider->RefCalculator(calcinfo);
			pT->Calc(calculator, dataset, bufferset);
		}
	}

	inline void Calc(std::shared_ptr<Calculator> calculator, std::shared_ptr<DataSet> & dataset, std::shared_ptr<BufferSet> & bufferset)
	{
		T* pT = static_cast<T*>(this);
		BufferSetProvider* bufferset_provider = pT->GetBufferSetProvider();
		if(bufferset_provider) {
			bufferset->Clear();
			bufferset->init(calculator, dataset);
			pT->Calc(dataset, bufferset);
			bufferset_provider->AddBufferSet(bufferset);
		}
	}

	inline void Calc(std::shared_ptr<DataSet> & dataset)
	{
		T* pT = static_cast<T*>(this);
		//遍历BufferSet更新BufferSet数据
		BufferSetProvider* bufferset_provider = pT->GetBufferSetProvider();
		if(bufferset_provider) {
			bufferset_provider->SafeHandle([&](BufferSets& buffersets){
				auto it = buffersets.begin();
				for(; it != buffersets.end(); ++it)
				{
					std::shared_ptr<BufferSet> & bufferset = it->second;
					if(bufferset->dataset == dataset) {
						pT->Calc(dataset, bufferset);
					}
				}
			}
			);
		}
	}

	inline void Calc(std::shared_ptr<DataSet> & dataset, std::shared_ptr<BufferSet> & bufferset)
	{
		T* pT = static_cast<T*>(this);
		bufferset->calculator->Calc(dataset, bufferset);
	}

};

} // namespace XCalc

#endif //_H_XCALC_MGR_H_
