#ifndef SAMGRAPH_CPU_SIMPLE_HASHTABLE_H
#define SAMGRAPH_CPU_SIMPLE_HASHTABLE_H

#include <parallel_hashmap/phmap.h>

#include "cpu_hashtable.h"

namespace samgraph {
namespace common {
namespace cpu {

class SimpleHashTable : public HashTable {
 public:
  SimpleHashTable(size_t sz);
  ~SimpleHashTable();

  void Populate(const IdType *input, const size_t num_input) override;
  void MapNodes(IdType *output, size_t num_output) override;
  void MapEdges(const IdType *src, const IdType *dst, const size_t len,
                IdType *new_src, IdType *new_dst) override;
  void Reset() override;
  size_t NumItems() const override { return _num_items; }

 private:
  struct Bucket1 {
    IdType global;
  };

  phmap::flat_hash_map<IdType, IdType> _o2n_table;
  Bucket1 *_n2o_table;
  size_t _num_items;
};

}  // namespace cpu
}  // namespace common
}  // namespace samgraph

#endif  // SAMGRAPH_CPU_SIMPLE_HASHTABLE_H
