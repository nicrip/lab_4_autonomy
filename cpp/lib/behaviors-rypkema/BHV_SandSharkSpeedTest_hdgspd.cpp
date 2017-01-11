/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: BHV_SandSharkSpeedTest_hdgspd.cpp               */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_SandSharkSpeedTest_hdgspd.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_SandSharkSpeedTest_hdgspd::BHV_SandSharkSpeedTest_hdgspd(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "BHV_SandSharkSpeedTest_hdgspd");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  m_startup_flag = true;
  m_run_flag = false;
  m_os_x = 0.0;
  m_os_y = 0.0;

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING, NAV_SPEED");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_SandSharkSpeedTest_hdgspd::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  bool non_neg_number = (isNumber(val) && (double_val >= 0));

  if (param == "target_x") {
    m_target_x = double_val;
    return(true);
  } else if (param == "target_y") {
    m_target_y = double_val;
    return(true);
  } else if ((param == "desired_speed") && non_neg_number) {
    m_desired_speed = double_val;
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

void BHV_SandSharkSpeedTest_hdgspd::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_SandSharkSpeedTest_hdgspd::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_SandSharkSpeedTest_hdgspd::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_SandSharkSpeedTest_hdgspd::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_SandSharkSpeedTest_hdgspd::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_SandSharkSpeedTest_hdgspd::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_SandSharkSpeedTest_hdgspd::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_SandSharkSpeedTest_hdgspd::onRunState()
{
  updateInfoIn();

  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;
  IvPFunction *hdg_ipf = 0;
  IvPFunction *spd_ipf = 0;

  if (m_startup_flag) {
    m_start_time = MOOSTime();
    m_startup_flag = false;
  }

  if (!m_run_flag) {
    if ((MOOSTime() - m_start_time) <= m_startup_time) {
      m_desired_heading = relAng(m_os_x, m_os_y, m_target_x, m_target_y);
    } else {
      m_start_time = MOOSTime();
      m_run_flag = true;
    }
  }

  if (m_run_flag) {
    if ((MOOSTime() - m_start_time) <= m_run_time) {
      // Heading
      ZAIC_PEAK hdg_zaic(m_domain, "course");
      // summit, pwidth, bwidth, delta, minutil, maxutil
      hdg_zaic.setParams(m_desired_heading, 90., 75., 20., 0., 100.);
      hdg_zaic.setValueWrap(true);
      hdg_ipf = hdg_zaic.extractIvPFunction();

      // Speed
      ZAIC_PEAK spd_zaic(m_domain, "speed");
      spd_zaic.setValueWrap(false);
      // summit, pwidth, bwidth, delta, minutil, maxutil
      spd_zaic.setParams(m_desired_speed, 0.1 * m_desired_speed, 0.3 * m_desired_speed, 50., 0., 100.);
      spd_ipf = spd_zaic.extractIvPFunction();
    } else {
      // Heading
      ZAIC_PEAK hdg_zaic(m_domain, "course");
      // summit, pwidth, bwidth, delta, minutil, maxutil
      hdg_zaic.setParams(m_desired_heading, 90., 75., 20., 0., 100.);
      hdg_zaic.setValueWrap(true);
      hdg_ipf = hdg_zaic.extractIvPFunction();

      // Speed
      ZAIC_PEAK spd_zaic(m_domain, "speed");
      spd_zaic.setValueWrap(false);
      // summit, pwidth, bwidth, delta, minutil, maxutil
      spd_zaic.setParams(0.0, 0.1, 0.3, 50., 0., 100.);
      spd_ipf = spd_zaic.extractIvPFunction();
    }
  }

  OF_Coupler coupler;
  ipf = coupler.couple(hdg_ipf, spd_ipf);

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

bool BHV_SandSharkSpeedTest_hdgspd::updateInfoIn()
{
  bool ok1, ok2;

  m_os_x = getBufferDoubleVal("NAV_X", ok1);
  m_os_y = getBufferDoubleVal("NAV_Y", ok2);
  if (!ok1 || !ok2) {
    postEMessage("No NAV_X or NAV_Y info in info_buffer.");
    return(false);
  }

  m_os_heading = getBufferDoubleVal("NAV_HEADING", ok1);
  if (!ok1) {
    postEMessage("No NAV_HEADING info in info_buffer.");
    return(false);
  }

  m_os_speed = getBufferDoubleVal("NAV_SPEED", ok1);
  if (!ok1) {
    postEMessage("No NAV_SPEED info in info_buffer.");
    return(false);
  }

  return(true);
}
