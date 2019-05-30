#pragma once
#ifndef _H_XCALC_DEF_HPP_
#define _H_XCALC_DEF_HPP_

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <bitset>
#include <boost/variant.hpp>

namespace XCalc {

//boost::get<std::string>(v)
// template <typename variant, typename Target>
// Target varto(const variant &arg, const Target &def = Target())
// {
// 	try
// 	{
// 		Target o;
// 		class var_visitor : public boost::static_visitor<void>
// 		{
// 		  private:
// 			Target &o_;

// 		  public:
// 			var_visitor(Target &o) : o_(o) {}

// 			template <typename Type>
// 			void operator()(Type &v) const
// 			{
// 				std::stringstream ss;
// 				ss << v;
// 				ss >> o;
// 			}
// 		};
// 		boost::apply_visitor(var_visitor(o), arg);
// 		return o;
// 	}
// 	catch (std::exception &e)
// 	{
// 	}
// 	catch (...)
// 	{
// 	}
// 	return def;
// }
typedef boost::variant<int64_t, double, std::string> variant;

//这里的参数会影响计算结果
struct InputInfo
{
	std::string name;
	variant value;

    bool operator < (const InputInfo & r) const
    {
		if (name < r.name)
            return true;
        else if(r.name < name)
			return false;
		else
            return (value < r.value);
    };

    bool operator == (const InputInfo &r) const
    {
        return (name == r.name && value == r.value);
    }
};

template<class BaseSet>
struct BufferInfo : public BaseSet
{
	std::string name;

	BufferInfo(const char* name):name(name){}

    bool operator < (const BufferInfo & r) const
    {
		return name < r.name;
    };

    bool operator == (const BufferInfo &r) const
    {
        return (name == r.name);
    }
};

template<class Calculator, class DataSet, class BaseSet>
struct BufferSet : public BaseSet
{
public:
	typedef std::vector<std::shared_ptr<DataSet>> RefDataSets;
	typedef std::vector<std::shared_ptr<BufferSet>> RefBufferSets;
	std::shared_ptr<Calculator> calculator; //计算器
	std::shared_ptr<DataSet> dataset; //计算数据集
	RefDataSets refdatasets; //引用的数据集
	RefBufferSets refbuffersets; //引用的数据集
	int buffer_size; //有效buffer数目，即已经计算指标数目

	BufferSet():buffer_size(0)
	{
		
	}
	BufferSet(std::shared_ptr<Calculator> calculator, std::shared_ptr<DataSet> dataset)
	{
		this->calculator = calculator;
		this->dataset = dataset;
		this->refdatasets.clear();
		this->refbuffersets.clear();
		this->buffer_size = 0;
	}

    inline bool operator < (const BufferSet & r) const
    {
		if (calculator < r.calculator)
            return true;
        else if(r.calculator < calculator)
			return false;
		else
            return (*dataset < *r.dataset);
    };

    inline bool operator == (const BufferSet &r) const
    {
        return (calculator == r.calculator && *dataset == *r.dataset);
    }

	inline void Clear()
	{
		calculator = nullptr;
		dataset = nullptr;
		ClearRefData();
		ClearBuffer();
	}

	inline void AddRefBufferSet(std::shared_ptr<BufferSet> bufferset)
	{
		refbuffersets.push_back(bufferset);
	}

	inline void AddRefDataSet(std::shared_ptr<DataSet> dataset)
	{
		refdatasets.push_back(dataset);
	}

	inline void ClearRefData() 
	{
		refdatasets.clear();
		refbuffersets.clear();
	}
	
	inline void ClearBuffer()
	{
		buffer_size = 0;
	}
};

//context
template<class TInputSet, class TBufferInfoSet, class BaseSet>
struct CalculatorInfo : public BaseSet
{
public:
	typedef TInputSet InputSet;
	typedef TBufferInfoSet BufferInfoSet;
	typedef std::vector<BufferInfoSet> BufferInfoSets;
	const std::string name; //指标名称
	InputSet inputs; //输入信息
	BufferInfoSets buffers; //输出信息

	//CalculatorInfo() {}
	CalculatorInfo(const std::string& name):name(name) {}
	CalculatorInfo(const std::string& name, const InputSet& inputs)
	:name(name),inputs(inputs) {}

    inline bool operator < (const CalculatorInfo & r) const
    {
		if (name < r.name)
            return true;
        else if(r.name < name)
			return false;
		else {
			return inputs < r.inputs;
		}
        return false;
    };

    inline bool operator == (const CalculatorInfo &r) const
    {
        return (name == r.name && inputs == r.inputs);
    }
/*
	bool SetBufferInfo(const BufferInfoSet &info)
	{
		for (size_t index = 0; index < buffers.size(); index++)
		{
			if (buffers[index].name == info.name)
			{
				buffers[index] = info;
				return true;
				break;
			}
		}
		return false;
	}
	bool SetBufferInfo(size_t index, const BufferInfo &info)
	{
		if (index < buffers.size())
		{
			buffers[index] = info;
			return true;
		}
		return false;
	}
	bool GetBufferInfo(size_t index, BufferInfo &info)
	{
		if (index < buffers.size())
		{
			info = buffers[index];
			return true;
		}
		return false;
	}
	size_t GetBufferCount() { return buffers.size(); }*/
};

} // namespace XCalc

#endif //_H_XCALC_DEF_HPP_
