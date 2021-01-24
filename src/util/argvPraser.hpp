#pragma once
#ifndef argv_praser
#    define argv_praser

#    include <algorithm>
#    include <cctype>
#    include <exception>
#    include <functional>
#    include <optional>
#    include <string>
#    include <unordered_map>
#    include <unordered_set>
#    include <vector>

namespace ArgvPraser
{
template <typename T>
class UnableToPrase: std::exception
{
private:
    std::string raw = std::string("Unable to prase type")
                      + typeid(T).name();

public:
    UnableToPrase(const std::string &str = "")
    {
        if(str.length() != 0)
        {
            raw.append(std::string(" from string :") + str);
        }
    }
    UnableToPrase &operator=(const UnableToPrase &other)
    {
        this->raw = other.raw;
    }
    const char *what() const noexcept override
    {
        return this->raw.c_str();
    }
};

template <typename T>
using PraseFunction = void(const std::string &, T &t);

template <typename T>
void prase(const std::string &str, T &t);

template <>
void prase<bool>(const std::string &str, bool &b)
{
    static const std::unordered_map<std::string, bool>
                boolDictionary = { { "yes", true }, { "y", true },
                         { "ok", true },  { "1", true },
                         { "no", false }, { "n", false } };
    std::string lower(str);
    std::transform(lower.begin(),
                   lower.end(),
                   lower.begin(),
                   tolower);
    auto it = boolDictionary.find(lower);
    if(it != boolDictionary.end())
    {
        b = it->second;
        return;
    }
    else
    {
        throw UnableToPrase<bool>(str);
    }
}

template <>
void prase<unsigned int>(const std::string &str,
                         unsigned int &     i)
{
    i = std::stoul(str);
}

template <>
void prase<int>(const std::string &str, int &i)
{
    i = std::stoi(str);
}

template <>
void prase<long>(const std::string &str, long &i)
{
    i = std::stol(str);
}

template <>
void prase<long long>(const std::string &str, long long &i)
{
    i = std::stoll(str);
}

template <>
void prase<std::string>(const std::string &str,
                        std::string &      s)
{
    s = str;
}

template <class Options>
class OptionPraser
{
private:
    class OptionParamBase
    {
    private:
    public:
        virtual int praseOp(Options &options,
                            int      argc,
                            char *   argv[]) const = 0;
        virtual ~OptionParamBase() = default;
    };

    template <typename OptionType>
    class OptionParam: public OptionParamBase
    {
    private:
        OptionType Options::*pointer;
        const std::function<PraseFunction<OptionType>>
          praser;

    public:
        OptionParam(OptionType Options::*p,
                    std::function<PraseFunction<OptionType>>
                      f = ArgvPraser::prase<OptionType>):
            pointer(p),
            praser(f)
        {
        }
        int praseOp(Options &options,
                    int      argc,
                    char *   argv[]) const override
        {
            if(argc)
            {
                praser(std::string(argv[0]),
                       options.*pointer);
                return 1;
            }
            else
            {
                return 0;
            }
        }
    };

    template <typename OptionType, typename Alloc>
    class OptionParam<std::vector<OptionType, Alloc>>:
        public OptionParamBase
    {
    private:
        std::vector<OptionType, Alloc> Options::*pointer;
        const std::function<PraseFunction<OptionType>>
          praser;

    public:
        OptionParam(
          std::vector<OptionType, Alloc> Options::*p,
          std::function<PraseFunction<OptionType>> f =
            ArgvPraser::prase<OptionType>):
            pointer(p),
            praser(f)
        {
        }
        int praseOp(Options &options,
                    int      argc,
                    char *   argv[]) const override
        {
            for(int i = 0; i < argc; ++i)
            {
                OptionType t;
                praser(std::string(argv[i]), t);
                (options.*pointer).push_back(t);
            }
            return argc;
        }
    };

    std::unordered_map<std::string, OptionParamBase *>
                                    optionMap;
    std::unordered_set<std::string> require;

public:
    template <typename OptionType>
    OptionPraser<Options> &add(const std::string &fullName,
                               OptionType Options::*pointer,
                               const bool required = false)
    {
        optionMap.insert(
          { fullName,
            new OptionParam<OptionType>(pointer) });
        if(required)
        {
            require.insert(fullName);
        }

        return *this;
    }

    Options &prase(int argc, char *argv[], Options &opt)
    {
        std::unordered_set<std::string> got;
        for(int cur = 1; cur < argc;)
        {
            if(argv[cur][0] == '-' && cur + 1 < argc)
            {
                std::string name(
                  argv[cur]
                  + std::string(argv[cur])
                      .find_first_not_of('-'));
                auto it = optionMap.find(name);
                if(it != optionMap.end())
                {
                    got.insert(name);
                    cur +=
                      1
                      + it->second->praseOp(opt,
                                            argc - cur - 1,
                                            argv + cur + 1);
                }
                else
                {
                    throw std::invalid_argument(
                      "Unknow arg " + name);
                }
            }
            else
            {
                ++cur;
            }
        }
        for(const auto &s: require)
        {
            if(got.find(s) == got.end())
            {
                throw std::invalid_argument("Missing arg "
                                            + s);
            }
        }
        return opt;
    }
    Options prase(int argc, char *argv[])
    {
        Options opt;
        return prase(argc, argv, opt);
    }
    ~OptionPraser()
    {
        for(auto iter: optionMap)
        {
            delete iter.second;
        }
    }
};
}  // namespace ArgvPraser

#endif