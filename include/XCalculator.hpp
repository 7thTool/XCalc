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
	//计算信息
	CalcInfo info_; //计算器描述信息

  public:
	Calculator(DataProvider *dataset_provider, BufferSetProvider *bufferset_provider)
		: dataset_provider_(dataset_provider), bufferset_provider_(bufferset_provider)
	{
	}
	~Calculator()
	{
	}

	inline bool IsNeedReCalc(DataSet *dataset, BufferSet *bufferset)
	{
		if(dataset) {
			return dataset->GetFieldValueAsInt(idf_recalc) != 0;
		}
		return true;
	}

	inline void Clear(BufferSet *bufferset)
	{
		buffer_set->Clear();
	}
	
	inline void Calc(DataSet *dataset, BufferSet *bufferset, size_t calcfrom, size_t calcto)
	{
		
	}

	inline void Calc(DataSet *dataset, BufferSet *bufferset)
	{
		T *pT = static_cast<T *>(this);
		if (dataset)
		{
			bool recalc = IsNeedReCalc(dataset, bufferset);
			size_t buffer_count = dataset->GetBufferFieldCount();
			if(buffer_count > 0)
			{
				size_t buffer_size = dataset->GetBufferSize(0);
				if(recalc) 
				{
					pT->Clear(bufferset);
				}
				else if (buffer_size < bufferset->buffer_size)
				{
					bufferset->buffer_size = 0; //重新计算
				}
				else
				{
					bufferset->buffer_size = bufferset->buffer_size - 1; //重新计算最后一条数据
				}
				for(size_t i = 0; i < bufferset->buffers->size(); i++)
				{
					bufferset->buffers->at(i).resize(buffer_size);
				}
				if (buffer_size > bufferset->buffer_size)
				{
					pT->Calc(data, buffer, bufferset->buffer_size, buffer_size);
					bufferset->buffer_size = buffer_size;
				}
			}
			else
			{
				pT->Clear(bufferset);
				//pT->Calc(data, buffer, 0, 0);
			}
		}
		else
		{
			pT->Clear(bufferset);
		}
	}

}; // namespace XCalc
} // namespace XCalc

#endif //_H_XCALCULATOR_HPP_
