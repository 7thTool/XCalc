#pragma once
#ifndef _H_XCALCULATOR_HPP_
#define _H_XCALCULATOR_HPP_

#include "XCalcDef.hpp"
#include "XCalcProvider.hpp"

namespace XCalc
{

template <class T, class CalcInfo, class DataSetProvider, class BufferSetProvider>
class XCalculator : public CalcInfo
{
	typedef XCalculator<T, CalcInfo, DataSetProvider, BufferSetProvider> This;
  public:
	typedef DataSetProvider::DataSet DataSet;
	typedef BufferSetProvider::BufferSet BufferSet;

  protected:
	DataSetProvider *dataset_provider_;
	BufferSetProvider *bufferset_provider_;

  public:
	Calculator(DataProvider *dataset_provider, BufferSetProvider *bufferset_provider)
		: dataset_provider_(dataset_provider), bufferset_provider_(bufferset_provider)
	{
	}
	~Calculator()
	{
	}

	inline bool CalcNeedRedo(std::shared_ptr<DataSet>& dataset, std::shared_ptr<BufferSet>& bufferset)
	{
		size_t buffer_count = dataset->GetBufferCount();
		for(size_t i = 0; i < buffer_count; i++)
		{
			size_t buffer_size = dataset->GetBufferSize(i);
			if (buffer_size < bufferset->buffer_size)
			{
				return true;
			}
		}
		return false;
	}

	inline void CalcClear(std::shared_ptr<DataSet>& dataset, std::shared_ptr<BufferSet>& bufferset)
	{
		bufferset->ClearBuffer();
	}

	inline void CalcInit(std::shared_ptr<DataSet>& dataset, std::shared_ptr<BufferSet>& bufferset)
	{
		T *pT = static_cast<T *>(this);
		if(bufferset->IsEmpty()) {
			bufferset->Init(this, dataset);
		} else {
			bool redo = pT->CalcNeedRedo(dataset, bufferset);
			if(redo) 
			{
				bufferset->ClearBuffer(bufferset);
			}
			size_t buffer_count = dataset->GetBufferCount();
			for(size_t i = 0; i < buffer_count; i++)
			{
				size_t buffer_size = dataset->GetBufferSize(i);
				if(bufferset->buffer_size > 0)
				{
					bufferset->buffer_size = bufferset->buffer_size - 1; //重新计算最后一条数据
				}
				bufferset->buffers[i].resize(buffer_size);
			}
		}
	}
	
	inline void CalcDo(std::shared_ptr<DataSet>& dataset, std::shared_ptr<BufferSet>& bufferset)
	{
		
	}

	inline void CalcDone(std::shared_ptr<DataSet>& dataset, std::shared_ptr<BufferSet>& bufferset)
	{
		size_t buffer_count = dataset->GetBufferCount();
		for(size_t i = 0; i < buffer_count; i++)
		{
			size_t buffer_size = dataset->GetBufferSize(i);
			bufferset->buffer_size = buffer_size;
			return;
		}
	}

	inline void Calc(std::shared_ptr<DataSet> dataset, std::shared_ptr<BufferSet> bufferset)
	{
		T *pT = static_cast<T *>(this);
		if (dataset)
		{
			pT->CalcInit(dataset, bufferset);
			pT->CalcDo(dataset, bufferset);
			pT->CalcDone(dataset, bufferset);
		}
		else
		{
			pT->CalcClear(dataset, bufferset);
		}
	}
}; // namespace XCalculator

} // namespace XCalc

#endif //_H_XCALCULATOR_HPP_
