#ifndef _H_CALC_H_
#define _H_CALC_H_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <bitset>
#include <boost/variant.hpp>

#include "XCalc.h"

namespace XCalc
{
template <class Manager, class T, class Info = IndicatorInfo, class DataInfo = IndicatorDataInfo, class BufferInfo = IndicatorBufferInfo>
class Indicator
{
	typedef Indicator<T> This;

  protected:
	Manager &mgr_;
	//正常计算指标都父对象指针都为空，因为可以被多个父指标共同引用，但是如果不为空，说明该计算指标属于特定指标，比如策略交易指标
	T *parent_;
	//指标信息(上下文和计算信息)
	Info info_;
	DataInfo data_;
	BufferInfo buffer_;

	bool inited_;	 //是否初始化了
	size_t calc_num_; //累计计算次数

	//策略/池指标计算相关信息
	typedef std::set<DataInfo> mapPool;
	mapPool pool_;

	std::shared_mutex mutex_; //读写锁,计算时独占写，其他共享读

	typedef std::set<std::string> mapSymbol;
	mapSymbol symbols_;

	bool IsRefSymbol(const std::string &symbol)
	{
		if (symbol.empty())
		{
			return false;
		}
		if (symbols_.find(symbol) != symbols_.end())
		{
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

		voidptr dataptr; //计算数据
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
	voidptr RefData(const DataInfo &data)
	{
		RefCalcData &value = refcalcdatas[data];
		value.dataptr = mgr_.RefData(data);
		value.refcount += 1;
		return (voidptr)value.dataptr;
	}
	long ReleaseData(voidptr dataptr)
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
				mgr_.ReleaseData(it->second.dataptr);
			}
		}
		refcalcdatas.clear();
	}

	//引用的指标
	class RefIndicator
	{
	  public:
		RefIndicator()
		{
			//indicator = NULL;
			refcount = 0;
		}

		std::shared_ptr<XIndicator> indicator; //计算数据
		long refcount;						   //引用计数
	};
	typedef std::map<Info, RefIndicator> RefIndicatorMap;
	RefIndicatorMap refindicators;

	XIndicator *RefIndicator(const IndicatorInfo &info, const DataInfo &data)
	{
		XIndicator *pIndicator = mgr_.RefIndicator(info, data);
		if (pIndicator)
		{
			refindicators[pIndicator] += 1;
		}
		return pIndicator;
	}
	long ReleaseIndicator(XIndicator *handle)
	{
		ASSERT(0);
		//if (handle) {
		auto it = refindicators.find(handle);
		if (it != refindicators.end())
		{
			mgr_.ReleaseIndicator((XIndicator *)this, handle);
			it->second -= 1;
			if (it->second <= 0)
			{
				refindicators.erase(it);
			}
			else
			{
				return it->second;
			}
		}
		//}
		return 0;
	}
	void ReleaseIndicatorAll()
	{
		auto it = refindicators.begin();
		for (; it != refindicators.end(); ++it)
		{
			for (int i = 0; i < it->second; i++)
			{
				mgr_.ReleaseIndicator((XIndicator *)this, it->first);
			}
		}
		refindicators.clear();
	}

  public:
	Indicator(Manager &mgr) : mgr_(mgr), inited_(false), calc_num_(0) {}
	~Indicator() {}

	inline T *parent() { return parent_; }
	inline const std::string &name() { return info_.name; }

	bool Create(T *parent, const std::string &name)
	{
		T *pT = static_cast<T *>(this);
		parent_ = parent;
		info_.name = name;
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
	inline bool IsCalc() { return calc_num_ != 0; }
	inline size_t GetCalcNum() { return calc_num_; }

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

	void SetCalcData(const DataInfo &data)
	{
		std::unique_lock<std::shared_mutex> write_lock(mutex_);
		data_ = data;
	}
	DataInfo GetCalcData()
	{
		std::shared_lock<std::shared_mutex> read_lock(mutex_);
		return data_;
	}

	long GetCalcCount()
	{
		std::shared_lock<std::shared_mutex> read_lock(mutex_);
		return buffer_.counted;
	}
	double GetIndexValue(size_t index, size_t offset)
	{
		std::shared_lock<std::shared_mutex> read_lock(mutex_);
		return buffer_.indexs[index][offset];
	}

	bool UpdateData(const std::string &symbol)
	{
		std::unique_lock<std::shared_mutex> write_lock(mutex_);
		T *pT = static_cast<T *>(this);
		if (pT->IsRefSymbol(symbol))
		{
			DataInfo data = data_;
			data.symbol = symbol;
			pT->Calc(&data);
			return true;
		}
		return false;
	}

	void ClearData()
	{
		std::unique_lock<std::shared_mutex> write_lock(mutex_);
		T *pT = static_cast<T *>(this);
		pT->Calc(nullptr);
	}

  protected:
	void Calc(DataInfo *data)
	{
		T *pT = static_cast<T *>(this);

		pT->ReleaseDataAll();
		pT->ReleaseIndicatorAll();

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
		pT->ReleaseIndicatorAll();
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

	void AddPool(const DataInfo &data)
	{
		T *pT = static_cast<T *>(this);
		ASSERT(pT->IsPool());
		if (pool_.find(data) == pool_.end())
		{
			data.dataptr = pT->RefData(data);
			pool_.insert(data);
		}
	}

	void RemovePool(const DataInfo &data)
	{
		T *pT = static_cast<T *>(this);
		ASSERT(pT->IsPool());
		auto it = pool_.find(data);
		if (it != pool_.end())
		{
			pT->ReleaseData(*it);
			pool_.erase(it);
		}
	}

	size_t PoolsTotal()
	{
		return pool_.size();
	}

	bool IsRefPool(const DataInfo &data)
	{
		if (pool_.find(data) != pool_.end())
		{
			return true;
		}
		return false;
	}

	void ReRefPool()
	{
		T *pT = static_cast<T *>(this);
		ASSERT(pT->IsPool());
		//重新引用所有Pool对象
		auto it = pool_.begin();
		for (; it != pool_.end(); ++it)
		{
			it->dataptr = pT->RefData(*it);
		}
	}

	void CleanPool()
	{
		/*ReleaseDataAll已经释放了数据对象了
		auto it = pool_.begin();
		for (; it!=pool_.end(); ++it)
		{
			mgr_.ReleaseData(*it);
		}*/
		pool_.clear();
	}
}; // namespace XCalc
} // namespace XCalc

#endif //_H_CALC_H_
