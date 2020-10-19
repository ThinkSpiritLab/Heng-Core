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
T prase(const std::string &str);

template <>
bool prase<bool>(const std::string &str)
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
        return it->second;
    }
    else
    {
        throw UnableToPrase<bool>(str);
    }
}

template <>
int prase<int>(const std::string &str)
{
    return std::stoi(str);
}

template <>
long prase<long>(const std::string &str)
{
    return std::stol(str);
}

template <>
long long prase<long long>(const std::string &str)
{
    return std::stoll(str);
}

template <>
std::string prase<std::string>(const std::string &str)
{
    return str;
}

template <class Options>
class OptionPraser
{
private:
    class OptionParamBase
    {
    private:
    public:
        virtual bool
        praseOp(Options &          options,
                const std::string &str) const = 0;
    };

    template <typename OptionType>
    class OptionParam: public OptionParamBase
    {
    private:
        OptionType Options::*pointer;
        const std::function<OptionType(const std::string &)>
          praser;

    public:
        OptionParam(
          OptionType Options::*                          p,
          std::function<OptionType(const std::string &)> f =
            ArgvPraser::prase<OptionType>):
            pointer(p),
            praser(f)
        {
        }
        bool praseOp(Options &          options,
                     const std::string &str) const override
        {
            options.*pointer = praser(str);
            return true;
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

    Options prase(int argc, char *argv[])
    {
        Options                         opt;
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
                    it->second->praseOp(opt, argv[cur + 1]);
                }
                else
                {
                    throw std::invalid_argument(
                      "Unknow arg " + name);
                }
                cur += 2;
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
    Options prase(int argc, char *argv[], Options &opt) {}
};
}  // namespace ArgvPraser

#endif