// Copyright (C) 2014 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "StateUpdaterSF/MoveDps4Pg.hh"
#include "Framework/Log.hh"
#include "Framework/MeshData.hh"
#include "Framework/PhysicsInfo.hh"
#include "SConfig/ObjectProvider.hh"

//----------------------------------------------------------------------------//

using namespace std;
using namespace SConfig;

//----------------------------------------------------------------------------//

namespace ShockFitting {

//----------------------------------------------------------------------------//

// this variable instantiation activates the self-registration mechanism
ObjectProvider<MoveDps4Pg, MoveDps> moveDpsPgProv("MoveDps4Pg");

//----------------------------------------------------------------------------//

MoveDps4Pg::MoveDps4Pg(const std::string& objectName) :
 MoveDps(objectName)
{
}

//----------------------------------------------------------------------------//

MoveDps4Pg::~MoveDps4Pg()
{
}

//----------------------------------------------------------------------------//

void MoveDps4Pg::setup()
{
  LogToScreen(VERBOSE,"MoveDps4Pg::setup() => start\n");

  LogToScreen(VERBOSE,"MoveDps4Pg::setup() => end\n");
}

//----------------------------------------------------------------------------//

void MoveDps4Pg::unsetup()
{
  LogToScreen(VERBOSE,"MoveDps4Pg::unsetup()\n");
}

//----------------------------------------------------------------------------//

void MoveDps4Pg::update()
{
  LogToScreen(INFO,"MoveDps4Pg::update()\n");

  setMeshData();

  setPhysicsData();

  setAddress();

  logfile.Open(getClassName().c_str());

  // compute the dt max
  dt = 1e39;
  for(unsigned ISH=0; ISH<(*nShocks); ISH++) {
   unsigned iShock = ISH+1;
   logfile("Shock n. ", iShock, "\n");
   for(unsigned IV=0; IV<nShockPoints->at(ISH); IV++) {
    unsigned iShockPoint = IV+1;
 
    ro = (*ZroeSh)(0,IV,ISH)*(*ZroeSh)(0,IV,ISH);
    help = pow((*ZroeSh)(2,IV,ISH),2) + pow((*ZroeSh)(3,IV,ISH),2);
    p = PhysicsInfo::getGm1()/PhysicsInfo::getGam() *
        ((*ZroeSh)(0,IV,ISH) * (*ZroeSh)(1,IV,ISH)-0.5*help);
    a = sqrt(PhysicsInfo::getGam()*p/ro);

    dum = 0;
    for(unsigned K=0; K<2; K++) { dum = dum + pow((*WSh)(K,IV,ISH),2); }
    WShMod = sqrt(dum);
    dum = 0;
    for(unsigned K=0; K<2; K++) { dum = dum + pow((*WSh)(K,IV,ISH),2); }
    WShMod = sqrt(dum);

    logfile("Shock point n. ", iShockPoint," speed: ", WShMod, "\n");

    dum = (MeshData::getInstance().getSHRELAX()) * 
          (MeshData::getInstance().getDXCELL())  *
          (MeshData::getInstance().getSNDMIN()) /(a+WShMod);

    if(dt>dum) { dt = dum; }
   }
  }

  logfile("DT max: ", dt, "\n");

  for(unsigned ISH=0; ISH<(*nShocks); ISH++) {
   unsigned iShock = ISH+1;
   logfile("Shock/Discontinuity n. ", iShock, "\n");
   for(unsigned IV=0; IV<nShockPoints->at(ISH); IV++) {
    for(unsigned I=0; I<2; I++) {
     (*XYSh)(I,IV,ISH) = (*XYSh)(I,IV,ISH) + (*WSh)(I,IV,ISH) * dt;
    }
    logfile((*XYSh)(0,IV,ISH), " ", (*XYSh)(1,IV,ISH), "\n");
   }
  }
  logfile.Close();
}

//----------------------------------------------------------------------------//

} // namespace ShockFitting