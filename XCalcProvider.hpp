#pragma once
#ifndef _H_XCALC_PROVIDER_HPP_
#define _H_XCALC_PROVIDER_HPP_

#include "XCalcDef.hpp"

namespace XCalc {
	template<class T, class TDataSet>
	class XDataSetProvider
	{
	public:
		typedef TDataSet DataSet;
		struct DataSetPtrLess
		{
			bool operator()(const DataSet* const& x, const DataSet* const& y) const
			{
				return *x < *y;
			}
		};
		typedef std::map<const DataSet*,std::shared_ptr<DataSet>,DataSetPtrLess> DataSets;
	protected:
		DataSets datasets_;
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
		template<typename F>
		inline void SafeHandle(F f)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			f(datasets_);
		}
	};

	template<class T, class TCalculator>
	class XCalculatorProvider
	{
	public:
		typedef TCalculator Calculator;
		typedef typename Calculator::CalcInfo CalcInfo;
		typedef typename CalcInfo::InputInfos InputInfos;
		typedef typename CalcInfo::BufferInfos BufferInfos;
		struct CalcInfoPtrLess
		{
			bool operator()(const CalcInfo* const& x, const CalcInfo* const& y) const
			{
				return *x < *y;
			}
		};
		typedef std::map<const CalcInfo*,std::shared_ptr<Calculator>,CalcInfoPtrLess> Calculators;
	protected:
		Calculators calculators_;
		std::mutex mutex_;
	public:
		XCalculatorProvider() {};
		virtual ~XCalculatorProvider() {};

		inline void AddCalculator(std::shared_ptr<Calculator>& calculator) 
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			calculators_[calculator->GetInfo()] = calculator;
		}
		inline std::shared_ptr<Calculator> RefCalculator(const CalcInfo& calcinfo)
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = calculators_.find(&calcinfo);
			if(it != calculators_.end()) {
				return it->second;
			}
			return nullptr; 
		}
		inline std::shared_ptr<Calculator> RefCalculator(const std::string& name
		, const InputInfos& inputs, const BufferInfos& buffers)
		{ 
			T* pT = static_cast<T*>(this);
			CalcInfo calcinfo = {name, inputs, buffers};
			return pT->RefCalculator(calcinfo);
		}
		template<typename F>
		inline void SafeHandle(F f)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			f(calculators_);
		}
	};

	template<class T, class TBufferSet>
	class XBufferSetProvider
	{
	public:
		typedef TBufferSet BufferSet;
		struct BufferSetPtrLess
		{
			bool operator()(const BufferSet* const& x, const BufferSet* const& y) const
			{
				return *x < *y;
			}
		};
		typedef std::map<const BufferSet*,std::shared_ptr<BufferSet>,BufferSetPtrLess> BufferSets;
	protected:
		BufferSets buffersets_;
		std::mutex mutex_;
	public:
		XBufferSetProvider() {};
		virtual ~XBufferSetProvider() {};

		inline void AddBufferSet(const std::shared_ptr<BufferSet>& bufferset) 
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			buffersets_[bufferset.get()] = bufferset;
		}
		inline void RemoveBufferSet(const std::shared_ptr<BufferSet>& bufferset)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			buffersets_.erase(bufferset.get());
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
		template<typename F>
		inline void SafeHandle(F f)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			f(buffersets_);
		}
	};

} // namespace XCalc

#endif //_H_XCALC_PROVIDER_HPP_
