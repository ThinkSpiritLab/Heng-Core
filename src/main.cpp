#include "config.hpp"
#include "hc.hpp"

#include "util/argvPraser.hpp"

#include <iostream>

#include "json.hpp"
using Cfg       = HengCore::Config::Config;
using Excutable = HengCore::Excutable;
int main(int argc, char *argv[])
{
    auto praser = ArgvPraser::OptionPraser<Cfg>();
    praser.add("tl", &Cfg::timeLimit);
    praser.add("t", &Cfg::timeLimit);
    praser.add("ml", &Cfg::memLimit);
    praser.add("m", &Cfg::memLimit);
    // praser.add("ol", &Cfg::outPutLimit);
    praser.add("uid", &Cfg::uid);
    praser.add("u", &Cfg::uid);
    praser.add("gid", &Cfg::gid);
    praser.add("g", &Cfg::gid);
    praser.add("pidl", &Cfg::maxPid);
    praser.add("p", &Cfg::maxPid);
    praser.add("stdin", &Cfg::stdin);
    praser.add("i", &Cfg::stdin);
    praser.add("stdout", &Cfg::stdout);
    praser.add("o", &Cfg::stdout);
    praser.add("stderr", &Cfg::stderr);
    praser.add("e", &Cfg::stderr);
    praser.add("f", &Cfg::outFd);
    praser.add("cwd", &Cfg::cwd);
    praser.add("c", &Cfg::cwd);
    praser.add("args", &Cfg::args);
    praser.add("a", &Cfg::args);
    praser.add("bin", &Cfg::bin, true);
    auto      arg = praser.prase(argc, argv);
    Excutable excutable(arg);
#ifdef DEBUG
    std::cout << nlohmann::json(arg).dump(4) << std::endl;
#endif
    if(excutable.exec())
    {
        std::string result =
          nlohmann::json(excutable.getResult()).dump(4);
        if(arg.outFd != -1)
        {
            // dup2(arg.outFd, fileno(stdout));
            write(arg.outFd, result.c_str(), result.size());
        }
        else
        {
            std::cout << result << std::endl;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}