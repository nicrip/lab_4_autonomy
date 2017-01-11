/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: BHV_SandSharkSpeedTest_dpth.h                   */
/*    DATE:                                                 */
/************************************************************/

#ifndef SandSharkSpeedTest_dpth_HEADER
#define SandSharkSpeedTest_dpth_HEADER

#include <string>
#include "IvPBehavior.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "MOOS/libMOOS/MOOSLib.h"

class BHV_SandSharkSpeedTest_dpth : public IvPBehavior {
public:
  BHV_SandSharkSpeedTest_dpth(IvPDomain);
  ~BHV_SandSharkSpeedTest_dpth() {};

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
  double      m_desired_depth;
  double      m_startup_time;
  double      m_run_time;

protected: // State variables
  double      m_os_depth;
  bool        m_startup_flag;
  bool        m_run_flag;
  double      m_start_time;
  double      m_curr_time;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain)
  {return new BHV_SandSharkSpeedTest_dpth(domain);}
}
#endif
