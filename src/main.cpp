#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
#include "sorter.hpp"
#include "counter.hpp"

int main() {
    const char* url_file = "100g.url";
    const char* output_file = "url.sorted";

    {
        top100::ExternalSorter sorter{std::ifstream(url_file), std::ofstream(output_file)};
        sorter.sort();
    }

    std::cout << "sort done\n";

    top100::Counter counter{std::ifstream(output_file)};
    counter.count();
    for (const auto& r : counter.getResult()) {
        std::cout << r.second << ": " << r.first << '\n';
    }

    return 0;
}
