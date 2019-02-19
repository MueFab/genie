#include "conformance/program_options.h"

#include <cassert>

#include "conformance/exceptions.h"
#include "conformance/log.h"


namespace genie {


ProgramOptions::ProgramOptions(int argc, char* argv[]) : inputFilePath()
{
    processCommandLine(argc, argv);
}


ProgramOptions::~ProgramOptions() = default;


void ProgramOptions::processCommandLine(int argc, char* argv[])
{
    try
    {
        namespace po = boost::program_options;

        // Declare the supported options
        po::options_description options("Options");
        options.add_options()
                (
                        "help,h",
                        "Help"
                )
                (
                        "input_file_path,i",
                        po::value<std::string>(&(this->inputFilePath))->required(),
                        "Input file path"
                );

        // Parse the command line
        po::variables_map optionsMap;
        po::store(po::command_line_parser(argc, argv).options(options).run(), optionsMap);

        // First thing to do is to print the help
        if (optionsMap.count("help") || optionsMap.count("h"))
        {
            std::stringstream optionsStringStream;
            optionsStringStream << options;
            std::string optionsLine;
            while (std::getline(optionsStringStream, optionsLine))
            {
                GENIE_LOG_INFO << optionsLine;
            }
            exit(0);  // Just get out here, quickly
        }

        // po::notify() will // throw on erroneous program options, that's why we
        // call it after printing the help
        po::notify(optionsMap);
    }
    catch (const boost::program_options::error& e)
    {
        GENIE_DIE("Program options error: " + std::string(e.what()));
    }
}


}  // namespace genie
