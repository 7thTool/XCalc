#ifndef _H_CALC_H_
#define _H_CALC_H_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <bitset>
#include <boost/variant.hpp>

#include "XCalcMgr.h"

namespace XCalc
{

//这里的参数会影响计算结果
struct InputVal
{
	std::string name;
	boost::variant value;
	uint32_t number:1;
	uint32_t digits:4;
	uint32_t visible:1;
	uint32_t reserved:26;
};

struct IndexInfo
{
	std::string name;
	uint32_t type:4;			//INDICATOR_INDEX_TYPE
	uint32_t shift:8;			//指标偏移
	uint32_t begin:8;			//指标开始
	uint32_t draw:4;			//INDICATOR_DRAW_TYPE
	uint32_t line:4;			//INDICATOR_LINE_TYPE
	uint32_t next:4;			//画线关联指标线
	uint32_t :0;
	uint32_t digits:4;			//指标精度,如果指定了精度就不使用商品的digits
	uint32_t width:4;			//线宽
	uint32_t color;				//指标颜色
	//double min_value;			//指标最大值
	//double max_value;			//指标最小值
	//double base_value;		//基准值
	//int level_count:8;		//水平线数目
	//int level_style:24;		//水平线风格
	//uint32_t level_color;		//水平线颜色
};

struct tagIndexVal
{
	union
	{
		double dvalue;
		char value[8];
	};
};

//context
struct IndicatorInfo
{
	std::string name; //指标名称
	std::vector<InputVal> inputs;					//输入信息,除了显示信息外的可以用户修改的计算信息
	std::vector<IndexInfo indexs;					//index信息
};

struct IndicatorCalcInfo
{
	std::string symbol;								//当前标示
	voidptr dataptr;								//计算对象数据
};

struct IndicatorBufferInfo
{
	int counted;										//已经计算指标数目
	std::vector<double*> indexs;						//指标线数据指针，最多MAX_INDICATOR_INDEX条指标
}INDICATORCALCINFO,*PINDICATORCALCINFO;

class InputHelper
{
  public:
	INPUTINFO *m_input;

  public:
	InputHelper::InputHelper(const char *name, INPUTINFO *input) : m_input(input)
	{
		IndicatorHelper helper(name, m_input);
	}

	InputHelper::InputHelper(INPUTINFO *input) : m_input(input)
	{
		//
	}

	long InputHelper::SetInput(const char *name, const char *value, bool visible, bool addorupdate)
	{
		ASSERT(m_input);
		int i;
		for (i = 0; i < m_input->count; i++)
		{
			if (stricmp(name, m_input->input[i].name) == 0)
			{
				break;
			}
		}
		if (i < m_input->count || addorupdate)
		{
			if (i >= m_input->count && addorupdate)
			{
				m_input->count += 1;
				strncpy(m_input->input[i].name, name, MAX_INDICATOR_NAME);
			}
			strcpy(m_input->input[i].value, value);
			m_input->input[i].number = false;
			m_input->input[i].digits = 0;
			m_input->input[i].visible = visible;
			return 1;
		}
		return 0;
	}

	long InputHelper::SetInput(const char *name, double value, bool visible, bool addorupdate)
	{
		ASSERT(m_input);
		int i;
		for (i = 0; i < m_input->count; i++)
		{
			if (stricmp(name, m_input->input[i].name) == 0)
			{
				break;
			}
		}
		if (i < m_input->count || addorupdate)
		{
			if (i >= m_input->count && addorupdate)
			{
				m_input->count += 1;
				strncpy(m_input->input[i].name, name, MAX_INDICATOR_NAME);
			}
			int digits = GetDecimalDigits(value);
			char szFormat[MAX_INDICATOR_STRING];
			sprintf(szFormat, "%%.%df", digits);
			sprintf(m_input->input[i].value, szFormat, value);
			m_input->input[i].number = true;
			m_input->input[i].digits = digits;
			m_input->input[i].visible = visible;
			return 1;
		}
	}

	const char *InputHelper::GetInput(const char *name, bool *visible) const
	{
		int i, j;
		for (i = 0; i < m_input->count; i++)
		{
			if (stricmp(name, m_input->input[i].name) == 0)
			{
				if (visible)
				{
					*visible = m_input->input[i].visible;
				}
				return m_input->input[i].value;
			}
		}
		return _tcsnull();
	}

	bool InputHelper::IsLess(const INPUTINFO *input) const
	{
		ASSERT(input);
		int i, j, cmp = 0;
		ASSERT(m_input->count == input->count);
		{
			for (i = 0; i < m_input->count; i++)
			{
				for (j = 0; j < input->count; j++)
				{
					if (stricmp(m_input->input[i].name, input->input[j].name) == 0)
					{
						break;
					}
				}
				if (j >= input->count)
				{
					ASSERT(0);
					continue;
				}
				cmp = stricmp(m_input->input[i].value, input->input[j].value);
				if (cmp != 0)
				{
#ifdef _DEBUG
					double dvalue1 = strto<double>(m_input->input[i].value);
					double dvalue2 = strto<double>(input->input[j].value);
					ASSERT(!IsZeroFloat(dvalue1 - dvalue2));
#endif //
					break;
				}
			}
		}
		return cmp < 0;
	}

	bool InputHelper::IsEqual(const INPUTINFO *input) const
	{
		ASSERT(input);
		int i, j;
		ASSERT(m_input->count == input->count);
		{
			for (i = 0; i < m_input->count; i++)
			{
				for (j = 0; j < input->count; j++)
				{
					if (stricmp(m_input->input[i].name, input->input[j].name) == 0)
					{
						break;
					}
				}
				if (j >= input->count)
				{
					ASSERT(0);
					continue;
				}
				if (stricmp(m_input->input[i].value, input->input[j].value))
				{
#ifdef _DEBUG
					double dvalue1 = strto<double>(m_input->input[i].value);
					double dvalue2 = strto<double>(input->input[j].value);
					ASSERT(!IsZeroFloat(dvalue1 - dvalue2));
#endif //
					break;
				}
			}
			if (i >= m_input->count)
			{
				return true;
			}
		}
		return false;
	}
};

} // namespace XCalc

#endif //_H_MARKETAPP_H_
