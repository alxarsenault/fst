#include "fst_bench.h"
#include "benchmark/benchmark.h"
#include <iostream>

FST_BEGIN_NAMESPACE
    namespace bench
    {
        void do_not_optimize_ptr(void* ptr) { benchmark::DoNotOptimize(ptr); }

        void clobber() { benchmark::ClobberMemory(); }
        inline benchmark::internal::Benchmark* to_native(void* ptr) { return (benchmark::internal::Benchmark*) ptr; }

        inline benchmark::State* to_native_state(void* ptr) { return (benchmark::State*) ptr; }

        state::iterator::iterator(state* st)
            : cached_(to_native_state(st->_data)->error_occurred() ? 0 : to_native_state(st->_data)->max_iterations)
            , parent_(st)
        {}

        bool state::iterator::operator!=(state::iterator const&) const
        {
            //if (BENCHMARK_BUILTIN_EXPECT(cached_ != 0, true)) return true;

            struct iter_wrapper
            {

                benchmark::IterationCount cached_;
                benchmark::State* const parent_;
            };

            iter_wrapper w{ cached_, to_native_state(parent_->_data) };
            benchmark::State::StateIterator* it = (benchmark::State::StateIterator*) &w;

            return *it != to_native_state(parent_->_data)->begin();
            //to_native_state(parent_->_data)->FinishKeepRunning();
            //return false;
        }

        state::state(void* data)
            : _data(data)
        {}

        int64_t state::range(size_t pos) const { return to_native_state(_data)->range(pos); }

        state::iterator state::begin() { return iterator(this); }

        state::iterator state::end()
        {
            //StartKeepRunning();
            to_native_state(_data)->end();
            return iterator();
        }

        bench_case::bench_case(void* data)
            : _data(data)
        {}
        bench_case* bench_case::name(const std::string& x)
        {
            to_native(_data)->Name(x);
            return this;
        }

        bench_case* bench_case::arg(int64_t x)
        {
            to_native(_data)->Arg(x);
            return this;
        }

        bench_case* bench_case::unit(time_unit tu)
        {
            to_native(_data)->Unit((benchmark::TimeUnit) tu);
            return this;
        }

        // Run this benchmark once for a number of values picked from the
        // range [start..limit].  (start and limit are always picked.)
        // REQUIRES: The function passed to the constructor must accept an arg1.
        bench_case* bench_case::range(int64_t start, int64_t limit)
        {
            to_native(_data)->Range(start, limit);
            return this;
        }

        // Run this benchmark once for all values in the range [start..limit] with
        // specific step
        // REQUIRES: The function passed to the constructor must accept an arg1.
        bench_case* bench_case::dense_range(int64_t start, int64_t limit, int step)
        {
            to_native(_data)->DenseRange(start, limit, step);
            return this;
        }

        // Run this benchmark once with "args" as the extra arguments passed
        // to the function.
        // REQUIRES: The function passed to the constructor must accept arg1, arg2 ...
        bench_case* bench_case::args(const std::vector<int64_t>& args)
        {
            to_native(_data)->Args(args);
            return this;
        }

        // Equivalent to Args({x, y})
        // NOTE: This is a legacy C++03 interface provided for compatibility only.
        //   New code should use 'Args'.
        bench_case* bench_case::arg_pair(int64_t x, int64_t y)
        {
            to_native(_data)->ArgPair(x, y);
            return this;
        }

        // Run this benchmark once for a number of values picked from the
        // ranges [start..limit].  (starts and limits are always picked.)
        // REQUIRES: The function passed to the constructor must accept arg1, arg2 ...
        bench_case* bench_case::ranges(const std::vector<std::pair<int64_t, int64_t>>& r)
        {
            to_native(_data)->Ranges(r);
            return this;
        }

        // Run this benchmark once for each combination of values in the (cartesian)
        // product of the supplied argument lists.
        // REQUIRES: The function passed to the constructor must accept arg1, arg2 ...
        bench_case* bench_case::args_product(const std::vector<std::vector<int64_t>>& arglists)
        {
            to_native(_data)->ArgsProduct(arglists);
            return this;
        }

        // Equivalent to ArgNames({name})
        bench_case* bench_case::arg_name(const std::string& name)
        {
            to_native(_data)->ArgName(name);
            return this;
        }

        // Set the argument names to display in the benchmark name. If not called,
        // only argument values will be shown.
        bench_case* bench_case::arg_names(const std::vector<std::string>& names)
        {
            to_native(_data)->ArgNames(names);
            return this;
        }

        // Equivalent to Ranges({{lo1, hi1}, {lo2, hi2}}).
        // NOTE: This is a legacy C++03 interface provided for compatibility only.
        //   New code should use 'Ranges'.
        bench_case* bench_case::range_pair(int64_t lo1, int64_t hi1, int64_t lo2, int64_t hi2)
        {
            to_native(_data)->RangePair(lo1, hi1, lo2, hi2);
            return this;
        }

        // Pass this benchmark object to *func, which can customize
        // the benchmark by calling various methods like Arg, Args,
        // Threads, etc.
        bench_case* bench_case::apply(void (*func)(bench_case* benchmark))
        {
            to_native(_data)->Apply((void (*)(benchmark::internal::Benchmark*)) (void*)func);
            return this;
        }
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
        bench_case* bench_case::setup(void (*s)(const state&))
        {
            to_native(_data)->Setup((void (*)(const benchmark::State&)) (void*)s);

            return this;
        }

        bench_case* bench_case::teardown(void (*t)(const state&))
        {
            to_native(_data)->Teardown((void (*)(const benchmark::State&)) (void*)t);
            return this;
        }

        // Set the range multiplier for non-dense range. If not called, the range
        // multiplier kRangeMultiplier will be used.
        bench_case* bench_case::range_multiplier(int multiplier)
        {
            to_native(_data)->RangeMultiplier(multiplier);
            return this;
        }

        // Set the minimum amount of time to use when running this benchmark. This
        // option overrides the `benchmark_min_time` flag.
        // REQUIRES: `t > 0` and `Iterations` has not been called on this benchmark.
        bench_case* bench_case::min_time(double t)
        {
            to_native(_data)->MinTime(t);
            return this;
        }

        // Set the minimum amount of time to run the benchmark before taking runtimes
        // of this benchmark into account. This
        // option overrides the `benchmark_min_warmup_time` flag.
        // REQUIRES: `t >= 0` and `Iterations` has not been called on this benchmark.
        bench_case* bench_case::min_warm_up_time(double t)
        {
            to_native(_data)->MinWarmUpTime(t);
            return this;
        }

        // Specify the amount of iterations that should be run by this benchmark.
        // REQUIRES: 'n > 0' and `MinTime` has not been called on this benchmark.
        //
        // NOTE: This function should only be used when *exact* iteration control is
        //   needed and never to control or limit how long a benchmark runs, where
        // `--benchmark_min_time=N` or `MinTime(...)` should be used instead.
        bench_case* bench_case::iterations(int64_t n)
        {
            to_native(_data)->Iterations(n);
            return this;
        }

        // Specify the amount of times to repeat this benchmark. This option overrides
        // the `benchmark_repetitions` flag.
        // REQUIRES: `n > 0`
        bench_case* bench_case::repetitions(int n)
        {
            to_native(_data)->Repetitions(n);
            return this;
        }

        // Specify if each repetition of the benchmark should be reported separately
        // or if only the final statistics should be reported. If the benchmark
        // is not repeated then the single result is always reported.
        // Applies to *ALL* reporters (display and file).
        bench_case* bench_case::report_aggregates_only(bool value)
        {
            to_native(_data)->ReportAggregatesOnly(value);
            return this;
        }

        // Same as ReportAggregatesOnly(), but applies to display reporter only.
        bench_case* bench_case::display_aggregates_only(bool value)
        {
            to_native(_data)->DisplayAggregatesOnly(value);
            return this;
        }

        // By default, the CPU time is measured only for the main thread, which may
        // be unrepresentative if the benchmark uses threads internally. If called,
        // the total CPU time spent by all the threads will be measured instead.
        // By default, only the main thread CPU time will be measured.
        bench_case* bench_case::measure_process_cpu_time()
        {
            to_native(_data)->MeasureProcessCPUTime();
            return this;
        }

        // If a particular benchmark should use the Wall clock instead of the CPU time
        // (be it either the CPU time of the main thread only (default), or the
        // total CPU usage of the benchmark), call this method. If called, the elapsed
        // (wall) time will be used to control how many iterations are run, and in the
        // printing of items/second or MB/seconds values.
        // If not called, the CPU time used by the benchmark will be used.
        bench_case* bench_case::use_real_time()
        {
            to_native(_data)->UseRealTime();
            return this;
        }

        // If a benchmark must measure time manually (e.g. if GPU execution time is
        // being
        // measured), call this method. If called, each benchmark iteration should
        // call
        // SetIterationTime(seconds) to report the measured time, which will be used
        // to control how many iterations are run, and in the printing of items/second
        // or MB/second values.
        bench_case* bench_case::use_manual_time()
        {
            to_native(_data)->UseManualTime();
            return this;
        }

        // Support for running multiple copies of the same benchmark concurrently
        // in multiple threads.  This may be useful when measuring the scaling
        // of some piece of code.

        // Run one instance of this benchmark concurrently in t threads.
        bench_case* bench_case::threads(int t)
        {
            to_native(_data)->Threads(t);
            return this;
        }

        // Pick a set of values T from [min_threads,max_threads].
        // min_threads and max_threads are always included in T.  Run this
        // benchmark once for each value in T.  The benchmark run for a
        // particular value t consists of t threads running the benchmark
        // function concurrently.  For example, consider:
        //    BENCHMARK(Foo)->ThreadRange(1,16)
        // {
        // to_native(_data)->Ar();
        //return this;
        // }
        // This will run the following benchmarks:
        //    Foo in 1 thread
        //    Foo in 2 threads
        //    Foo in 4 threads
        //    Foo in 8 threads
        //    Foo in 16 threads
        bench_case* bench_case::thread_range(int min_threads, int max_threads)
        {
            to_native(_data)->ThreadRange(min_threads, max_threads);
            return this;
        }

        // For each value n in the range, run this benchmark once using n threads.
        // min_threads and max_threads are always included in the range.
        // stride specifies the increment. E.g. DenseThreadRange(1, 8, 3) starts
        // a benchmark with 1, 4, 7 and 8 threads.
        bench_case* bench_case::dense_thread_range(int min_threads, int max_threads, int stride)
        {
            to_native(_data)->DenseThreadRange(min_threads, max_threads, stride);
            return this;
        }

        // Equivalent to ThreadRange(NumCPUs(), NumCPUs())
        bench_case* bench_case::thread_per_cpu()
        {
            to_native(_data)->ThreadPerCpu();
            return this;
        }

        time_unit bench_case::get_time_unit() const { return (time_unit) to_native(_data)->GetTimeUnit(); }
        // Set the asymptotic computational complexity for the benchmark. If called
        // the asymptotic computational complexity will be shown on the output.
        bench_case* bench_case::complexity(asymptotic_complexity c)
        {
            to_native(_data)->Complexity((benchmark::BigO) c);
            return this;
        }

        // Set the asymptotic computational complexity for the benchmark. If called
        // the asymptotic computational complexity will be shown on the output.
        bench_case* bench_case::complexity(asymptotic_complexity_func* c)
        {
            to_native(_data)->Complexity((benchmark::BigOFunc*) (void*) c);
            return this;
        }

        // Add this statistics to be computed over all the values of benchmark run
        bench_case* bench_case::compute_statistics(const std::string& name, statistics_func* s, statistic_unit u)
        {
            to_native(_data)->ComputeStatistics(name, (benchmark::StatisticsFunc*) s, (benchmark::StatisticUnit) u);
            return this;
        }

        benchmarks::benchmarks() { benchmark::Initialize(nullptr, nullptr); }

        benchmarks::~benchmarks() { benchmark::Shutdown(); }

        bench_case benchmarks::add(const char* name, std::function<void(state&)>&& fct)
        {
            return bench_case((void*) benchmark::RegisterBenchmark(name,
                [f = std::move(fct)](benchmark::State& s)
                {
                    state ss((void*) &s);
                    f(ss);
                }));
        }

        bench_case benchmarks::add(const char* name, const std::function<void(state&)>& fct)
        {
            return bench_case((void*) benchmark::RegisterBenchmark(name,
                [=](benchmark::State& s)
                {
                    state ss((void*) &s);
                    fct(ss);
                }));
        }

        std::vector<bench_result> benchmarks::run(bool print_output)
        {
            std::vector<bench_result> results;
            run(results, print_output);
            return results;
        }

        void benchmarks::run(std::vector<bench_result>& results, bool print_output)
        {
            class Reporter : public benchmark::ConsoleReporter
            {
              public:
                std::vector<bench_result>& _results;
                bool _print_output;

                Reporter(std::vector<bench_result>& results, bool print_output)
                    : _results(results)
                    , _print_output(print_output)
                {
                    //printed_header_ = true;
                }

                bool ReportContext(const Context& context) override
                {
                    name_field_width_ = context.name_field_width;
                    printed_header_ = false;
                    prev_counters_.clear();

                    //PrintBasicContext(&GetErrorStream(), context);

                    return true;
                }

                //virtual void ReportRuns(const std::vector<Run>& reports) override {

                //}

              protected:
                virtual void PrintRunData(const benchmark::BenchmarkReporter::Run& report) override
                {
                    if (_print_output) { ConsoleReporter::PrintRunData(report); }

                    _results.push_back(bench_result{ report.benchmark_name(), report.GetAdjustedRealTime(), report.GetAdjustedCPUTime(), report.iterations });
                }

                virtual void PrintHeader(const benchmark::BenchmarkReporter::Run& report) override
                {
                    if (_print_output) { ConsoleReporter::PrintHeader(report); }
                }
            };

            Reporter rep(results, print_output);
            //size_t size =
            benchmark::RunSpecifiedBenchmarks(&rep);
        }
    } // namespace bench.
FST_END_NAMESPACE