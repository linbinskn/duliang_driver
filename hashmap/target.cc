#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <set>
#include <algorithm>
#include <sys/stat.h>
#include <vector>

#include "bitmap_hashmap.h"
#include "probing_hashmap.h"
#include "tombstone_hashmap.h"
#include "backshift_hashmap.h"
#include "bitmap_hashmap.h"
#include "shadow_hashmap.h"


std::string concatenate(std::string const& str, int i)
{
    std::stringstream s;
    s << str << i;
    return s.str();
}

 
uint32_t NearestPowerOfTwo(const uint32_t number)	{
  uint32_t power = 1;
  while (power < number) {
    power <<= 1;
  }
  return power;
}


int exists_or_mkdir(const char *path) {
  struct stat sb;

  if (stat(path, &sb) == 0) {
    if (!S_ISDIR(sb.st_mode)) {
      return 1;
    }
  } else if (mkdir(path, 0777) != 0) {
    return 1;
  }

  return 0;
}


void show_usage() {
  fprintf(stdout, "Test program for implementations of open addressing hash table algorithms.\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "General parameters (mandatory):\n");
  fprintf(stdout, " --algo            algorithm to use for the hash table. Possible values are:\n");
  fprintf(stdout, "                     * linear: basic linear probing\n");
  fprintf(stdout, "                     * tombstone: Robin Hood hashing with tombstone deletion\n");
  fprintf(stdout, "                     * backshift: Robin Hood hashing with backward shifting deletion\n");
  fprintf(stdout, "                     * bitmap: hopscotch hashing with bitmap representation\n");
  fprintf(stdout, "                     * shadow: hopscotch hashing with shadow representation\n");
  fprintf(stdout, " --testcase        test case to use. Possible values are:\n");
  fprintf(stdout, "                     * loading: load the table until it is full (does not perform any removals).\n");
  fprintf(stdout, "                     * batch: load the table, then remove a large batch, and re-insert a large batch.\n");
  fprintf(stdout, "                     * ripple: load the table, then do a series of removal-insertion operations.\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "Parameters for linear probing algorithm (optional):\n");
  fprintf(stdout, " --num_buckets     number of buckets in the hash table (default=10000)\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "Parameters for tombstone algorithm (optional):\n");
  fprintf(stdout, " --num_buckets     number of buckets in the hash table (default=10000)\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "Parameters for backshift algorithm (optional):\n");
  fprintf(stdout, " --num_buckets     number of buckets in the hash table (default=10000)\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "Parameters for bitmap algorithm (optional):\n");
  fprintf(stdout, " --num_buckets     number of buckets in the hash table (default=10000)\n");
  fprintf(stdout, " --size_probing    maximum number of buckets used in the probing (default=4096)\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "Parameters for shadow algorithm (optional):\n");
  fprintf(stdout, " --num_buckets     number of buckets in the hash table (default=10000)\n");
  fprintf(stdout, " --size_probing    maximum number of buckets used in the probing (default=4096)\n");
  fprintf(stdout, " --size_nh_start   starting size of the neighborhoods (default=32)\n");
  fprintf(stdout, " --size_nh_end     ending size of the neighborhoods (default=32)\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "Parameters for the batch test case (optional):\n");
  fprintf(stdout, " --load_factor_max   maxium load factor at which the table should be used (default=.7)\n");
  fprintf(stdout, " --load_factor_step  load factor by which items in the table should be removed and inserted (default=.1)\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "Parameters for the ripple test case (optional):\n");
  fprintf(stdout, " --load_factor_max   maxium load factor at which the table should be used (default=.7)\n");
  fprintf(stdout, " --load_factor_step  load factor by which items in the table should be removed and inserted (default=.1)\n");
  fprintf(stdout, "\n");

  fprintf(stdout, "Examples:\n");
  fprintf(stdout, "./hashmap --algo backshift --num_buckets 10000 --testcase batch --load_factor_max 0.8 --load_factor_step 0.1\n");
  fprintf(stdout, "./hashmap --algo shadow --num_buckets 10000 --size_nh_start 4 --size_nh_end 64 --testcase loading\n");
}


hashmap::HashMap* get_hashmap(std::string algorithm, int num_items=10000, uint32_t size_probing=4096, uint32_t size_neighborhood_start=32, 
uint32_t size_neighborhood_end=32)
{
  hashmap::HashMap* hm;
  if (algorithm == "bitmap") {
    hm = new hashmap::BitmapHashMap(num_items, size_probing);
  } else if (algorithm == "shadow") {
    hm = new hashmap::ShadowHashMap(num_items, size_probing, size_neighborhood_start, size_neighborhood_end);
  } else if (algorithm == "linear") {
    hm = new hashmap::ProbingHashMap(num_items, size_probing);
  } else if (algorithm == "tombstone") {
    hm = new hashmap::TombstoneHashMap(num_items);
  } else if (algorithm == "backshift") {
    hm = new hashmap::BackshiftHashMap(num_items);
  }

  return hm;
}


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Size == 0) { return 0; }
  bool has_error;
  std::string str((const char*)Data, Size);
  std::stringstream ss(str);
  std::vector<std::string> datas;
  std::string algorithms[5] = {"bitmap", "shadow", "linear", "tombstone", "backshift"};

  while (ss) {
    std::string tmp;
    ss >> tmp;
    datas.push_back(tmp);
  }

  int msize = datas.size();
  if (msize % 2 == 1) { msize -= 1; }
  if (msize == 0) { return 0; }

  uint32_t size_neighborhood_start = 32;
  uint32_t size_neighborhood_end = 32;
  uint32_t size_probing = 4096;
  uint32_t num_buckets = 10000;
  double load_factor_max = 0.7;
  double load_factor_step = 0.1;
  
  int num_items = num_buckets;

  for (int ind = 0; ind < 5; ind++) {
    hashmap::HashMap *hm;
    hm = get_hashmap(algorithms[ind], num_items, size_probing, size_neighborhood_start, size_neighborhood_end);
    hm->Open();
    std::string value_out("value_out");
    int num_items_reached = 0;

    for (int i = 0; i < msize; i+=2) {
      value_out = "value_out";
      std::string key = concatenate( datas[i], i);
      std::string value = concatenate( datas[i+1], i+1 );
      int ret_put = hm->Put(key, value);
      hm->Get(key, &value_out);
    }

    delete hm;


    hashmap::HashMap *hm2;
    hm2 = get_hashmap(algorithms[ind], num_items, size_probing, size_neighborhood_start, size_neighborhood_end);
    hm2->Open();
    has_error = false;
    for (int i = 0; i < msize; i+=2) {
      value_out = "value_out";
      std::string key = concatenate( datas[i], i );
      std::string value = concatenate( datas[i+1], i+1 );
      int ret_get = hm2->Get(key, &value_out);
      if (ret_get != 0 || value != value_out) {
        std::cout << "Final check: error at step [" << i << "]" << std::endl; 
        has_error = true;
        break;
      }
    }

    if (!has_error) {
        std::cout << "Final check: OK" << std::endl; 
    }
    delete hm2;

    hashmap::HashMap *hm3;
    hm3 = new hashmap::BitmapHashMap(num_items, size_probing);
    // hm3 = get_hashmap(algorithms[ind], num_items, size_probing, size_neighborhood_start, size_neighborhood_end);
    hm3->Open();
    has_error = false;
    for (int i = 0; i < msize; i++) {
      std::string key = concatenate( datas[i], i );
      std::string value = concatenate( datas[i+1], i+1 );
      int ret_remove = hm3->Remove(key);
      if (ret_remove != 0) {
        std::cout << "Remove: error at step [" << i << "]" << std::endl; 
        has_error = true;
        break;
      }
      int ret_get = hm3->Get(key, &value_out);
      if (ret_get == 0) {
        std::cout << "Remove: error at step [" << i << "] -- can get after remove" << std::endl; 
        has_error = true;
        break;
      }
    }

    if (!has_error) {
        std::cout << "Removing items: OK" << std::endl; 
    }
    delete hm3;
  }

  return 0;
}
