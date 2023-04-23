#pragma once
#include <functional>
#include <ostream>
#include <iomanip>
#include <string>
#include "fst/common.h"

FST_BEGIN_NAMESPACE
    namespace bench
    {
        void do_not_optimize_ptr(void*);
        void clobber();

        template <class Tp>
        inline void do_not_optimize(Tp& value)
        {
            return do_not_optimize_ptr((void*) &value);
        }

        enum class time_unit
        {
            nanosecond,
            microsecond,
            millisecond,
            second
        };

        enum class asymptotic_complexity
        {
            none,
            one,
            n,
            squared,
            cubed,
            log_n,
            n_log_n,
            inferred,
            lambda
        };

        typedef double(asymptotic_complexity_func)(int64_t);

        enum class statistic_unit
        {
            time,
            percentage
        };

        typedef double(statistics_func)(const std::vector<double>&);

        class benchmarks;
        class bench_case;

        class state
        {
          public:
            struct iterator
            {
                struct value
                {};

                typedef std::forward_iterator_tag iterator_category;
                typedef value value_type;
                typedef value reference;
                typedef value pointer;
                typedef ptrdiff_t difference_type;

              private:
                friend class state;

                inline iterator()
                    : cached_(0)
                    , parent_()
                {}
                explicit iterator(state* st);

              public:
                inline value operator*() const { return value(); }

                inline iterator& operator++()
                {
                    //assert(cached_ > 0);
                    --cached_;
                    return *this;
                }

                bool operator!=(iterator const&) const;

              private:
                int64_t cached_;
                state* const parent_;
            };

            iterator begin();
            iterator end();
            int64_t range(size_t pos = 0) const;

          private:
            friend class benchmarks;

            state(void* data);
            void* _data;
        };

        struct bench_result
        {
            std::string name;
            double real_time;
            double cpu_time;
            int64_t iterations;

            inline friend std::ostream& operator<<(std::ostream& stream, bench_result r)
            {
                return stream << std::left << "name: " << std::setw(18) << r.name << " real time: " << std::setw(18) << r.real_time << " cpu time: " << std::setw(18)
                              << r.cpu_time << " iterations: " << std::setw(18) << r.iterations;
            }
        };

        class bench_case
        {
          public:
            inline bench_case* operator->() { return this; }

            bench_case* name(const std::string& x);

            bench_case* arg(int64_t x);

            bench_case* unit(time_unit tu);

            // Run this benchmark once for a number of values picked from the
            // range [start..limit].  (start and limit are always picked.)
            // REQUIRES: The function passed to the constructor must accept an arg1.
            bench_case* range(int64_t start, int64_t limit);

            // Run this benchmark once for all values in the range [start..limit] with
            // specific step
            // REQUIRES: The function passed to the constructor must accept an arg1.
            bench_case* dense_range(int64_t start, int64_t limit, int step = 1);

            // Run this benchmark once with "args" as the extra arguments passed
            // to the function.
            // REQUIRES: The function passed to the constructor must accept arg1, arg2 ...
            bench_case* args(const std::vector<int64_t>& args);

            // Equivalent to Args({x, y})
            // NOTE: This is a legacy C++03 interface provided for compatibility only.
            //   New code should use 'Args'.
            bench_case* arg_pair(int64_t x, int64_t y);

            // Run this benchmark once for a number of values picked from the
            // ranges [start..limit].  (starts and limits are always picked.)
            // REQUIRES: The function passed to the constructor must accept arg1, arg2 ...
            bench_case* ranges(const std::vector<std::pair<int64_t, int64_t>>& r);

            // Run this benchmark once for each combination of values in the (cartesian)
            // product of the supplied argument lists.
            // REQUIRES: The function passed to the constructor must accept arg1, arg2 ...
            bench_case* args_product(const std::vector<std::vector<int64_t>>& arglists);

            // Equivalent to ArgNames({name})
            bench_case* arg_name(const std::string& name);

            // Set the argument names to display in the benchmark name. If not called,
            // only argument values will be shown.
            bench_case* arg_names(const std::vector<std::string>& names);

            // Equivalent to Ranges({{lo1, hi1}, {lo2, hi2}}).
            // NOTE: This is a legacy C++03 interface provided for compatibility only.
            //   New code should use 'Ranges'.
            bench_case* range_pair(int64_t lo1, int64_t hi1, int64_t lo2, int64_t hi2);

            // Pass this benchmark object to *func, which can customize
            // the benchmark by calling various methods like Arg, Args,
            // Threads, etc.
            bench_case* apply(void (*func)(bench_case* benchmark));

            // Have "setup" and/or "teardown" invoked once for every benchmark run.
            // If the benchmark is multi-threaded (will run in k threads concurrently),
            // the setup callback will be be invoked exactly once (not k times) before
            // each run with k threads. Time allowing (e.g. for a short benchmark), there
            // may be multiple such runs per benchmark, each run with its own
            // "setup"/"teardown".
            //
            // If the benchmark uses different size groups of threads (e.g. via
            // ThreadRange), the above will be true for each size group.
            //
            // The callback will be passed a State object, which includes the number
            // of threads, thread-index, benchmark arguments, etc.
            //
            // The callback must not be NULL or self-deleting.
            bench_case* setup(void (*s)(const state&));

            bench_case* teardown(void (*t)(const state&));

            // Set the range multiplier for non-dense range. If not called, the range
            // multiplier kRangeMultiplier will be used.
            bench_case* range_multiplier(int multiplier);

            // Set the minimum amount of time to use when running this benchmark. This
            // option overrides the `benchmark_min_time` flag.
            // REQUIRES: `t > 0` and `Iterations` has not been called on this benchmark.
            bench_case* min_time(double t);

            // Set the minimum amount of time to run the benchmark before taking runtimes
            // of this benchmark into account. This
            // option overrides the `benchmark_min_warmup_time` flag.
            // REQUIRES: `t >= 0` and `Iterations` has not been called on this benchmark.
            bench_case* min_warm_up_time(double t);

            // Specify the amount of iterations that should be run by this benchmark.
            // REQUIRES: 'n > 0' and `MinTime` has not been called on this benchmark.
            //
            // NOTE: This function should only be used when *exact* iteration control is
            //   needed and never to control or limit how long a benchmark runs, where
            // `--benchmark_min_time=N` or `MinTime(...)` should be used instead.
            bench_case* iterations(int64_t n);

            // Specify the amount of times to repeat this benchmark. This option overrides
            // the `benchmark_repetitions` flag.
            // REQUIRES: `n > 0`
            bench_case* repetitions(int n);

            // Specify if each repetition of the benchmark should be reported separately
            // or if only the final statistics should be reported. If the benchmark
            // is not repeated then the single result is always reported.
            // Applies to *ALL* reporters (display and file).
            bench_case* report_aggregates_only(bool value = true);

            // Same as ReportAggregatesOnly(), but applies to display reporter only.
            bench_case* display_aggregates_only(bool value = true);

            // By default, the CPU time is measured only for the main thread, which may
            // be unrepresentative if the benchmark uses threads internally. If called,
            // the total CPU time spent by all the threads will be measured instead.
            // By default, only the main thread CPU time will be measured.
            bench_case* measure_process_cpu_time();

            // If a particular benchmark should use the Wall clock instead of the CPU time
            // (be it either the CPU time of the main thread only (default), or the
            // total CPU usage of the benchmark), call this method. If called, the elapsed
            // (wall) time will be used to control how many iterations are run, and in the
            // printing of items/second or MB/seconds values.
            // If not called, the CPU time used by the benchmark will be used.
            bench_case* use_real_time();

            // If a benchmark must measure time manually (e.g. if GPU execution time is
            // being
            // measured), call this method. If called, each benchmark iteration should
            // call
            // SetIterationTime(seconds) to report the measured time, which will be used
            // to control how many iterations are run, and in the printing of items/second
            // or MB/second values.
            bench_case* use_manual_time();

            // Support for running multiple copies of the same benchmark concurrently
            // in multiple threads.  This may be useful when measuring the scaling
            // of some piece of code.

            // Run one instance of this benchmark concurrently in t threads.
            bench_case* threads(int t);

            // Pick a set of values T from [min_threads,max_threads].
            // min_threads and max_threads are always included in T.  Run this
            // benchmark once for each value in T.  The benchmark run for a
            // particular value t consists of t threads running the benchmark
            // function concurrently.  For example, consider:
            //    BENCHMARK(Foo)->ThreadRange(1,16)
            // {
            // return (bench_case*)this->Ar();
            // }
            // This will run the following benchmarks:
            //    Foo in 1 thread
            //    Foo in 2 threads
            //    Foo in 4 threads
            //    Foo in 8 threads
            //    Foo in 16 threads
            bench_case* thread_range(int min_threads, int max_threads);

            // For each value n in the range, run this benchmark once using n threads.
            // min_threads and max_threads are always included in the range.
            // stride specifies the increment. E.g. DenseThreadRange(1, 8, 3) starts
            // a benchmark with 1, 4, 7 and 8 threads.
            bench_case* dense_thread_range(int min_threads, int max_threads, int stride = 1);

            // Equivalent to ThreadRange(NumCPUs(), NumCPUs())
            bench_case* thread_per_cpu();

            time_unit get_time_unit() const;

            // Set the asymptotic computational complexity for the benchmark. If called
            // the asymptotic computational complexity will be shown on the output.
            bench_case* complexity(asymptotic_complexity c = asymptotic_complexity::inferred);

            // Set the asymptotic computational complexity for the benchmark. If called
            // the asymptotic computational complexity will be shown on the output.
            bench_case* complexity(asymptotic_complexity_func* c);

            // Add this statistics to be computed over all the values of benchmark run
            bench_case* compute_statistics(const std::string& name, statistics_func* s, statistic_unit u = statistic_unit::time);

          private:
            friend class benchmarks;
            bench_case(void* data);
            void* _data;
        };

        class benchmarks
        {
          public:
            benchmarks();

            ~benchmarks();

            std::vector<bench_result> run(bool print_output = true);
            void run(std::vector<bench_result>& results, bool print_output = true);

            bench_case add(const char* name, std::function<void(state&)>&& fct);
            bench_case add(const char* name, const std::function<void(state&)>& fct);

            template <class Lambda, class... Args>
            inline bench_case add(const char* name, Lambda&& fn, Args&&... args)
            {
                return add(name, std::function<void(state&)>([=](state& st) { fn(st, args...); }));
            }
        };

        class benchmarks_proxy
        {
          private:
            benchmarks_proxy() = delete;
            benchmarks_proxy(const benchmarks_proxy&) = delete;

            class proxy : benchmarks
            {
              public:
                proxy() = delete;
                proxy(const proxy&) = delete;

                template <class Lambda, class... Args>
                inline proxy(const char* name, Lambda&& fn, Args&&... args)
                    : _case(benchmarks::add(name, std::forward<Lambda>(fn), std::forward<Args>(args)...))
                {}

                ~proxy() { benchmarks::run(); }

                inline bench_case* operator->() { return &_case; }

              private:
                bench_case _case;
            };

            template <class Lambda, class... Args>
            friend proxy benchmark(const char* name, Lambda&& fn, Args&&... args);
        };

        template <class Lambda, class... Args>
        inline benchmarks_proxy::proxy benchmark(const char* name, Lambda&& fn, Args&&... args)
        {
            return benchmarks_proxy::proxy(name, std::forward<Lambda>(fn), std::forward<Args>(args)...);
        }

    } // namespace bench
FST_END_NAMESPACE
