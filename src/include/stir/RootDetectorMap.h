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
  \ingroup projdata
  \brief Declaration of class stir::RootDetectorMap
  \author Kris Thielemans
*/
#include <stir/extend_boost.h>

#ifndef __stir_RootDetectorMap_H__
#define __stir_RootDetectorMap_H__

START_NAMESPACE_STIR

class Succeeded;

/*!   \ingroup projdata
 \brief
 This code is a generic implementation of unordered map between two vectors of known type but unknown size.
*/

class RootDetectorMap
{
private:
  boost::coord_map<int, int> map;
  std::string filename = "";
  int col_split = -1;
public:
  RootDetectorMap();
  void set_detector_map_filename(const std::string &filename);
  void set_col_split(const int col_split);
  boost::coord_map<int, int> read_detectormap_from_file_help(const std::string &filename);
  void read_detectormap_from_file(const std::string& filename);
  void set_detector_map(boost::coord_map<int,int>& coord_map);
  ~RootDetectorMap();
};

RootDetectorMap::RootDetectorMap(/* args */){}
RootDetectorMap::~RootDetectorMap(){}
void RootDetectorMap::set_detector_map_filename(const std::string &filename)
  {
	  this->filename = filename;
  }
void RootDetectorMap::set_col_split(const int col_split)
  {
	  this->col_split = col_split;
  }
boost::coord_map<int, int> RootDetectorMap::read_detectormap_from_file_help(const std::string &filename)
  {
    std::ifstream myfile(filename.c_str());
    int col_split = this->col_split;
    if( !myfile ) {error("Error opening file '" + filename + "'");}
    if( col_split == -1){error("Error column split for the table not defined");}

    boost::coord_map<int, int> coord_map;
    std::string line;
    
    while( std::getline( myfile, line))
    {
      if( line.size() && line[0] == '#' ) continue;
      bool has_layer_index = false;
      std::vector<int> root_coord;
      std::vector<int> det_coord;
      std::vector<std::string> col;
      boost::split(col, line, boost::is_any_of("\t,"));
      if( !col.size() ) break;
      int col_length = col.size();
      for (size_t i = 0; i < col_split; i++)
      {
        root_coord[i] = static_cast<int>(atof(col[i].c_str() ));
      };
      for (size_t i = col_split; i<col_length; i++){
        det_coord[i] = static_cast<int>(atof(col[i].c_str() ));
      };
      coord_map[root_coord] = det_coord;
    };
    return coord_map;
  };
void RootDetectorMap::read_detectormap_from_file(const std::string& filename)
  {
    boost::coord_map<int, int> coord_map = this->read_detectormap_from_file_help(filename);
    RootDetectorMap::set_detector_map(coord_map);
  };
void RootDetectorMap::set_detector_map(boost::coord_map<int,int>& coord_map)
  {
    this->map = coord_map;
  };

END_NAMESPACE_STIR
#endif

