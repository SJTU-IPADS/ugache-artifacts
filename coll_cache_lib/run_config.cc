/*
 * Copyright 2022 Institute of Parallel and Distributed Systems, Shanghai Jiao Tong University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "run_config.h"

#include "common.h"
#include "constant.h"
#include "logging.h"
#include <unordered_set>

namespace coll_cache_lib {
namespace common {


// clang-format off
CachePolicy          RunConfig::cache_policy                   = kCollCacheAsymmLink;
double               RunConfig::cache_percentage               = 0.0f;

bool                 RunConfig::is_configured                  = false;

// For arch7
size_t               RunConfig::worker_id                      = 0;
size_t               RunConfig::num_device                     = 1;
bool                 RunConfig::cross_process                  = false;
std::vector<int>     RunConfig::device_id_list                   = {0};

bool                 RunConfig::option_profile_cuda            = false;
bool                 RunConfig::option_log_node_access         = false;
bool                 RunConfig::option_log_node_access_simple  = false;

bool                 RunConfig::option_dump_trace              = false;
size_t               RunConfig::option_empty_feat              = 0;

size_t               RunConfig::option_fake_feat_dim = 0;

int                  RunConfig::omp_thread_num                   = 40;
int                  RunConfig::solver_omp_thread_num            = 40;
int                  RunConfig::solver_omp_thread_num_per_gpu    = 10;
int                  RunConfig::refresher_omp_thread_num         = 8;
int                  RunConfig::refresher_omp_thread_num_per_gpu = 1;

std::string          RunConfig::shared_meta_path               = "/shared_meta_data";
// clang-format on

ConcurrentLinkImpl   RunConfig::concurrent_link_impl       = kConcurrentExtractAuto;
bool                 RunConfig::coll_cache_concurrent_link = false;
size_t               RunConfig::coll_cache_num_slot    = 100;
double               RunConfig::coll_cache_coefficient = 1.2;
double               RunConfig::coll_cache_hyperparam_T_local  = 1;
double               RunConfig::coll_cache_hyperparam_T_remote = 438 / (double)213;  // performance on A100
double               RunConfig::coll_cache_hyperparam_T_cpu    = 438 / (double)11.8; // performance on A100
double               RunConfig::coll_cache_cpu_addup = 0.02;
size_t               RunConfig::coll_cache_scale_nb = 0;
HashImpl             RunConfig::coll_hash_impl = kHashImplAuto;
bool                 RunConfig::coll_skip_hash = false;

size_t               RunConfig::seed  = 1;

uint64_t             RunConfig::num_global_step_per_epoch = 0;
uint64_t             RunConfig::num_epoch = 0;
uint64_t             RunConfig::num_total_item = 0;
uint64_t             RunConfig::num_bucket_step = 0;

coll_cache::AsymmLinkDesc RunConfig::coll_cache_link_desc;

RollingPolicy        RunConfig::rolling = AutoRolling;

void RunConfig::LoadConfigFromEnv() {
  std::unordered_set<std::string> ture_values = {"TRUE", "1", "ON"};
  if (IsEnvSet(Constant::kEnvProfileCuda)) {
    RunConfig::option_profile_cuda = true;
  }

  if (IsEnvSet(Constant::kEnvLogNodeAccessSimple)) {
    RunConfig::option_log_node_access_simple = true;
  }

  if (IsEnvSet(Constant::kEnvLogNodeAccess)) {
    RunConfig::option_log_node_access = true;
  }

  if (IsEnvSet(Constant::kEnvDumpTrace)) {
    RunConfig::option_dump_trace = true;
  }
  if (GetEnv(Constant::kEnvEmptyFeat) != "") {
    RunConfig::option_empty_feat = std::stoul(GetEnv(Constant::kEnvEmptyFeat));
  }

  if (GetEnv(Constant::kEnvFakeFeatDim) != "") {
    std::string env = GetEnv(Constant::kEnvFakeFeatDim);
    RunConfig::option_fake_feat_dim = std::stoi(env);
  }
  if (GetEnv("SAMGRAPH_COLL_CACHE_NO_GROUP") != "") {
    if (GetEnv("SAMGRAPH_COLL_CACHE_NO_GROUP") == "DIRECT") {
      RunConfig::concurrent_link_impl = kDirectNoGroup;
    } else if (GetEnv("SAMGRAPH_COLL_CACHE_NO_GROUP") == "ORDERED") {
      RunConfig::concurrent_link_impl = kOrderedNoGroup;
    } else {
      CHECK(false) << "Unknown nogroup impl " << GetEnv("SAMGRAPH_COLL_CACHE_NO_GROUP");
    }
  }
  if (GetEnv("COLL_HASH_IMPL") != "") {
    if (GetEnv("COLL_HASH_IMPL") == "DEFAULT") {
      RunConfig::coll_hash_impl = kDefault;
    } else if (GetEnv("COLL_HASH_IMPL") == "RR") {
      RunConfig::coll_hash_impl = kRR;
    } else if (GetEnv("COLL_HASH_IMPL") == "CHUNK") {
      RunConfig::coll_hash_impl = kChunk;
    } else {
      CHECK(false) << "Unknown hash impl " << GetEnv("COLL_HASH_IMPL");
    }
  }
  if (GetEnv("COLL_SKIP_HASH") != "") {
    RunConfig::coll_skip_hash = ture_values.find(GetEnv("COLL_SKIP_HASH")) != ture_values.end();
  }
  if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK") != "") {
    CHECK(false) << "deprecated env SAMGRAPH_COLL_CACHE_CONCURRENT_LINK";
  }
  if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "FUSED") {
    RunConfig::concurrent_link_impl = kFused;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "FUSED_LIMIT_BLOCK") {
    RunConfig::concurrent_link_impl = kFusedLimitNumBlock;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "MULTI_KERNEL") {
    RunConfig::concurrent_link_impl = kMultiKernelNumBlock;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "MULTI_KERNEL_OLD") {
    RunConfig::concurrent_link_impl = kMultiKernelNumBlockOld;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "MPS") {
    RunConfig::concurrent_link_impl = kMPS;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "MPSForLandC") {
    RunConfig::concurrent_link_impl = kMPSForLandC;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "MPSPhase") {
    RunConfig::concurrent_link_impl = kMPSPhase;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "DIRECT") {
    RunConfig::concurrent_link_impl = kDirectNoGroup;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "ORDERED") {
    RunConfig::concurrent_link_impl = kOrderedNoGroup;
  } else if (GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL") == "") {
    RunConfig::concurrent_link_impl = coll_cache::AutoDecideConcurrentExtractImpl();
  } else {
    CHECK(false) << "Unknown concurrent link impl " << GetEnv("SAMGRAPH_COLL_CACHE_CONCURRENT_LINK_IMPL");
  }
  LOG(ERROR) << "using concurrent impl " << RunConfig::concurrent_link_impl;
  RunConfig::coll_cache_concurrent_link = coll_cache::AutoEnableConcurrentLink();
  if (GetEnv("COLL_CACHE_CPU_ADDUP") != "") {
    RunConfig::coll_cache_cpu_addup = std::stod(GetEnv("COLL_CACHE_CPU_ADDUP"));
  }
  if (GetEnv("COLL_CACHE_SCALE") != "") {
    RunConfig::coll_cache_scale_nb = std::stod(GetEnv("COLL_CACHE_SCALE"));
  }

  if (GetEnv("PROFILE_SEQ_BUCKET_SZ") != "") {
    RunConfig::num_bucket_step = std::stod(GetEnv("PROFILE_SEQ_BUCKET_SZ"));
  }
}


}  // namespace common
}  // namespace coll_cache_lib
