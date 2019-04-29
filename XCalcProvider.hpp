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
		using Factory = std::function<std::shared_ptr<Calculator>(const std::string& name)>;
		typedef std::map<std::string,Factory> Factorys;
		typedef typename Calculator::CalcInfo CalcInfo;
		typedef typename CalcInfo::InputSet InputSet;
		typedef typename CalcInfo::BufferInfoSets BufferInfoSets;
		struct CalcInfoPtrLess
		{
			bool operator()(const CalcInfo* const& x, const CalcInfo* const& y) const
			{
				return *x < *y;
			}
		};
		typedef std::map<const CalcInfo*,std::shared_ptr<Calculator>,CalcInfoPtrLess> Calculators;
	protected:
		Factorys factorys_;
		Calculators calculators_;
		std::mutex mutex_;
	public:
		XCalculatorProvider() {};
		virtual ~XCalculatorProvider() {};

		inline void AddFactory(const std::string& name, const Factory& f) 
		{
			factorys_[name] = f;
		}
		inline std::shared_ptr<Calculator> RefCalculator(const CalcInfo& calcinfo)
		{ 
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = calculators_.find(&calcinfo);
			if(it != calculators_.end()) {
				return it->second;
			}
			auto it_f = factorys_.find(calcinfo.name);
			if(it_f != factorys_.end()) {
				auto ptr = it_f->second(calcinfo.name);
				if(ptr) {
					calculators_[ptr.get()] = ptr;
				}
				return ptr;
			}
			return nullptr;
		}
		template<typename F>
		inline void SafeHandle(F f)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			f(calculators_);
		}
	};

	template<class T, class TCalculator, class TDataSet, class TBufferSet>
	class XBufferSetProvider
	{
	public:
		typedef TCalculator Calculator;
		typedef TDataSet DataSet;
		typedef TBufferSet BufferSet;
		struct BufferSetPtrLess
		{
			bool operator()(const BufferSet* const& x, const BufferSet* const& y) const
			{
				return *x < *y;
			}
		};
		struct BufferSetEx : public BufferSet
		{
			using BufferSet::BufferSet;
			int ref = 0;
			int depth = 0;
		};
		typedef std::map<const BufferSet*,BufferSetEx*,BufferSetPtrLess> BufferSets;
		typedef std::multimap<const BufferSetEx*,const BufferSetEx*> BufferSetCallers;
	protected:
		BufferSets buffersets_;
		BufferSetCallers bufferset_callers_; //记录调用者信息
		int max_depth_ = 0; //最大调用深度
		std::shared_mutex mutex_;
	public:
		XBufferSetProvider() {}
		virtual ~XBufferSetProvider() {}

		inline void RemoveBufferSet() 
		{
			std::unique_lock<std::shared_mutex> lock(mutex_);
			bufferset_callers_.clear();
			buffersets_.clear();
		}
		inline BufferSet* RefBufferSet(const BufferSet* caller, const std::shared_ptr<Calculator> calculator, const std::shared_ptr<DataSet>& calcdata)
		{ 
			std::unique_lock<std::shared_mutex> lock(mutex_);
			BufferSet buffinfo = {calculator,calcdata};
			auto it = buffersets_.find(&buffinfo);
			if(it != buffersets_.end()) {
				it->second->ref++;
				AddCaller(dynamic_cast<const BufferSetEx*>(caller), it->second);
				return it->second;
			} else {
				auto bufferset = new BufferSetEx(calculator,calcdata);
				if(bufferset) {
					calculator->Calc(calcdata, bufferset);
					bufferset->ref = 1;
					buffersets_[bufferset] = bufferset;
					AddCaller(dynamic_cast<const BufferSetEx*>(caller), bufferset);
				}
			}
			return nullptr; 
		}
		inline void ReleaseBufferSet(const BufferSet* caller, BufferSet* dataset) 
		{
			BufferSetEx* bufferset = dynamic_cast<BufferSetEx*>(dataset);
			if (!bufferset)
			{
				return;
			}
			RemoveCaller(dynamic_cast<const BufferSetEx*>(caller), bufferset);
			std::unique_lock<std::shared_mutex> lock(mutex_);
			int ref = --bufferset->ref;
			auto it = buffersets_.find(dataset);
			if (it != buffersets_.end())
			{
				if (ref == 0)
				{
					buffersets_.erase(it);
				}
			}
			if (ref == 0)
			{
				delete bufferset;
			}
		}
		inline size_t GetBufferSetCount() { return buffersets_.size(); }
		template<typename F>
		inline void SafeHandle(F f)
		{
			std::shared_lock<std::shared_mutex> lock(mutex_);
			f(buffersets_, max_depth_);
		}
	protected:
		inline void AddCaller(const BufferSetEx* caller, BufferSetEx* bufferset)
		{
			if(!caller) {
				return;
			}
			bufferset_callers_.insert(std::make_pair(bufferset,caller));
			int depth = 1;
			if(bufferset->depth < depth) {
				bufferset->depth = depth;
			} else {
				depth = bufferset->depth;
			}
			if (caller->depth >= depth) {
				depth = caller->depth + 1;
				bufferset->depth = depth;
			}
			if(depth > max_depth_) {
				max_depth_ = depth;
			}
		}
		inline void RemoveCaller(const BufferSetEx* caller, BufferSetEx* bufferset)
		{
			if(!caller) {
				return;
			}
			auto pr = bufferset_callers_.equal_range(bufferset);
			int depth = 0;
			for(auto it = pr.first; it != pr.second; ++it)
			{
				if(it->second == caller)
				{
					it = bufferset_callers_.erase(it);
					continue;
				}
				if (it->second->depth >= depth) {
					depth = it->second->depth + 1;
				}
			}
			bufferset->depth = depth;
		}
	};

} // namespace XCalc

#endif //_H_XCALC_PROVIDER_HPP_
