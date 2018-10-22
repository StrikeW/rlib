#pragma once

#include <string>
#include <set>
#include <functional>

#include "common.hpp"

namespace rdmaio {

typedef std::function<void(char *,int,int)> msg_callback_t_;

/**
 * An abstract message interface
 * Assumption: one per thread
 */
class MsgAdapter {
 public:

  MsgAdapter(msg_callback_t_ callback)
      : callback_(callback) {
  }
  virtual ConnStatus connect(std::string ip,int port) = 0;

  /**
   * Basic send interfaces
   */
  virtual ConnStatus send_to(int node_id,char *msg,int len) = 0;

  virtual ConnStatus send_to(int node_id,int tid,char *msg,int len) {
    return send_to(node_id,msg,len);
  }

  /**
   * Interfaces which allow batching at the sender's side
   */
  virtual void prepare_pending() {
  }

  virtual ConnStatus send_pending(int node_id,char *msg,int len) {
    return send_to(node_id,msg,len);
  }

  virtual ConnStatus send_pending(int node_id,int tid,char *msg,int len) {
    return send_to(node_id,tid,msg,len);
  }

  /**
   * Flush all the currently pended message
   */
  virtual ConnStatus flush_pending() {
    return SUCC;
  }

  /**
   * Examples to use batching at the sender side
   * Broadcast the message to a set of servers
   */
  virtual ConnStatus broadcast_to(const std::set<int> &nodes, char *msg,int len) {
    prepare_pending();
    for(auto it = nodes.begin(); it != nodes.end(); ++it) {
      send_pending(*it,msg,len);
    }
    flush_pending();
    return SUCC; // TODO
  }

  virtual ConnStatus broadcast_to(int *nodes,int num, char *msg,int len) {
    prepare_pending();
    for(int i = 0;i < num;++i) {
      send_pending(nodes[i],msg,len);
    }
    flush_pending();
    return SUCC;  // TODO
  }

  /**
   * The receive function
   */
  virtual void poll_comps() = 0;

  /**
   * The size of meta data used by the MsgAdapter for each message
   */
  virtual int msg_meta_len() {
    return 0;
  }

 protected:
  msg_callback_t_ callback_;
};

};
