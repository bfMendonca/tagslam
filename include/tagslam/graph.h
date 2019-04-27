/* -*-c++-*--------------------------------------------------------------------
 * 2019 Bernd Pfrommer bernd.pfrommer@gmail.com
 */

#include "tagslam/boost_graph.h"
#include "tagslam/vertex_desc.h"
#include "tagslam/optimizer.h"
#include "tagslam/pose_with_noise.h"
#include "tagslam/geometry.h"
#include "tagslam/vertex.h"
#include "tagslam/value/pose.h"
#include "tagslam/factor/tag_projection.h"
#include "tagslam/factor/absolute_pose_prior.h"
#include "tagslam/factor/relative_pose_prior.h"
#include <ros/ros.h>

#include <climits>
#include <map>
#include <unordered_map>
#include <memory>

#pragma once

namespace tagslam {
  class Graph {
    using string = std::string;
  public:
    Graph();
    typedef std::multimap<double, VertexDesc> ErrorToVertexMap;
    
    bool hasId(const VertexId &id) const { return (idToVertex_.count(id)!=0);}
    bool hasPose(const ros::Time &t, const std::string &name) const;
    bool isOptimized(const VertexDesc &v) const {
      return (optimized_.find(v) != optimized_.end());
    }
    string info(const VertexDesc &v) const;
    double optimize(double thresh);
    double optimizeFull(bool force = false);

    const VertexVec &getFactors() const { return (factors_); }
    VertexVec getConnected(const VertexDesc &v) const;

    VertexDesc add(const PoseValuePtr &p);
    VertexDesc add(const RelativePosePriorFactorPtr &p);
    VertexDesc add(const AbsolutePosePriorFactorPtr &p);
    VertexDesc add(const TagProjectionFactorPtr &p);
    
    OptimizerKey addToOptimizer(const VertexDesc &v, const Transform &tf);
    OptimizerKey addToOptimizer(const factor::RelativePosePrior *p);
    OptimizerKey addToOptimizer(const factor::AbsolutePosePrior *p);
    std::vector<OptimizerKey> addToOptimizer(const factor::TagProjection *p);

    VertexDesc addPose(const ros::Time &t, const string &name,
                       bool isCamPose = false);
  
    Transform getOptimizedPose(const VertexDesc &v) const;
    inline Transform pose(const VertexDesc &v) const {
      return (getOptimizedPose(v)); }
    PoseNoise2 getPoseNoise(const VertexDesc &v) const;

    // for debugging, compute error on graph
    double    getError() { return (optimizer_->errorFull()); }
    double    getMaxError() { return (optimizer_->getMaxError()); }
    void      plotDebug(const ros::Time &t, const string &tag);
    void      transferFullOptimization() {
      optimizer_->transferFullOptimization(); }
    void setVerbosity(const string &v) {
      optimizer_->setVerbosity(v);
    }
    const BoostGraph &getBoostGraph() const { return (graph_); }

    void  copyFrom(const Graph &g, const std::deque<VertexDesc> &vsrc);
    void  initializeFrom(const Graph &sg);
    void  print(const std::string &pre = "") const;
    std::string getStats() const;
    
    VertexPtr getVertex(const VertexDesc f) const { return (graph_[f]); }
    VertexPtr operator[](const VertexDesc f) const { return (graph_[f]); }

    VertexDesc findPose(const ros::Time &t, const string &name) const;
    ErrorToVertexMap getErrorMap() const;
    // static methods
    static string tag_name(int tagid);
    static string body_name(const string &body);
    static string cam_name(const string &cam);
    inline static bool is_valid(const VertexDesc &v) {
      return (v != ULONG_MAX);
    }
    void printUnoptimized() const;
  private:
    typedef std::unordered_map<VertexId, VertexDesc> IdToVertexMap;
    typedef std::unordered_map<VertexDesc,
                               std::vector<OptimizerKey>> VertexToOptMap;
    inline VertexDesc find(const VertexId &id) const {
      const auto it = idToVertex_.find(id);
      return (it == idToVertex_.end() ? ULONG_MAX : it->second);
    }
    VertexDesc find(const Vertex *vp) const;
    VertexToOptMap::const_iterator findOptimized(const VertexDesc &v) const;
    void verifyUnoptimized(const VertexDesc &v) const;
    ValueKey findOptimizedPoseKey(const VertexDesc &v) const;
    VertexDesc insertVertex(const VertexPtr &vp);
    std::vector<ValueKey> getOptKeysForFactor(VertexDesc fv, int nk) const;

    // ------ variables --------------
    BoostGraph                 graph_;
    VertexVec                  factors_;
    IdToVertexMap              idToVertex_;
    VertexToOptMap             optimized_;
    std::shared_ptr<Optimizer> optimizer_;
  };
  typedef std::shared_ptr<Graph> GraphPtr;
  typedef std::shared_ptr<const Graph> GraphConstPtr;
}
