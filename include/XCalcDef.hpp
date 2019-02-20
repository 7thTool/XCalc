#pragma once
#ifndef _H_XCALC_DEF_HPP_
#define _H_XCALC_DEF_HPP_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <bitset>
#include <boost/variant.hpp>

namespace XCalc {

typedef boost::variant<int64_t, double, std::string> variant;

//这里的参数会影响计算结果
struct InputInfo
{
	std::string name;
	variant value;
};

struct IndexInfo
{
	std::string name;
};

typedef std::vector<double> IndexVal;
typedef std::vector<IndexVal> BufferSet;

//context
template<class Calculator>
struct CalculatorInfo
{
	std::string name;			   //指标名称
	std::vector<InputVal> inputs;  //输入信息,除了显示信息外的可以用户修改的计算信息
	std::vector<IndexInfo> indexs; //index信息
	std::shared_ptr<Calculator> calcptr; //计算器
};

template<class DataSet>
struct CalculatorDataInfo
{
	std::string symbol; //当前标示
	std::shared_ptr<DataSet> dataptr;	//计算数据

	void Clear()
	{
		symbol.clear();
		dataptr = nullptr;
	}
};

template<class BufferSet>
struct CalculatorBufferInfo
{
	std::shared_ptr<BufferSet> buffptr; //结果数据
	int counted; //已经计算指标数目

	void Clear()
	{
		for(size_t i = 0; i < indexs.size(); i++)
		{
			indexs[i].clear();
		}
		counted = 0;
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
