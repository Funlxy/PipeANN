// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <omp.h>
#include <cstring>
#include <ctime>
#include <timer.h>
#include "log.h"

#include "pq_flash_index.h"
#include "utils.h"

#ifndef _WINDOWS
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Correct usage: " << argv[0] << " <base_partition_file>" << std::endl;
    exit(-1);
  }

  int arg_no = 1;
  char *partition_file = argv[arg_no++];
  std::string out_partition_file = std::string(partition_file) + ".aligned";

  std::ifstream part(partition_file);
  std::ofstream out_part(out_partition_file.data(), std::ios::binary);
  _u64 C, partition_nums, nd;
  part.read((char *) &C, sizeof(_u64));
  part.read((char *) &partition_nums, sizeof(_u64));
  part.read((char *) &nd, sizeof(_u64));
  out_part.write((char *) &C, sizeof(_u64));
  out_part.write((char *) &partition_nums, sizeof(_u64));
  out_part.write((char *) &nd, sizeof(_u64));

  uint32_t tmp_arr[4096];
  for (unsigned i = 0; i < partition_nums; i++) {
    unsigned s;
    part.read((char *) &s, sizeof(unsigned));
    // LOG(INFO) << "Partition " << i << " size " << s;
    // Normally: partition_nums (200000) * C (5) for SIFT1M
    part.read((char *) tmp_arr, sizeof(unsigned) * s);
    for (uint32_t j = s; j < C; ++j) {
      tmp_arr[j] = diskann::PQFlashIndex<uint8_t>::kInvalidID;
    }
    out_part.write((char *) &s, sizeof(unsigned));
    out_part.write((char *) tmp_arr, sizeof(unsigned) * C);
  }
}