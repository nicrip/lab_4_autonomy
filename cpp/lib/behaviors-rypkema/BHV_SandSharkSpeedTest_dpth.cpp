/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: BHV_SandSharkSpeedTest_dpth.cpp                 */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_SandSharkSpeedTest_dpth.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_SandSharkSpeedTest_dpth::BHV_SandSharkSpeedTest_dpth(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "BHV_SandSharkSpeedTest_dpth");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "depth");

  m_startup_flag = true;
  m_run_flag = false;

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_DEPTH");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_SandSharkSpeedTest_dpth::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  bool non_neg_number = (isNumber(val) && (double_val >= 0));

  if ((param == "desired_depth") && non_neg_number) {
    m_desired_depth = double_val;
    return(true);
  } else if ((param == "startup_time") && non_neg_number) {
    m_startup_time = double_val;
    return(true);
  } else if ((param == "run_time") && non_neg_number) {
    m_run_time = double_val;
    return(true);
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_SandSharkSpeedTest_dpth::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_SandSharkSpeedTest_dpth::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_SandSharkSpeedTest_dpth::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_SandSharkSpeedTest_dpth::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_SandSharkSpeedTest_dpth::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_SandSharkSpeedTest_dpth::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_SandSharkSpeedTest_dpth::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_SandSharkSpeedTest_dpth::onRunState()
{
  updateInfoIn();

  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  if (m_startup_flag) {
    m_start_time = MOOSTime();
    m_startup_flag = false;
  }

  if (!m_run_flag) {
    if ((MOOSTime() - m_start_time) <= m_startup_time) {
      // wait until BHV_SandSharkSpeedTest_hdgspd has finished setting up
    } else {
      m_start_time = MOOSTime();
      m_run_flag = true;
    }
  }

  if (m_run_flag) {
    if ((MOOSTime() - m_start_time) <= m_run_time) {
      // Depth
      ZAIC_PEAK spd_zaic(m_domain, "depth");
      spd_zaic.setValueWrap(false);
      // summit, pwidth, bwidth, delta, minutil, maxutil
      spd_zaic.setParams(m_desired_depth, 0.1 * m_desired_depth, 0.3 * m_desired_depth, 50., 0., 100.);
      ipf = spd_zaic.extractIvPFunction();
    } else {
      // Depth
      ZAIC_PEAK spd_zaic(m_domain, "depth");
      spd_zaic.setValueWrap(false);
      // summit, pwidth, bwidth, delta, minutil, maxutil
      spd_zaic.setParams(0.0, 0.1, 0.3, 50., 0., 100.);
      ipf = spd_zaic.extractIvPFunction();
    }
  }

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

//---------------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: update the info from MOOSDB vars.

bool BHV_SandSharkSpeedTest_dpth::updateInfoIn()
{
  bool ok1, ok2;

  m_os_depth = getBufferDoubleVal("NAV_DEPTH", ok1);
  if (!ok1) {
    postEMessage("No NAV_DEPTH info in info_buffer.");
    return(false);
  }

  return(true);
}
