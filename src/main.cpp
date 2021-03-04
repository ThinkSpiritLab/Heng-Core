#include "config.hpp"
#include "hc.hpp"

#include "util/argvPraser.hpp"

#include <iostream>

#include "json.hpp"
using Cfg       = HengCore::Config::Config;
using Excutable = HengCore::Excutable;
int main(int argc, char *argv[])
{
    auto arg = ArgvPraser::OptionPraser<Cfg>()
                 .add("tl", &Cfg::timeLimit)
                 .add("t", &Cfg::timeLimit)
                 .add("ml", &Cfg::memLimit)
                 .add("m", &Cfg::memLimit)
                 .add("uid", &Cfg::uid)
                 .add("u", &Cfg::uid)
                 .add("gid", &Cfg::gid)
                 .add("g", &Cfg::gid)
                 .add("pidl", &Cfg::maxPid)
                 .add("p", &Cfg::maxPid)
                 .add("stdin", &Cfg::stdin)
                 .add("i", &Cfg::stdin)
                 .add("stdout", &Cfg::stdout)
                 .add("o", &Cfg::stdout)
                 .add("stderr", &Cfg::stderr)
                 .add("e", &Cfg::stderr)
                 .add("f", &Cfg::outFd)
                 .add("cwd", &Cfg::cwd)
                 .add("c", &Cfg::cwd)
                 .add("args", &Cfg::args)
                 .add("a", &Cfg::args)
                 .add("bin", &Cfg::bin, true)
                 .prase(argc, argv);
    Excutable excutable(arg);
#ifdef DEBUG
    std::cout << nlohmann::json(arg).dump(4) << std::endl;
#endif
    if(excutable.exec())
    {
        if(arg.outFd != -1)
        {
            std::string result =
              nlohmann::json(excutable.getResult()).dump();
            // dup2(arg.outFd, fileno(stdout));
            write(arg.outFd, result.c_str(), result.size());
        }
        else
        {
            std::string result =
              nlohmann::json(excutable.getResult()).dump(4);
            std::cout << result << std::endl;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}