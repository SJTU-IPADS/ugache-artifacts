#include <string>
#include <vector>

#include "operation.h"
#include "logging.h"
#include "cuda/cuda_engine.h"
#include "cuda/cuda_loops.h"

namespace samgraph {
namespace common {

extern "C" {

void samgraph_init(const char *path, int sample_device, int train_device,
                   size_t batch_size, int *fanout, size_t num_fanout, int num_epoch) {
    cuda::SamGraphCudaEngine::Init(path, sample_device, train_device,  batch_size,
                         std::vector<int>(fanout, fanout + num_fanout), num_epoch);
    SAM_LOG(INFO) << "SamGraph has been initialied successfully";
    return;
}

void samgraph_start() {
    SAM_CHECK(cuda::SamGraphCudaEngine::IsInitialized() && !cuda::SamGraphCudaEngine::IsShutdown());

    std::vector<cuda::LoopFunction> func;
    func.push_back(cuda::HostPermutateLoop);
    func.push_back(cuda::IdCopyHost2DeviceLoop);
    func.push_back(cuda::DeviceSampleLoop);
    func.push_back(cuda::GraphCopyDevice2DeviceLoop);
    func.push_back(cuda::IdCopyDevice2HostLoop);
    func.push_back(cuda::HostFeatureExtractLoop);
    func.push_back(cuda::FeatureCopyHost2DeviceLoop);
    func.push_back(cuda::SubmitLoop);

    // func.push_back(SingleLoop);

    cuda::SamGraphCudaEngine::Start(func);
    SAM_LOG(INFO) << "SamGraph has been started successfully";
}

int samgraph_num_epoch() {
    SAM_CHECK(cuda::SamGraphCudaEngine::IsInitialized() && !cuda::SamGraphCudaEngine::IsShutdown());
    return cuda::SamGraphCudaEngine::GetRandomPermutation()->num_epoch();
}

int samgraph_num_step_per_epoch() {
    SAM_CHECK(cuda::SamGraphCudaEngine::IsInitialized() && !cuda::SamGraphCudaEngine::IsShutdown());
    return cuda::SamGraphCudaEngine::GetRandomPermutation()->num_batch();
}

size_t samgraph_dataset_num_class() {
    SAM_CHECK(cuda::SamGraphCudaEngine::IsInitialized() && !cuda::SamGraphCudaEngine::IsShutdown());
    return cuda::SamGraphCudaEngine::GetGraphDataset()->num_class;
}

size_t samgraph_dataset_num_feat_dim() {
    SAM_CHECK(cuda::SamGraphCudaEngine::IsInitialized() && !cuda::SamGraphCudaEngine::IsShutdown());
    return cuda::SamGraphCudaEngine::GetGraphDataset()->feat->shape().at(1);
}

uint64_t samgraph_get_next_batch(int epoch, int step) {
    SAM_CHECK(cuda::SamGraphCudaEngine::IsInitialized() && !cuda::SamGraphCudaEngine::IsShutdown());

    uint64_t key = encodeBatchKey(epoch, step);
    SAM_LOG(DEBUG) << "samgraph_get_next_batch encodeKey with epoch " << epoch << " step " << step << " and key " << key;
    
    // RunSingleLoopOnce();

    auto graph = cuda::SamGraphCudaEngine::GetGraphPool()->GetGraphBatch(key);

    SAM_LOG(DEBUG) << "Get next batch with key " << key;
    cuda::SamGraphCudaEngine::SetGraphBatch(graph);

    return key;
}

uint64_t samgraph_get_graph_key(uint64_t batch_key, int graph_id) {
    return encodeGraphID(batch_key, graph_id);
}

size_t samgraph_get_graph_num_row(uint64_t key) {
    int layer_idx = decodeGraphID(key);

    auto batch = cuda::SamGraphCudaEngine::GetGraphBatch();

    return batch->output_graph[layer_idx]->num_row;
}

size_t samgraph_get_graph_num_col(uint64_t key) {
    int layer_idx = decodeGraphID(key);

    auto batch = cuda::SamGraphCudaEngine::GetGraphBatch();

    return batch->output_graph[layer_idx]->num_column;
}

size_t samgraph_get_graph_num_edge(uint64_t key) {
    int layer_idx = decodeGraphID(key);

    auto batch = cuda::SamGraphCudaEngine::GetGraphBatch();

    return batch->output_graph[layer_idx]->num_edge;
}

void samgraph_shutdown() {
    cuda::SamGraphCudaEngine::Shutdown();
    SAM_LOG(DEBUG) << "SamGraph has been completely shutdown now";
    return;
}

}

} // namespace common
} // namespace samgraph