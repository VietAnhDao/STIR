//
//
/*
    Copyright (C) 2002- 2007, Hammersmith Imanet Ltd
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/
/*!
  \file
  \brief Declaration of class stir::RootDetectorMap
  \author Viet Dao
*/


#ifndef __stir_RootDetectorMap_H__
#define __stir_RootDetectorMap_H__

#include <fstream>
#include <string>
#include <vector>
#include <stir/extend_boost.h>

START_NAMESPACE_STIR

class Succeeded;

/*!   \ingroup root indicies to ring + detector map e.g. {rsectorID, moduleID, submoduleID, detectorID, layerID} -> {ringID, detectorID}
 \brief
 This code is a generic implementation of unordered map between two vectors of known type but unknown size.
*/

class RootDetectorMap
{
private:
  boost::coord_map<int, int> map;
  int col_split = -1;
public:
  inline RootDetectorMap();
  inline void set_detector_map_filename(const std::string &filename);
  inline std::vector<int> get_ring_crystal_from_map(std::vector<int> root_det);
  inline void set_col_split(const int col_split);
  inline boost::coord_map<int, int> load_detector_map(const std::string &filename);
  inline void load_detectormap_from_file(const std::string& filename);
  inline ~RootDetectorMap();
};

END_NAMESPACE_STIR

#include "stir/RootDetectorMap.inl"

#endif // __RootDetectorMap_h__

