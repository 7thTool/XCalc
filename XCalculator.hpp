#pragma once
#ifndef _H_XCALCULATOR_HPP_
#define _H_XCALCULATOR_HPP_

#include "XCalcDef.hpp"
#include "XCalcProvider.hpp"

namespace XCalc
{

template <class T, class TCalcInfo, class TDataSetProvider, class TBufferSetProvider>
class XCalculator : public TCalcInfo, public std::enable_shared_from_this<T>
{
	typedef XCalculator<T, TCalcInfo, TDataSetProvider, TBufferSetProvider> This;
	typedef TCalcInfo Base;
  public:
	typedef TCalcInfo CalcInfo;
	typedef TDataSetProvider DataSetProvider;
	typedef TBufferSetProvider BufferSetProvider;
	typedef typename DataSetProvider::DataSet DataSet;
	typedef typename BufferSetProvider::BufferSet BufferSet;

  public:
	using Base::Base;

	inline const CalcInfo* GetInfo() { return this; }
	inline DataSetProvider* GetDataSetProvider() { return nullptr; }
	inline BufferSetProvider* GetBufferSetProvider() { return nullptr; }

	inline bool CalcNeedRedo(std::shared_ptr<DataSet>& dataset, std::shared_ptr<BufferSet>& bufferset)
	{
		size_t buffer_count = dataset->GetFieldCount();
		for(size_t i = 0; i < buffer_count; i++)
		{
			size_t buffer_size = dataset->GetFieldSize(i);
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
			bufferset->Init(pT->shared_from_this(), dataset);
		} else {
			bool redo = pT->CalcNeedRedo(dataset, bufferset);
			if(redo) 
			{
				bufferset->ClearBuffer();
			}
			size_t buffer_count = dataset->GetFieldCount();
			for(size_t i = 0; i < buffer_count; i++)
			{
				size_t buffer_size = dataset->GetFieldSize(i);
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
		size_t buffer_count = dataset->GetFieldCount();
		for(size_t i = 0; i < buffer_count; i++)
		{
			size_t buffer_size = dataset->GetFieldSize(i);
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
