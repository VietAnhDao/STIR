//
//
/*!

  \file
  \ingroup RootDetectorMap
  \brief inline implementations for class stir::RootDetectorMap

  \author Viet Dao


*/
/*
    Copyright (C) 2000 PARAPET partners
    Copyright (C) 2000-2005, Hammersmith Imanet Ltd
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0 AND License-ref-PARAPET-license

    See STIR/LICENSE.txt for details
*/

START_NAMESPACE_STIR

RootDetectorMap::RootDetectorMap(){}
RootDetectorMap::~RootDetectorMap(){}
void RootDetectorMap::set_col_split(const int col_split)
  {
	  this->col_split = col_split;
  }
boost::coord_map<int, int> RootDetectorMap::load_detector_map(const std::string &filename)
  {
    std::ifstream myfile(filename.c_str());
    int col_split = this->col_split;
    //std::cout << "col_split: " << col_split << "\n";
    if( !myfile ) {error("Error opening file '" + filename + "'");}
    if( col_split == -1){error("Error column split for the table not defined");}
    boost::coord_map<int, int> coord_map;
    std::string line;
    while( std::getline( myfile, line))
    {
      if( line.size() && line[0] == '#' ) continue;
      bool has_layer_index = false;
      std::vector<std::string> col;
      boost::split(col, line, boost::is_any_of("\t,"));
      int col_length = col.size();
      std::vector<int> root_coord(col_split);
      std::vector<int> det_coord(2);
      if( !col.size() ) break;
      for (size_t i = 0; i < col_split; i++)
      {
        int val = static_cast<int>(atof(col[i].c_str() ));
        root_coord[i] = val;
      };
      for (size_t i = 0; i < 2; i++){
        det_coord[i] = (static_cast<int>(atof(col[i+col_split].c_str() )));
      };
      coord_map[root_coord] = det_coord;
      //for (size_t i = 0; i < root_coord.size(); i++)
      //{
      //  std::cout << root_coord[i] << ", ";
      //}
      //std::cout << "\t" << det_coord[0] << ", " << det_coord[1] << "\n"; 
    };
    return coord_map;
  };
void RootDetectorMap::load_detectormap_from_file(const std::string& filename)
  {
    boost::coord_map<int, int> coord_map = this->load_detector_map(filename);
    this->map = coord_map;
  };
std::vector<int> RootDetectorMap::get_ring_crystal_from_map(std::vector<int> root_det)
{
    std::vector<int> ring_det = map[root_det];
    return ring_det;
}

END_NAMESPACE_STIR