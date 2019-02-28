/* -*-c++-*--------------------------------------------------------------------
 * 2018 Bernd Pfrommer bernd.pfrommer@gmail.com
 */

#include "tagslam/pose_noise2.h"

namespace tagslam {
  PoseNoise2 PoseNoise2::make(const Point3d &a, const Point3d &p) {
    Matrix6d m = Matrix6d::Zero();
    m.diagonal() << a(0),a(1),a(2),p(0),p(1),p(2);
    return (PoseNoise2(m, true));
  }

  PoseNoise2 PoseNoise2::make(double a, double p) {
    return (make(Point3d(a, a, a), Point3d(p, p, p)));
  }
  Eigen::Matrix<double, 6, 1> PoseNoise2::getDiagonal() const {
    return (noise.diagonal());
  }

  std::ostream &operator<<(std::ostream &os, const PoseNoise2 &pn) {
    os << "is_diagonal: " << pn.isDiagonal << std::endl << pn.noise;
    return (os);
  }
    
}  // namespace