// Copyright (C) 2017 Deutsche Telekom AG.
//
// Authors: Andrei Pavel <andrei.pavel@qualitance.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//           http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <config.h>

#include <dhcpsrv/benchmarks/generic_host_data_source_benchmark.h>
#include <dhcpsrv/host_data_source_factory.h>
#include <dhcpsrv/testutils/mysql_schema.h>

#include <iostream>

using isc::dhcp::bench::GenericHostDataSourceBenchmark;
using isc::dhcp::test::createMySQLSchema;
using isc::dhcp::test::destroyMySQLSchema;
using isc::dhcp::HostDataSourceFactory;
using isc::dhcp::test::validMySQLConnectionString;
using std::cerr;
using std::endl;

namespace {

class MySqlHostDataSourceBenchmark : public GenericHostDataSourceBenchmark {
public:
    void SetUp(::benchmark::State const&) override {
        destroyMySQLSchema(false);
        createMySQLSchema(false);
        try {
            HostDataSourceFactory::destroy();
            HostDataSourceFactory::create(validMySQLConnectionString());
        } catch (...) {
            cerr << "ERROR: unable to open database" << endl;
            throw;
        }
        hdsptr_ = HostDataSourceFactory::getHostDataSourcePtr();
    }

    void TearDown(::benchmark::State const&) override {
        try {
            hdsptr_->rollback();
        } catch (...) {
            cerr << "WARNING: rollback has failed, this is expected if database"
                    " is opened in read-only mode, continuing..."
                 << endl;
        }
        HostDataSourceFactory::destroy();
        destroyMySQLSchema(false);
    }
};

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, insertHosts)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUp(state, host_count);
        insertHosts();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, updateHosts)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        updateHosts();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, getAll2)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        getAll2();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, getAll3)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        getAll3();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, getAll1)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        getAll1();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, get4_3)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        get4_3();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, get4_4)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        get4_4();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, get4_2)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        get4_2();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, get6_3)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        get6_3();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, get6_4)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        get6_4();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, get6_2_subnetid_address)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        get6_2_subnetid_address();
    }
}

BENCHMARK_DEFINE_F(MySqlHostDataSourceBenchmark, get6_2_prefix_prefixlen)(benchmark::State& state) {
    const size_t host_count = state.range(0);
    while (state.KeepRunning()) {
        ReentrantSetUpWithInserts(state, host_count);
        get6_2_prefix_prefixlen();
    }
}

constexpr size_t MIN_HOST_COUNT = 512;
constexpr size_t MAX_HOST_COUNT = 0xfffd;
constexpr benchmark::TimeUnit UNIT = benchmark::kMicrosecond;

BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, insertHosts)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, updateHosts)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, getAll2)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, getAll3)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, getAll1)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, get4_3)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, get4_4)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, get4_2)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, get6_3)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, get6_4)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, get6_2_subnetid_address)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);
BENCHMARK_REGISTER_F(MySqlHostDataSourceBenchmark, get6_2_prefix_prefixlen)->Range(MIN_HOST_COUNT, MAX_HOST_COUNT)->Unit(UNIT);

}  // namespace