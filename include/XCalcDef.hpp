#pragma once
#ifndef _H_XCALC_DEF_HPP_
#define _H_XCALC_DEF_HPP_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <bitset>
#include <boost/variant.hpp>
#include <xutility/XDataSet.hpp>

namespace XCalc {

	const size_t idf_recalc = idf_next;

typedef boost::variant<int64_t, double, std::string> variant;

//这里的参数会影响计算结果
struct InputInfo
{
	std::string name;
	variant value;
};

struct BufferInfo
{
	std::string name;
};

typedef std::vector<double> Buffer;
typedef std::vector<Buffer> VecBuffer;

template<class Calculator, class DataSet>
struct BufferSet : public XDataSet
{
public:
	std::vector<std::shared_ptr<Calculator>> refcalculators; //引用的计算器
	std::vector<std::shared_ptr<DataSet>> refdatasets; //引用的数据集
	std::shared_ptr<VecBuffer> buffers; //结果数据集
	int buffer_size; //有效buffer数目，即已经计算指标数目

	BufferSet(size_t count):buffers(std::make_shared<VecBuffer>(count)),counted(0)
	{

	}

	inline void Clear()
	{
		ClearCalculator();
		ClearDataSet();
		ClearBuffer();
	}

	inline void ClearCalculator() 
	{
		refcalculators.clear();
	}
	
	inline void ClearDataSet() 
	{
		refdatasets.clear();
	}
	
	inline void ClearBuffer()
	{
		for(size_t i = 0; i < buffers.size(); i++)
		{
			buffers[i].clear();
		}
		counted = 0;
	}
};

//context
struct CalculatorInfo
{
	std::string name;			   //指标名称
	std::vector<InputInfo> inputs;  //输入信息,除了显示信息外的可以用户修改的计算信息
	std::vector<BufferInfo> indexs; //index信息

	inline const std::string &name() { return name; }
	
	bool SetInputInfo(const InputInfo &info)
	{
		for (size_t i = 0; i < inputs.size())
		{
			if (inputs[index].name == info.name)
			{
				inputs[index] = info;
				return true;
				break;
			}
		}
		return false;
	}
	bool GetInputInfo(InputInfo &info)
	{
		for (size_t i = 0; i < inputs.size())
		{
			if (inputs[index].name == info.name)
			{
				info = inputs[index];
				return true;
				break;
			}
		}
		return false;
	}

	bool SetBufferInfo(size_t index, const IndexInfo &info)
	{
		if (index < indexs.size())
		{
			indexs[index] = info;
			return true;
		}
		return false;
	}
	bool GetBufferInfo(size_t index, IndexInfo &info)
	{
		if (index < indexs.size())
		{
			info = indexs[index];
			return true;
		}
		return false;
	}

};

//boost::get<std::string>(v)
template <typename variant, typename Target>
Target varto(const variant &arg, const Target &def = Target())
{
	try
	{
		Target o;
		class var_visitor : public boost::static_visitor<void>
		{
		  private:
			Target &o_;

		  public:
			var_visitor(Target &o) : o_(o) {}

			template <typename Type>
			void operator()(Type &v) const
			{
				std::stringstream ss;
				ss << v;
				ss >> o;
			}
		};
		boost::apply_visitor(var_visitor(o), arg);
		return o;
	}
	catch (std::exception &e)
	{
	}
	catch (...)
	{
	}
	return def;
}

} // namespace XCalc

#endif //_H_XCALC_DEF_HPP_
