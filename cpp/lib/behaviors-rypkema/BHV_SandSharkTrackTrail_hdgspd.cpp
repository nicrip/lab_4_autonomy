/************************************************************/
/*    NAME: Nick Rypkema                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_SandSharkTrackTrail_hdgspd.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_SandSharkTrackTrail_hdgspd.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include <cmath>

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_SandSharkTrackTrail_hdgspd::BHV_SandSharkTrackTrail_hdgspd(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "BHV_SandSharkTrackTrail_hdgspd");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  m_activated = false;
  m_running = false;
  m_read_count = 0;
  m_cbf_count = -1;
  m_azimuth_2 = 0;
  m_azimuth_3 = 0;
  m_range_2 = 0;
  m_range_3 = 0;

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_HEADING, NAV_DEPTH, CBF_COUNT, CBF_RANGE, CBF_RANGE_VARIANCE, CBF_AZIMUTH_DEG");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_SandSharkTrackTrail_hdgspd::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  bool non_neg_number = (isNumber(val) && (double_val >= 0));

  if ((param == "desired_speed") && non_neg_number) {
    m_desired_speed = double_val;
    return(true);
  } else if ((param == "run_time") && non_neg_number) {
    m_run_time = double_val;
    return(true);
  } else if ((param == "activation_depth") && non_neg_number) {
    m_activation_depth = double_val;
    return(true);
  } else if ((param == "activation_azimuth_tol") && non_neg_number) {
    m_activation_azimuth_tol = double_val;
    return(true);
  } else if ((param == "activation_range_tol") && non_neg_number) {
    m_activation_range_tol = double_val;
    return(true);
  } else if ((param == "deactivation_range") && non_neg_number) {
    m_deactivation_range = double_val;
    return(true);
  } else if ((param == "heading_upper_bound") && non_neg_number) {
    m_heading_upper_bound = double_val;
    return(true);
  } else if ((param == "heading_lower_bound") && non_neg_number) {
    m_heading_lower_bound = double_val;
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

void BHV_SandSharkTrackTrail_hdgspd::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_SandSharkTrackTrail_hdgspd::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_SandSharkTrackTrail_hdgspd::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_SandSharkTrackTrail_hdgspd::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_SandSharkTrackTrail_hdgspd::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_SandSharkTrackTrail_hdgspd::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_SandSharkTrackTrail_hdgspd::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_SandSharkTrackTrail_hdgspd::onRunState()
{
  updateInfoIn();

  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;
  IvPFunction *hdg_ipf = 0;
  IvPFunction *spd_ipf = 0;
  // Heading
  ZAIC_PEAK hdg_zaic(m_domain, "course");
  // summit, pwidth, bwidth, delta, minutil, maxutil
  hdg_zaic.setParams(0., 90., 75., 20., 0., 100.);
  hdg_zaic.setValueWrap(true);
  hdg_ipf = hdg_zaic.extractIvPFunction();

  // Speed
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setValueWrap(false);
  // summit, pwidth, bwidth, delta, minutil, maxutil
  spd_zaic.setParams(0., 0.1, 0.3, 50., 0., 100.);
  spd_ipf = spd_zaic.extractIvPFunction();

  // only activate behavior if we are below the activation depth (hold the vehicle under)
  if (m_os_depth >= m_activation_depth) {
    m_activated = true;
  } else {
    m_activated = false;
  }

  if (m_activated) {
    if (!m_running && (m_read_count==3)) {
      double max_range_diff = 0;
      double max_azimuth_diff = 0;
      double range_diff, azimuth_diff;

      range_diff = std::fabs(m_range_1-m_range_2);
      if (range_diff > max_range_diff) {
        max_range_diff = range_diff;
      }
      range_diff = std::fabs(m_range_2-m_range_3);
      if (range_diff > max_range_diff) {
        max_range_diff = range_diff;
      }
      range_diff = std::fabs(m_range_3-m_range_1);
      if (range_diff > max_range_diff) {
        max_range_diff = range_diff;
      }

      azimuth_diff = 180.0 - std::fabs(std::fmod(std::fabs(m_azimuth_1 - m_azimuth_2), 360.0) - 180.0);
      if (azimuth_diff > max_azimuth_diff) {
        max_azimuth_diff = azimuth_diff;
      }
      azimuth_diff = 180.0 - std::fabs(std::fmod(std::fabs(m_azimuth_2 - m_azimuth_3), 360.0) - 180.0);
      if (azimuth_diff > max_azimuth_diff) {
        max_azimuth_diff = azimuth_diff;
      }
      azimuth_diff = 180.0 - std::fabs(std::fmod(std::fabs(m_azimuth_3 - m_azimuth_1), 360.0) - 180.0);
      if (azimuth_diff > max_azimuth_diff) {
        max_azimuth_diff = azimuth_diff;
      }

      if ((max_range_diff <= m_activation_range_tol) && (max_range_diff <= m_activation_range_tol)) {
        m_running = true;
        m_start_time = MOOSTime();
      }
    }

    if (m_running) {
      if ((MOOSTime() - m_start_time) <= m_run_time) {
        if (m_range_3 > m_deactivation_range) {
          // Speed
          ZAIC_PEAK spd_zaic(m_domain, "speed");
          spd_zaic.setValueWrap(false);
          // summit, pwidth, bwidth, delta, minutil, maxutil
          spd_zaic.setParams(m_desired_speed, 0.1 * m_desired_speed, 0.3 * m_desired_speed, 50., 0., 100.);
          spd_ipf = spd_zaic.extractIvPFunction();

          if (m_azimuth_3 <= 90.0) {
            m_desired_heading = std::fmod((m_os_heading - m_azimuth_3), 360.0);
          } else if (m_azimuth_3 >= 270.0) {
            m_desired_heading = std::fmod((m_os_heading + (360.0 - m_azimuth_3)), 360.0);
          } else {
            m_desired_heading = m_os_heading;
          }

          if (!containsAngle(m_heading_lower_bound, m_heading_upper_bound, m_desired_heading)) {
            m_desired_heading = m_os_heading;
          }

          // Heading
          ZAIC_PEAK hdg_zaic(m_domain, "course");
          // summit, pwidth, bwidth, delta, minutil, maxutil
          hdg_zaic.setParams(m_desired_heading, 90., 75., 20., 0., 100.);
          hdg_zaic.setValueWrap(true);
          hdg_ipf = hdg_zaic.extractIvPFunction();
        }
      }
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

bool BHV_SandSharkTrackTrail_hdgspd::updateInfoIn()
{
  bool ok1;

  m_os_heading = getBufferDoubleVal("NAV_HEADING", ok1);
  if (!ok1) {
    postEMessage("No NAV_HEADING info in info_buffer.");
    return(false);
  }

  m_os_depth = getBufferDoubleVal("NAV_DEPTH", ok1);
  if (!ok1) {
    postEMessage("No NAV_DEPTH info in info_buffer.");
    return(false);
  }

  double cbf_count = getBufferDoubleVal("CBF_COUNT", ok1);
  if (!ok1) {
    postWMessage("No CBF_COUNT info in info_buffer.");
    return(false);
  }

  if (cbf_count != m_cbf_count) {
    m_cbf_count = cbf_count;

    m_range_variance = getBufferDoubleVal("CBF_RANGE_VARIANCE", ok1);
    if (!ok1) {
      postWMessage("No CBF_RANGE_VARIANCE info in info_buffer.");
      return(false);
    }

    if (m_range_variance <= 4.0) {
      double cbf_azimuth_deg = getBufferDoubleVal("CBF_AZIMUTH_DEG", ok1);
      if (!ok1) {
        postWMessage("No CBF_AZIMUTH_DEG info in info_buffer.");
        return(false);
      }

      double cbf_range = getBufferDoubleVal("CBF_RANGE", ok1);
      if (!ok1) {
        postWMessage("No CBF_RANGE info in info_buffer.");
        return(false);
      }

      if (cbf_azimuth_deg <= 90 || cbf_azimuth_deg >= 270) {
        if (m_read_count < 3) {
          m_read_count = m_read_count + 1;
        }

        m_range_1 = m_range_2;
        m_range_2 = m_range_3;
        m_range_3 = cbf_range;

        m_azimuth_1 = m_azimuth_2;
        m_azimuth_2 = m_azimuth_3;
        m_azimuth_3 = cbf_azimuth_deg;
      }
    }
  }

  return(true);
}
