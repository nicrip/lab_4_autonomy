/************************************************************/
/*    NAME: Nick Rypkema                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_SandSharkTrackTrail_hdgspd.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef SandSharkTrackTrail_hdgspd_HEADER
#define SandSharkTrackTrail_hdgspd_HEADER

#include <string>
#include "IvPBehavior.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "MOOS/libMOOS/MOOSLib.h"

class BHV_SandSharkTrackTrail_hdgspd : public IvPBehavior {
public:
  BHV_SandSharkTrackTrail_hdgspd(IvPDomain);
  ~BHV_SandSharkTrackTrail_hdgspd() {};

  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();

protected: // Local Utility functions
  bool         updateInfoIn();

protected: // Configuration parameters
  double      m_desired_speed;
  double      m_run_time;
  double      m_activation_depth;
  double      m_activation_azimuth_tol;
  double      m_activation_range_tol;
  double      m_deactivation_range;
  double      m_heading_upper_bound;
  double      m_heading_lower_bound;

protected: // State variables
  double      m_os_heading;
  double      m_os_depth;
  double      m_desired_heading;
  double      m_start_time;
  double      m_azimuth_1;
  double      m_azimuth_2;
  double      m_azimuth_3;
  double      m_range_1;
  double      m_range_2;
  double      m_range_3;
  double      m_range_variance;
  double      m_cbf_count;
  int         m_read_count;
  bool        m_activated;
  bool        m_running;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain)
  {return new BHV_SandSharkTrackTrail_hdgspd(domain);}
}
#endif
