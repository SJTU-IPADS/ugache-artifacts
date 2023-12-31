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

#pragma once

#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "common.h"
#include "timer.h"

namespace coll_cache_lib {
namespace common {

enum LogInitItem {
  // L1
  kLogInitL1Common = 0,
  kLogInitL1Sampler, // pinmem, copy graph, *shuff, *hashtable, *random, *freq, queue, presmaple, cachetable
  kLogInitL1Trainer,
  // L2
  kLogInitL2LoadDataset,
  kLogInitL2DistQueue,
  kLogInitL2Presample,
  kLogInitL2InternalState,
  kLogInitL2BuildCache,
  // L3
  kLogInitL3LoadDatasetMMap,
  kLogInitL3LoadDatasetCopy,
  kLogInitL3DistQueueAlloc,
  kLogInitL3DistQueuePin,
  kLogInitL3DistQueuePush,
  kLogInitL3PresampleInit,
  kLogInitL3PresampleSample,
  kLogInitL3PresampleCopy,
  kLogInitL3PresampleCount,
  kLogInitL3PresampleSort,
  kLogInitL3PresampleReset,
  kLogInitL3PresampleGetRank,
  kLogInitL3InternalStateCreateCtx,
  kLogInitL3InternalStateCreateStream,
  kNumLogInitItems,
};

enum LogStepItem {
  // L1
  kLogL1NumSample = 0,
  kLogL1NumNode,
  kLogL1SampleTotalTime,
  kLogL1SampleTime,
  kLogL1SendTime,
  kLogL1RecvTime,
  kLogL1CopyTime,
  kLogL1ConvertTime,
  kLogL1TrainTime,
  kLogL1FeatureBytes,
  kLogL1LabelBytes,
  kLogL1IdBytes,
  kLogL1GraphBytes,
  kLogL1MissBytes,
  kLogL1RemoteBytes,
  kLogL1PrefetchAdvanced,
  kLogL1GetNeighbourTime,
  kLogL1SamplerId, // for arch9
  // L2
  kLogL2ShuffleTime,
  kLogL2LastLayerTime,
  kLogL2LastLayerSize,
  kLogL2CoreSampleTime,
  kLogL2IdRemapTime,
  kLogL2GraphCopyTime,
  kLogL2IdCopyTime,
  kLogL2ExtractTime,
  kLogL2FeatCopyTime,
  kLogL2CacheCopyTime,
  // L3
  kLogL3KHopSampleCooTime,
  kLogL3KHopSampleKernelTime,
  kLogL3KHopSampleSortCooTime,
  kLogL3KHopSampleCountEdgeTime,
  kLogL3KHopSampleCompactEdgesTime,
  kLogL3RandomWalkSampleCooTime,
  kLogL3RandomWalkTopKTime,
  kLogL3RandomWalkTopKStep1Time,
  kLogL3RandomWalkTopKStep2Time,
  kLogL3RandomWalkTopKStep3Time,
  kLogL3RandomWalkTopKStep4Time,
  kLogL3RandomWalkTopKStep5Time,
  kLogL3RandomWalkTopKStep6Time,
  kLogL3RandomWalkTopKStep7Time,
  kLogL3RemapFillUniqueTime,
  kLogL3RemapPopulateTime,
  kLogL3RemapMapNodeTime,
  kLogL3RemapMapEdgeTime,
  kLogL3CacheGetIndexTime,
  KLogL3CacheCopyIndexTime,
  kLogL3CacheExtractMissTime,
  kLogL3CacheCopyMissTime,
  kLogL3CacheCombineMissTime,
  kLogL3CacheCombineCacheTime,
  kLogL3CacheCombineRemoteTime,
  kLogL3LabelExtractTime,
  // Number of items
  kNumLogStepItems
};

enum LogEpochItem {
  kLogEpochSampleTime = 0,
  KLogEpochSampleGetCacheMissIndexTime,  // for arch5
  kLogEpochSampleSendTime,               // for arch5
  kLogEpochSampleTotalTime,              // for arch5
  kLogEpochCoreSampleTime,
  kLogEpochSampleCooTime,
  kLogEpochIdRemapTime,
  kLogEpochShuffleTime,
  kLogEpochSampleKernelTime,
  kLogEpochCopyTime,
  kLogEpochConvertTime,
  kLogEpochTrainTime,
  kLogEpochTotalTime,
  kLogEpochFeatureBytes,
  kLogEpochMissBytes,
  kNumLogEpochItems,
};

struct LogData {
  std::vector<double> vals;
  double sum;
  size_t cnt;
  std::vector<int> bitmap;

  LogData(size_t num_logs);
};

// log space across multiple process
struct SharedLogData {
  double* vals;
  int* bitmap;

  SharedLogData() {}
};

struct SortedLogData {
  double* vals;
  size_t cnt;

  SortedLogData() {}
};

#define TRACE_TYPES( F ) \
  F(kL0Event_Train_Step) \
  F(kL1Event_Sample) \
  F(kL2Event_Sample_Shuffle) \
  F(kL2Event_Sample_Core) \
  F(kL2Event_Sample_IdRemap) \
  F(kL3Event_Sample_Core_Coo) \
  F(kL1Event_Copy) \
  F(kL2Event_Copy_Id) \
  F(kL2Event_Copy_Graph) \
  F(kL2Event_Copy_Extract) /*for non cache*/ \
  F(kL2Event_Copy_FeatCopy) /*for non cache*/\
  F(kL2Event_Copy_CacheCopy) /*for cache*/ \
  F(kL3Event_Copy_CacheCopy_GetIndex) \
  F(kL3Event_Copy_CacheCopy_CopyIndex) \
  F(kL3Event_Copy_CacheCopy_ExtractMiss) \
  F(kL3Event_Copy_CacheCopy_CopyMiss) \
  F(kL3Event_Copy_CacheCopy_CombineMiss) \
  F(kL3Event_Copy_CacheCopy_CombineCache) \
  F(kL1Event_Convert) \
  F(kL1Event_Train)

#define F(name) name,
enum TraceItem {TRACE_TYPES( F ) kNumTraceItems };
#undef F

struct TraceEvent {
  uint64_t begin, end;

  TraceEvent();
};
struct TraceData {
  std::vector<TraceEvent> events;

  TraceData(size_t num_traces);
};

class Profiler {
 public:
  Profiler();
  void ResetStepEpoch();
  void LogInit(LogInitItem item, double val);
  void LogInitAdd(LogInitItem item, double val);
  void LogStep(uint64_t key, LogStepItem item, double val);
  void LogStepAdd(uint64_t key, LogStepItem item, double val);
  void LogEpochAdd(uint64_t key, LogEpochItem item, double val);
  inline void LogHPSAdd(const std::vector<double> &ratios) {_hps_cache_ratios = ratios;}
  inline void LogRefreshEnd() {_refresh_duration = _seq_ts->Passed();}

  inline void TraceStepBegin(uint64_t key, TraceItem item, uint64_t us) { _step_trace[item].events[key].begin = us; }
  inline void TraceStepEnd(uint64_t key, TraceItem item, uint64_t us) { _step_trace[item].events[key].end = us; }

  double GetLogInitValue(LogInitItem item);
  double GetLogStepValue(uint64_t key, LogStepItem item);
  double GetLogEpochValue(uint64_t epoch, LogEpochItem item);

  void ReportInit();
  void ReportStep(uint64_t epoch, uint64_t step);
  void ReportStepAverage(uint64_t epoch, uint64_t step);
  void ReportStepAverageLastEpoch(uint64_t epoch, uint64_t step);
  void ReportStepMax(uint64_t epoch, uint64_t step);
  void ReportStepMin(uint64_t epoch, uint64_t step);
  void ReportStepPercentile(uint64_t epoch, uint64_t step, double percentage);
  void ReportStepItemPercentiles(uint64_t epoch, uint64_t step, LogStepItem item, std::vector<double> percentages, const char* type);
  void ReportSequentialAverage(uint64_t bucket_size, std::ostream &out);
  void ReportEpoch(uint64_t epoch);
  void ReportEpochAverage(uint64_t epoch);

  void DumpTrace(std::ostream & of);

  void LogNodeAccess(uint64_t key, const IdType *input, size_t num_input);
  // void ReportNodeAccess();
  void ReportNodeAccessSimple();

  // static Profiler &Get();

 private:
  template<typename ReduceOp>
  void PrepareStepReduce(uint64_t epoch, uint64_t step, const double init, ReduceOp op, const double default_val = 0);
  void StepSort(uint64_t epoch, uint64_t step);
  void SetSeqTimeStamp(uint64_t key);
  void OutputStep(uint64_t key, std::string type, std::ostream &out = std::cout, double ts = 0);
  void OutputEpoch(uint64_t epoch, std::string type);

  std::vector<LogData> _init_data;
  std::vector<SharedLogData> _step_data;
  std::vector<SortedLogData> _sorted_step_data;
  std::vector<double> _step_buf;
  std::vector<LogData> _epoch_data;
  std::vector<double> _epoch_buf;
  // SharedLogData* _step_data;
  TensorPtr _step_data_val_buf;
  TensorPtr _step_data_bitmap_buf;
  
  // timestamp for sequence report
  Timer *_seq_ts;
  double _refresh_duration = 0; 
  std::vector<double> _seq_duration; 

  // for trace
  std::vector<TraceData> _step_trace;
  // std::vector<TraceData> _epoch_trace;
  // uint64_t _num_step;

  // for node access
  std::vector<size_t> _node_access;
  std::vector<int> _last_visit;
  std::vector<size_t> _similarity;
  std::vector<int> _epoch_last_visit;
  std::vector<int> _epoch_cur_visit;
  std::vector<double> _epoch_similarity;

  // for HPS: 
  // cache intersect ratio; 
  // cache hit ratio;
  // access hit overlap ratio; 
  // access miss overlap ratio;
  std::vector<double> _hps_cache_ratios;
};

}  // namespace common
}  // namespace samgraph
