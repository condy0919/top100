#include "version.hpp"
#include "sorter.hpp"
#include "counter.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    std::string input_file;
    std::string output_file;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Produce help message")
        ("version,v", "Print version details")
        ("input,i", po::value<std::string>(&input_file), "Input file")
        ("output,o", po::value<std::string>(&output_file), "Output file")
        ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    if (argc == 1 || vm.count("help") || !vm.count("input")) {
        std::cerr << "Usage: " << argv[0] << " -i <input>\n"
                  << desc << '\n';
        return 0;
    } else if (vm.count("version")) {
        std::cerr << "version: " << version << '\n';
        return 0;
    }

    top100::ExternalSorter sorter{std::ifstream(input_file), std::ofstream(output_file)};
    sorter.sort();

    top100::Counter counter{std::ifstream(output_file)};
    counter.count();

    for (const auto& r : counter.getResult()) {
        std::cout << r.first << ' ' << r.second << '\n';
    }

    return 0;
}
