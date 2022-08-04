#pragma once
#include "gqlite.h"
#include <vector>
#include <string>
#include <functional>
#include "StorageEngine.h"

class GVirtualNetwork;
class GStorageEngine;
typedef int (*gqlite_callback)(_gqlite_result*);

void init_result_info(gqlite_result& result, const std::vector<std::string>& info);
void release_result_info(gqlite_result& result);
void init_result_nodes(gqlite_result& result);
void release_result_nodes(gqlite_result& result);

/**
 * Control plan execute status.
 */
enum class ExecuteStatus {
  Continue,
  Stop
};

class GPlan {
public:
  GPlan(GVirtualNetwork* network, GStorageEngine* store);
  virtual ~GPlan();
  
  /** After Plan created, before Plan execute,
   * the prepare function will be called for validating preliminary.
   * For example: before update execute, we should check database created or not.
   */
  virtual int prepare() { return 0; }
  virtual int execute(const std::function<ExecuteStatus(KeyType, const std::string& key, const std::string& value)>& processor) = 0;
  /**
   * Try to interrupt plan when it still working
   */
  virtual int interrupt() { return 0; }
  void addLeft(GPlan* plan) { _left = plan; }

  inline GPlan* left() { return _left; }
protected:
  GVirtualNetwork* _network;
  GStorageEngine* _store;
  GPlan* _left;
  GPlan* _right;
};
