#pragma once
#ifndef _H_XCALC_PROVIDER_HPP_
#define _H_XCALC_PROVIDER_HPP_

#include "XCalcDef.hpp"

namespace XCalc {
	template<class T, class DataSet>
	class XDataSetProvider
	{
	protected:
		struct DataSetPtrLess
		{
			bool operator()(const DataSet*& x, const DataSet*& y) const
			{
				return *x < *y;
			}
		};
		std::map<DataSet*,std::shared_ptr<DataSet>,DataSetPtrLess> datasets_;
		std::mutex mutex_;
	public:
		XDataSetProvider() {};
		virtual ~XDataSetProvider() {};

		inline void AddDataSet(std::shared_ptr<DataSet>& dataset) 
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			datasets_[dataset.get()] = dataset;
		}
		inline std::shared_ptr<DataSet> RefDataSet(const DataSet& datainfo) 
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = datasets_.find(&datainfo);
			if(it != datasets_.end()) {
				return it->second;
			}
			return nullptr; 
		}
	};

	template<class T, class Calculator>
	class XCalculatorProvider
	{
	protected:
		struct CalculatorPtrLess
		{
			bool operator()(const Calculator*& x, const Calculator*& y) const
			{
				return *x < *y;
			}
		};
		std::map<Calculator*,std::shared_ptr<Calculator>,CalculatorPtrLess> calculators_;
		std::mutex mutex_;
	public:
		XCalculatorProvider() {};
		virtual ~XCalculatorProvider() {};

		inline void AddCalculator(std::shared_ptr<Calculator>& calculator) 
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			calculators_[calculator.get()] = calculator;
		}
		inline std::shared_ptr<Calculator> RefCalculator(const Calculator& calcinfo) 
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = calculators_.find(&calcinfo);
			if(it != calculators_.end()) {
				return it->second;
			}
			return nullptr; 
		}
	};

	template<class T, class BufferSet>
	class XBufferSetProvider
	{
	protected:
		struct BufferSetPtrLess
		{
			bool operator()(const BufferSet*& x, const BufferSet*& y) const
			{
				return *x < *y;
			}
		};
		std::map<BufferSet*,std::shared_ptr<BufferSet>,BufferSetPtrLess> buffersets_;
		std::mutex mutex_;
	public:
		XBufferSetProvider() {};
		virtual ~XBufferSetProvider() {};

		inline void AddBufferSet(std::shared_ptr<BufferSet>& bufferset) 
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			buffersets_[bufferset.get()] = bufferset;
		}
		inline std::shared_ptr<BufferSet> RefBufferSet(const BufferSet& buffinfo) 
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = buffersets_.find(&buffinfo);
			if(it != buffersets_.end()) {
				return it->second;
			}
			return nullptr; 
		}
	};

} // namespace XCalc

#endif //_H_XCALC_PROVIDER_HPP_
