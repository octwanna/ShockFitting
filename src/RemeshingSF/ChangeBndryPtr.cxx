// Copyright (C) 2014 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "RemeshingSF/ChangeBndryPtr.hh"
#include "Framework/Log.hh"
#include "Framework/MeshData.hh"
#include "MathTools/Binsrc.hh"
#include "SConfig/ObjectProvider.hh"

//--------------------------------------------------------------------------//

using namespace std;
using namespace SConfig;

//--------------------------------------------------------------------------//

namespace ShockFitting {

//--------------------------------------------------------------------------//

// this variable instantiation activates the self-registration mechanism
ObjectProvider<ChangeBndryPtr, Remeshing>
changeBndryPtrProv("ChangeBndryPtr");

//--------------------------------------------------------------------------//

ChangeBndryPtr::ChangeBndryPtr(const std::string& objectName) :
  Remeshing(objectName)
{
}

//--------------------------------------------------------------------------//

ChangeBndryPtr::~ChangeBndryPtr()
{
}

//--------------------------------------------------------------------------//

void ChangeBndryPtr::setup()
{
  LogToScreen (VERBOSE, "ChangeBndryPtr::setup => start\n");

  logfile.Open(getClassName());

  LogToScreen (VERBOSE, "ChangeBndryPtr::setup => end\n");
}

//----------------------------------------------------------------------------//

void ChangeBndryPtr::unsetup()
{
  logfile.Close();

  LogToScreen (VERBOSE, "ChangeBndryPtr::unsetup()\n");
}

//----------------------------------------------------------------------------//

void ChangeBndryPtr::remesh()
{
  LogToScreen (INFO, "ChangeBndryPtr::remesh() \n");

  setMeshData();

  logfile("Subr ChangeBndryPtr; NBFAC was = ",(*nbfac),"\n");
  logfile("Subr ChangeBndryPtr; NBPOIN was = ",(*nbpoin),"\n");
  logfile("Subr ChangeBndryPtr; NPOIN was = ",(*npoin),"\n");


  for (int IPOIN=0; IPOIN<(*npoin); IPOIN++) {

   if (nodcod->at(IPOIN)==-2) {

    // search the i-node IPOIN in inodptr vector if the node has been
    // dis-actived (nodcod=-2)
    lookForNode(IPOIN);


    // remove the i-node
    unsigned ipoin = IPOIN+1; //c++indeces start from 0
    logfile("Removing node", ipoin, "belongs to edges: ");
    logfile((*nodptr)(ipos,1) , (*nodptr)(ipos,2), "\n");
    removeNode(IPOIN);


    // update the i-face=nodptr(ipos,2)
    updateIface();

    // remove the i-face=nodptr(ipos,3)
    removeIface();

   }
  }

  logfile("Subr ChangeBndryPtr; NBFAC is now = ",(*nbfac),"\n");
}

//----------------------------------------------------------------------------//

void ChangeBndryPtr::lookForNode(int IPOIN)
{
  vector <int> iwork_nodptr;
  iwork_nodptr.resize((*nbpoin));

  for (unsigned i=0; i < (*nbpoin); i++) {
   iwork_nodptr.at(i) = (*nodptr)(i,0);
  }

  unsigned ipoin = IPOIN+1; //c++ indeces start from 0

  Binsrc findIpoin(ipoin,iwork_nodptr);
  ipos = findIpoin.callBinsrc();
  if (ipos==-1) {
   logfile("Entry NOT found for ", ipoin, "\n");
   logfile("Entry NOT found for ", ipoin, "\n");
   exit(1);
  }
}

//----------------------------------------------------------------------------//

void ChangeBndryPtr::removeNode(int IPOIN)
{
  inode.resize(2);
  for (unsigned K=1; K<3; K++) {
   iface = (*nodptr)(ipos,K);
   if ((*bndfac)(0,iface-1) != IPOIN+1) // c++ indeces start from 0
      { inode.at(K-1) = (*bndfac)(0,iface-1); }
   else { inode.at(K-1) = (*bndfac)(1,iface-1); } // c++ indeces start from 0
  }

}

//----------------------------------------------------------------------------//

void ChangeBndryPtr::updateIface()
{
  iface = (*nodptr)(ipos,1);
  (*bndfac)(0,iface-1)=inode.at(0); //c++ indeces start from 0
  (*bndfac)(1,iface-1)=inode.at(1); //c++ indeces start from 0
  logfile("Face: ",iface, "has been updated with: ");
  logfile(inode.at(0)," ",inode.at(1),"\n");
}

//----------------------------------------------------------------------------//

void ChangeBndryPtr::removeIface()
{
  iface = (*nodptr)(ipos,2);
  logfile("Face: ", iface, "has been removed\n");
  (*bndfac)(0,iface-1) = (*bndfac)(0,iface-1); // c++ indeces start from 0
  (*bndfac)(1,iface-1) = (*bndfac)(1,iface-1); // c++ indeces start from 0
  (*bndfac)(2,iface-1) = -(*bndfac)(2,iface-1); // c++ indeces start from 0
}

//----------------------------------------------------------------------------//

void ChangeBndryPtr::setMeshData()
{
  npoin = MeshData::getInstance().getData <unsigned> ("NPOIN");
  nbpoin = MeshData::getInstance().getData <unsigned> ("NBPOIN");
  nbfac = MeshData::getInstance().getData <unsigned> ("NBFAC");
  nodcod = MeshData::getInstance().getData< std::vector<int> >("NODCOD");
  nodptr = MeshData::getInstance().getData< Array2D<int> >("NODPTR");
  bndfac = MeshData::getInstance().getData< Array2D<int> >("BNDFAC");
}

//----------------------------------------------------------------------------//

} // namespace ShockFitting


