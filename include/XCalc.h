#ifndef _H_CALC_H_
#define _H_CALC_H_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <bitset>
#include <boost/variant.hpp>

namespace XCalc
{
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
typedef boost::variant<int64_t, double, std::string> variant;
typedef void *iHandle;
typedef void *voidptr;

enum LogLevel
{
	log_error,
	log_warn,
	log_debug,
	log_info
};

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

enum IndicatorType { eIndicator = 0, eStrategy };

//context
struct IndicatorInfo
{
	std::string name;			   //指标名称
	IndicatorType type;
	std::vector<InputVal> inputs;  //输入信息,除了显示信息外的可以用户修改的计算信息
	std::vector<IndexInfo> indexs; //index信息
};

struct IndicatorDataInfo
{
	std::string symbol; //当前标示
	voidptr dataptr;	//计算数据

	void Clear()
	{
		symbol.clear();
		dataptr = nullptr;
	}
};

struct IndicatorBufferInfo
{
	std::vector<IndexVal> indexs;
	int counted;	//已经计算指标数目

	void Clear()
	{
		for(size_t i = 0; i < indexs.size(); i++)
		{
			indexs[i].clear();
		}
		counted = 0;
	}
};

} // namespace XCalc

#endif //_H_MARKETAPP_H_
