#pragma once

#include <memory>

#include "qp.hpp"

namespace rdmaio {

const int MAX_SERVER_SUPPORTED = 16;
typedef UDQP<MAX_SERVER_SUPPORTED> RUDQP;

class RdmaCtrl {
 public:
  RdmaCtrl(int node_id, int tcp_base_port,std::string ip = "localhost");
  ~RdmaCtrl();

  int current_node_id();
  int listening_port();

  typedef struct { int dev_id; int port_id; } DevIdx;

  /**
   * Query devices info on this machine,
   * if there is a previous call, return previous results unless clear_dev_info has been called
   */
  std::vector<RNicInfo> query_devs();

  // clear the cached infos by RdmaCtrl;
  void clear_dev_info();

  /**
   * Open device handlers.
   * RdmaCtrl opens a device for each thread.
   * The get_device returns previously opened device of this thread, if it is already opened
   */
  RNicHandler *open_device(DevIdx idx);

  RNicHandler *get_device();

  void close_device();

  void close_device(RNicHandler *);

  /**
   * Each RDMA NIC has multiple ports, so we use two-dimeson index to locate the target port.
   * convert_port_idx provides a way to translate the one-dimeson index to the two-dimeson
   */
  DevIdx convert_port_idx(int idx);

  /**
   * Register memory to a specific RNIC handler
   */
  bool register_memory(int id,char *buf,uint64_t size,RNicHandler *rnic,
                       int flag = Memory::DEFAULT_PROTECTION_FLAG);

  /**
   * Get the local registered memory
   * undefined if mr_id has been registered
   */
  MemoryAttr get_local_mr(int mr_id);

  /**
   * Create and query QPs
   * For create, an optional local_attr can be provided to bind to this QP
   * A local MR is passed as the default local mr for this QP.
   * If local_attr = nullptr, then this QP is unbind to any MR.
   */
  RCQP *create_rc_qp(QPIdx idx, RNicHandler *dev,MemoryAttr *local_attr = NULL);
  RUDQP *create_ud_qp(QPIdx idx, RNicHandler *dev,MemoryAttr *local_attr = NULL);

  RCQP *get_rc_qp(QPIdx idx);
  RUDQP *get_ud_qp(QPIdx idx);

  /**
   * Some helper functions (example usage of RdmaCtrl)
   * Fully link the QP in a symmetric way, for this thread.
   * For example, node 0 can connect to node 1, while node 1 connect to node 0.
   */
  bool link_symmetric_rcqps(const std::vector<std::string> &cluster,
                            int l_mrid,int mr_id,int wid,int idx = 0);

 private:
  class RdmaCtrlImpl;
  std::unique_ptr<RdmaCtrlImpl> impl_;
};
} // namespace rdmaio

#include "rdma_ctrl_impl.hpp" // real implemeatation here