#pragma once
#ifndef _H_XCALCULATOR_HPP_
#define _H_XCALCULATOR_HPP_

#include "XCalcDef.hpp"
#include "XCalcProvider.hpp"

namespace XCalc {

template <class T
, class DataProvider, class HandleProvider
, class CalcInfo, class DataInfo, class BufferInfo>
class XCalculator
{
	typedef XCalculator<T,DataProvider,HandleProvider,Info,DataInfo,BufferInfo> This;
	typedef DataProvider::DataSet DataSet;
  protected:
	DataProvider* data_provider_;
	HandleProvider* handle_provider_;
	//正常计算指标都父对象指针都为空，因为可以被多个父指标共同引用，但是如果不为空，说明该计算指标属于特定指标，比如策略交易指标
	T *parent_;
	//计算信息
	CalcInfo info_; //计算器描述信息

	bool inited_;	 //是否初始化了

	std::shared_mutex mutex_; //读写锁,计算时独占写，其他共享读

	typedef std::set<std::string> SymbolSet;
	SymbolSet symbols_;

	bool IsRefSymbol(const std::string &symbol)
	{
		if (symbol.empty()) {
			return false;
		}
		if (symbols_.find(symbol) != symbols_.end()) {
			return true;
		}
		return false;
	}

	//引用的数据
	class RefCalcData
	{
	  public:
		RefCalcData()
		{
			dataptr = NULL;
			refcount = 0;
		}

		std::shared_ptr<DataSet> dataptr; //计算数据
		long refcount;   //引用计数
	};
	typedef std::map<DataInfo, RefCalcData> RefCalcDataMap;
	RefCalcDataMap refcalcdatas;

	bool IsRefData(const DataInfo &data)
	{
		auto it = refcalcdatas.begin();
		for (; it != refcalcdatas.end(); ++it)
		{
			if (it->first == data)
			{
				return true;
			}
		}
		return false;
	}
	std::shared_ptr<DataSet> RefData(const DataInfo &data)
	{
		RefCalcData &value = refcalcdatas[data];
		value.dataptr = data_provider_->RefCalcData(data);
		value.refcount += 1;
		return (voidptr)value.dataptr;
	}
	long ReleaseData(std::shared_ptr<DataSet> dataptr)
	{
		ASSERT(0);
		auto it = refcalcdatas.begin();
		for (; it != refcalcdatas.end(); ++it)
		{
			if (dataptr == it->second.dataptr)
			{
				break;
			}
		}
		if (it != refcalcdatas.end())
		{
			mgr_.ReleaseData(dataptr);
			it->second.refcount -= 1;
			if (it->second.refcount <= 0)
			{
				refcalcdatas.erase(it);
			}
			else
			{
				return it->second.refcount;
			}
		}
	}
	void ReleaseDataAll()
	{
		data_.dataptr = NULL;
		auto it = refcalcdatas.begin();
		for (; it != refcalcdatas.end(); ++it)
		{
			for (int i = 0; i < it->second.refcount; i++)
			{
				data_provider_->ReleaseData(it->second.dataptr);
			}
		}
		refcalcdatas.clear();
	}

	//引用的指标
	class RefCalculator
	{
	  public:
		RefCalculator()
		{
			//calculator = NULL;
			refcount = 0;
		}

		std::shared_ptr<XCalculator> calculator; //计算数据
		long refcount; //引用计数
	};
	typedef std::map<Info, RefCalculator> RefCalculatorMap;
	RefCalculatorMap refcalculators;

	std::shared_ptr<XCalculator> RefCalculator(const CalculatorInfo &calc_info, const DataInfo &data_info)
	{
		std::shared_ptr<XCalculator> calculator = handle_provider_->RefHandle(calc_info, data_info);
		if (calculator)
		{
			refcalculators[calc_info] += 1;
		}
		return calculator;
	}
	long ReleaseCalculator(XCalculator *handle)
	{
		ASSERT(0);
		//if (handle) {
		auto it = refCalculators.find(handle);
		if (it != refCalculators.end())
		{
			mgr_.ReleaseCalculator((XCalculator *)this, handle);
			it->second -= 1;
			if (it->second <= 0)
			{
				refCalculators.erase(it);
			}
			else
			{
				return it->second;
			}
		}
		//}
		return 0;
	}
	void ReleaseCalculatorAll()
	{
		auto it = refCalculators.begin();
		for (; it != refCalculators.end(); ++it)
		{
			for (int i = 0; i < it->second; i++)
			{
				mgr_.ReleaseCalculator((XCalculator *)this, it->first);
			}
		}
		refCalculators.clear();
	}

  public:
	Calculator(DataProvider* data_provider, HandleProvider* handle_provider) 
	: data_provider_(data_provider), handle_provider_(handle_provider), inited_(false), calc_num_(0) {

	}
	~Calculator() {

	}

	inline T *parent() { return parent_; }
	inline const std::string &name() { return info_.name; }

	bool Create(T *parent, const CalcInfo &info)
	{
		T *pT = static_cast<T *>(this);
		parent_ = parent;
		info_ = info;
		return pT->Init();
		return true;
	}

	void Destroy()
	{
		DeInit();
	}

	inline bool IsStrategy() { return info_.type == eStrategy; }
	inline bool IsInit() { return inited_; }
	inline bool IsPool() { return IsStrategy(); }

	bool SetInputInfo(const InputInfo &info)
	{
		std::unique_lock<std::shared_mutex> write_lock(mutex_);
		for (size_t i = 0; i < info_.inputs.size())
		{
			if (info_.inputs[index].name == info.name)
			{
				info_.inputs[index] = info;
				return true;
				break;
			}
		}
		return false;
	}
	bool GetInputInfo(InputInfo &info)
	{
		std::shared_lock<std::shared_mutex> read_lock(mutex_);
		for (size_t i = 0; i < info_.inputs.size())
		{
			if (info_.inputs[index].name == info.name)
			{
				info = info_.inputs[index];
				return true;
				break;
			}
		}
		return false;
	}

	bool SetIndexInfo(size_t index, const IndexInfo &info)
	{
		std::unique_lock<std::shared_mutex> write_lock(mutex_);
		if (index < info_.indexs.size())
		{
			info_.indexs[index] = info;
			return true;
		}
		return false;
	}
	bool GetIndexInfo(size_t index, IndexInfo &info)
	{
		std::shared_lock<std::shared_mutex> read_lock(mutex_);
		if (index < info_.indexs.size())
		{
			info = info_.indexs[index];
			return true;
		}
		return false;
	}

	void Calc(DataInfo *data, std::shared_ptr<BufferInfo>& buffer)
	{
		T *pT = static_cast<T *>(this);

		pT->ReleaseDataAll();
		pT->ReleaseCalculatorAll();

		if (data)
		{
			data_ = *data;
			bool bPool = pT->IsPool();
			if (bPool)
			{
				bool bDoFilter = false;
				if (!pT->IsCalc())
				{
					bDoFilter = true;
				}
				else
				{
					bDoFilter = data_.symbol.empty();
				}

				if (!bDoFilter)
				{
					pT->CleanPool();
					pT->Filter();
				}
				else
				{
					ReRefPool();
				}
			}

			bool bDoCalc = !data_.symbol.empty();
			if (bDoCalc)
			{
				//计算
				if (!data_.dataptr)
				{
					data_.dataptr = (voidptr)pT->RefData(data_);
				}
				int count = mgr_.GetDataCount(data_.dataptr);
				if (count < buffer_.counted)
				{
					buffer_.counted = 0; //重新计算
				}
				else
				{
					buffer_.counted = buffer_.counted - 1;
				}
				if (count > buffer_.counted)
				{
					pT->Calc();
					buffer_.counted = count;
				}
			}
		}
		else
		{
			ASSERT(0);
			data_.Clear();
			buffer_.Clear();
			rlt = pT->Calc();
		}
	}

	bool Init()
	{
		T *pT = static_cast<T *>(this);
		ASSERT(!inited_);
		inited_ = pT->DoInit();
		return inited_;
	}

	void Filter()
	{
		T *pT = static_cast<T *>(this);
		pT->DoPreFilter();
		pT->DoFilter();
		pT->DoAfterFilter();
	}

	void Calc()
	{
		T *pT = static_cast<T *>(this);
		pT->DoPreCalc();
		pT->DoCalc();
		pT->DoAfterCalc();
	}

	void DeInit()
	{
		T *pT = static_cast<T *>(this);
		pT->DoDeInit();
		pT->CleanPool();
		pT->ReleaseDataAll();
		pT->ReleaseCalculatorAll();
	}

	bool DoInit()
	{
		return false;
	}

	void DoInput()
	{
	}

	void DoPreFilter()
	{
	}

	void DoFilter()
	{
	}

	void DoAfterFilter()
	{
	}

	void DoPreCalc()
	{
	}

	void DoCalc()
	{
	}

	void DoAfterCalc()
	{
		calc_num_++;
	}

	void DoDeInit()
	{
	}
}; // namespace XCalc
} // namespace XCalc

#endif //_H_XCALCULATOR_HPP_
