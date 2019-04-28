#include "sorter.hpp"
#include "prefetched.hpp"
#include <queue>
#include <cstdint>
#include <cstdio>
#include <iostream>

namespace top100 {

void ExternalSorter::sort() {
    const std::size_t MAX_MEM = 1 << 30;   // 1G
    const std::size_t MAX_SIZE = 20 << 20; // 10M
    const std::size_t PARALLEL_WORKERS = 8;
    static_assert((PARALLEL_WORKERS & (PARALLEL_WORKERS - 1)) == 0,
                  "PARALLEL_WORKERS must be power of 2");
    static_assert(PARALLEL_WORKERS * MAX_SIZE < (1 << 30),
                  "memory usage large than 1G");

    const char* tmp_file_prefix = "tmp_file_prefix_";

    std::vector<std::string> filenames;

    std::size_t idx = 0;
    std::string line;
    while (ifs_) {
        std::array<std::future<std::vector<std::string>>, PARALLEL_WORKERS>
            futures;

        for (std::size_t i = 0; i < PARALLEL_WORKERS; ++i) {
            std::size_t sz = 0;
            std::vector<std::string> xs;
            while (sz < MAX_SIZE && std::getline(ifs_, line)) {
                line.shrink_to_fit();

                sz += line.size();
                xs.push_back(std::move(line));
            }

            xs.shrink_to_fit();
            futures[i] = thread_pool_.submit([ys = std::move(xs)]() mutable {
                std::sort(ys.begin(), ys.end());
                return ys;
            });
        }

        std::vector<std::vector<std::string>> results;
        for (std::size_t i = 0; i < PARALLEL_WORKERS; i += 2) {
            std::vector<std::string> result;

            auto x0 = futures[i].get();
            auto x1 = futures[i + 1].get();
            std::merge(x0.begin(), x0.end(), x1.begin(), x1.end(),
                       std::back_inserter(result));

            result.shrink_to_fit();
            results.push_back(std::move(result));
        }

        while (results.size() > 1) {
            std::vector<std::vector<std::string>> rs;
            for (std::size_t i = 0; i < results.size(); i += 2) {
                std::vector<std::string> r;

                auto& x0 = results[i];
                auto& x1 = results[i + 1];
                std::merge(x0.begin(), x0.end(), x1.begin(), x1.end(),
                           std::back_inserter(r));

                r.shrink_to_fit();
                rs.push_back(std::move(r));
            }

            rs.swap(results);
        }

        char buf[32];
        std::snprintf(buf, sizeof(buf), "%s%zu", tmp_file_prefix, idx++);

        std::ofstream ofs(buf);
        for (const auto& l : results[0]) {
            ofs << l << '\n';
        }

        filenames.push_back(buf);
    }

    std::size_t current_chunk_size = MAX_SIZE * 2;
    while (current_chunk_size < MAX_MEM / 4) {
        std::vector<std::string> fnames;

        if (filenames.size() % 2) {
            fnames.push_back(std::move(filenames.front()));
        }

        for (std::size_t i = filenames.size() % 2; i < filenames.size(); i += 2) {
            const std::string& fn0 = filenames[i];
            const std::string& fn1 = filenames[i + 1];

            char buf[32];
            std::snprintf(buf, sizeof(buf), "%s%zu", tmp_file_prefix, idx++);
            std::ofstream ofs(buf);

            auto xs0 = getContent(std::ifstream(fn0)),
                 xs1 = getContent(std::ifstream(fn1));

            std::vector<std::string> result;
            std::merge(xs0.begin(), xs0.end(), xs1.begin(), xs1.end(),
                       std::back_inserter(result));

            result.shrink_to_fit();
            for (const auto& s : result) {
                ofs << s << '\n';
            }

            fnames.push_back(buf);

            std::remove(fn0.c_str());
            std::remove(fn1.c_str());
        }

        fnames.swap(filenames);
        current_chunk_size *= 2;
    }

    std::cout << "filename = " << filenames.size() << '\n';
    auto chunks = std::make_unique<Prefetched[]>(filenames.size());
    std::transform(filenames.begin(), filenames.end(), chunks.get(),
                   [](const std::string& name) {
                       return Prefetched(std::ifstream(name));
                   });

    struct ChunkInfo {
        Prefetched* prefetched_;

        bool operator<(const ChunkInfo& rhs) const {
            const auto& [avail, s] = prefetched_->peak();
            const auto& [rhs_avail, rhs_s] = rhs.prefetched_->peak();

            if (avail && rhs_avail) {
                return s > rhs_s;
            }
            return avail > rhs_avail;
        }
    };

    std::priority_queue<ChunkInfo> chunks_pq;
    for (std::size_t i = 0; i < filenames.size(); ++i) {
        chunks_pq.push(ChunkInfo{.prefetched_ = &chunks[i]});
    }

    while (!chunks_pq.empty()) {
        auto c = chunks_pq.top();

        const auto [avail, s] = c.prefetched_->get();
        if (!avail) {
            chunks_pq.pop();
            continue;
        }

        ofs_ << s << '\n';
    }

    for (const auto& f : filenames) {
        std::remove(f.c_str());
    }
}

std::vector<std::string> ExternalSorter::getContent(std::ifstream ifs) {
    std::string line;
    std::vector<std::string> ret;
    while (std::getline(ifs, line)) {
        line.shrink_to_fit();
        ret.push_back(std::move(line));
    }

    return ret;
}

}
