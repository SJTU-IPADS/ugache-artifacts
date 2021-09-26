#include "pre_sampler.h"
#include "../profiler.h"
#include "../common.h"
#include "../constant.h"
#include "../logging.h"
#include "cuda_loops.h"
#include "cuda_engine.h"
#include "cuda_device.h"
#include <cstring>
#ifdef __linux__
#include <parallel/algorithm>
#else
#include <algorithm>
#endif
#include "../timer.h"


namespace samgraph {
namespace common {
namespace cuda {
PreSampler* PreSampler::singleton = nullptr;
PreSampler::PreSampler(size_t num_nodes, size_t num_step) :
    _num_nodes(num_nodes),
    _num_step(num_step) {
  Timer t_init;
  freq_table = new Id64Type[num_nodes];
#pragma omp parallel for num_threads(RunConfig::omp_thread_num)
  for (size_t i = 0; i < _num_nodes; i++) {
    auto nid_ptr = reinterpret_cast<IdType*>(&freq_table[i]);
    *nid_ptr = i;
    *(nid_ptr + 1) = 0;
  }
  Profiler::Get().LogInitAdd(kLogInitL2PresampleInit, t_init.Passed());

}

TensorPtr PreSampler::DoPreSample(){
  auto sampler_ctx = GPUEngine::Get()->GetSamplerCtx();
  auto sampler_device = Device::Get(sampler_ctx);
  for (int e = 0; e < RunConfig::presample_epoch; e++) {
    for (size_t i = 0; i < _num_step; i++) {
      Timer t0;
      auto task = DoShuffle();
      switch (RunConfig::cache_policy) {
        case kCacheByPreSample:
          DoGPUSample(task);
          break;
        case kCacheByPreSampleStatic:
          DoGPUSampleAllNeighbour(task);
          break;
        default:
          CHECK(0);
      }
      double sample_time = t0.Passed();
      size_t num_inputs = task->input_nodes->Shape()[0];
      Timer t1;
      IdType* input_nodes = new IdType[num_inputs];
      sampler_device->CopyDataFromTo(
        task->input_nodes->Data(), 0, input_nodes, 0, 
        num_inputs * sizeof(IdType), task->input_nodes->Ctx(), CPU());
      double copy_time = t1.Passed();
      Timer t2;
#pragma omp parallel for num_threads(RunConfig::omp_thread_num)
      for (size_t i = 0; i < num_inputs; i++) {
        auto freq_ptr = reinterpret_cast<IdType*>(&freq_table[input_nodes[i]]);
        *(freq_ptr+1) += 1;
      }
      double count_time = t2.Passed();
      Profiler::Get().LogInitAdd(kLogInitL2PresampleSample, sample_time);
      Profiler::Get().LogInitAdd(kLogInitL2PresampleCopy, copy_time);
      Profiler::Get().LogInitAdd(kLogInitL2PresampleCount, count_time);
    }
  }
  Timer ts;
#ifdef __linux__
  __gnu_parallel::sort(freq_table, &freq_table[_num_nodes],
                       std::greater<Id64Type>());
#else
  std::sort(freq_table, &freq_table[_num_nodes],
            std::greater<Id64Type>());
#endif
  double sort_time = ts.Passed();
  Profiler::Get().LogInit(kLogInitL2PresampleSort, sort_time);
  Timer t_reset;
  GPUEngine::Get()->GetShuffler()->Reset();
  Profiler::Get().ResetStepEpoch();
  Profiler::Get().LogInit(kLogInitL2PresampleReset, t_reset.Passed());
  Timer t_prepare_rank;
  auto rank_ptr = GetRankNode();
  Profiler::Get().LogInit(kLogInitL2PresampleGetRank, t_prepare_rank.Passed());
  return rank_ptr;
}

TensorPtr PreSampler::GetFreq() {
  auto ranking_freq = Tensor::Empty(DataType::kI32, {_num_nodes}, CPU(), "");
  auto ranking_freq_ptr = static_cast<IdType*>(ranking_freq->MutableData());
  for (size_t i = 0; i < _num_nodes; i++) {
    auto nid_ptr = reinterpret_cast<IdType*>(&freq_table[i]);
    ranking_freq_ptr[i] = *(nid_ptr + 1);
  }
  return ranking_freq;
}
TensorPtr PreSampler::GetRankNode() {
  auto ranking_nodes = Tensor::Empty(DataType::kI32, {_num_nodes}, CPU(), "");
  auto ranking_nodes_ptr = static_cast<IdType*>(ranking_nodes->MutableData());
  for (size_t i = 0; i < _num_nodes; i++) {
    auto nid_ptr = reinterpret_cast<IdType*>(&freq_table[i]);
    ranking_nodes_ptr[i] = *(nid_ptr);
  }
  return ranking_nodes;
}

}
}
}