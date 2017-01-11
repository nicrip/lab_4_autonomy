/************************************************************/
/*    NAME: Nick Rypkema                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_SandSharkTrackTrail_dpth.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef SandSharkTrackTrail_dpth_HEADER
#define SandSharkTrackTrail_dpth_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_SandSharkTrackTrail_dpth : public IvPBehavior {
public:
  BHV_SandSharkTrackTrail_dpth(IvPDomain);
  ~BHV_SandSharkTrackTrail_dpth() {};
  
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

protected: // Configuration parameters

protected: // State variables
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_SandSharkTrackTrail_dpth(domain);}
}
#endif
