/*
Copyright 2017 ETH Zurich, Institute of Particle Physics and Astrophysics

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*!

  \file
  \ingroup projdata
  \brief Declaration of class stir::ProjDataInfoGeneric
  
  \author Parisa Khateri
  \author Michael Roethlisberger
*/
#ifndef __stir_ProjDataInfoGeneric_H__
#define __stir_ProjDataInfoGeneric_H__


#include "stir/ProjDataInfo.h"
#include <utility>
#include <vector>

#ifndef STIR_NO_NAMESPACES
using std::vector;
using std::pair;
#endif

START_NAMESPACE_STIR

class Succeeded;
template <typename coordT> class CartesianCoordinate3D;

/*!
  \ingroup projdata
  \brief projection data info for data corresponding to a
  'Generic' sampling.
*/

class ProjDataInfoGeneric: public ProjDataInfo
{
private:
  typedef ProjDataInfo base_type;
  typedef ProjDataInfoGeneric self_type;

public:
  //! Type used by get_all_ring_pairs_for_segment_axial_pos_num()
  typedef vector<pair<int, int> > RingNumPairs;

  //! Constructors
  ProjDataInfoGeneric();
  //! Constructor given all the necessary information
  /*! The min and max ring difference in each segment are passed
  as VectorWithOffsets. All three vectors have to have index ranges
  from min_segment_num to max_segment_num.

  \warning Most of this library assumes that segment 0 corresponds
  to an average ring difference of 0.
  */
  ProjDataInfoGeneric(const shared_ptr<Scanner>& scanner_ptr,
    const VectorWithOffset<int>& num_axial_poss_per_segment, //index ranges from min_segment_num to max_segment_num
    const VectorWithOffset<int>& min_ring_diff,
    const VectorWithOffset<int>& max_ring_diff,
    const int num_views,const int num_tangential_poss);

  inline virtual float get_tantheta(const Bin&) const;

  inline float get_phi(const Bin&) const;

  inline float get_t(const Bin&) const;

  //! Return z-coordinate of the middle of the LOR
  /*!
  The 0 of the z-axis is chosen in the middle of the scanner.
  */
  inline float get_m(const Bin&) const;

  virtual void
    get_LOR(LORInAxialAndNoArcCorrSinogramCoordinates<float>& lor, const Bin& bin) const;

  void set_azimuthal_angle_sampling(const float angle);

  //! set new number of views, covering the same azimuthal angle range
  /*! calls ProjDataInfo::set_num_views(), but makes sure that we cover the
      same range of angles as before (usually, but not necessarily, 180 degrees)
      by adjusting azimuthal_angle_sampling.
  */
  virtual void
    set_num_views(const int new_num_views);

  //! Get the azimuthal sampling (in radians)
  inline float get_azimuthal_angle_sampling() const;
  virtual inline float get_sampling_in_t(const Bin&) const;
  virtual inline float get_sampling_in_m(const Bin&) const;

  //! Get the axial sampling (e.g in z_direction)
  /*!
   \warning The implementation of this function currently assumes that the axial
   sampling is equal to the ring spacing for non-spanned data
   (i.e. no axial compression), while it is half the
   ring spacing for spanned data.
  */
  inline float get_axial_sampling(int segment_num) const;

  //! Get average ring difference for the given segment
  inline float get_average_ring_difference(int segment_num) const;
  //! Get minimum ring difference for the given segment
  inline int get_min_ring_difference(int segment_num) const;
  //! Get maximum ring difference for the given segment
  inline int get_max_ring_difference(int segment_num) const;

  //! Set minimum ring difference
  void set_min_ring_difference(int min_ring_diff_v, int segment_num);
  //! Set maximum ring difference
  void set_max_ring_difference(int max_ring_diff_v, int segment_num);

  virtual void set_num_axial_poss_per_segment(const VectorWithOffset<int>& num_axial_poss_per_segment);
  virtual void set_min_axial_pos_num(const int min_ax_pos_num, const int segment_num);
  virtual void set_max_axial_pos_num(const int max_ax_pos_num, const int segment_num);
  virtual void reduce_segment_range(const int min_segment_num, const int max_segment_num);

  //! Set detector ring radius for all views
  inline void set_ring_radii_for_all_views(const VectorWithOffset<float>& new_ring_radius);

  //! Get detector ring radius for all views
  inline VectorWithOffset<float> get_ring_radii_for_all_views() const;

  //! Get detector ring radius
  inline float get_ring_radius() const;
  inline float get_ring_radius( const int view_num) const;

  //! Get detector ring spacing
  inline float get_ring_spacing() const;

  //! Set detector ring spacing
  void set_ring_spacing(float ring_spacing_v);

  //! Get the mashing factor, i.e. how many 'original' views are combined
  /*! mashing factor = Ndet/(2Nview)
      This gets the result by comparing the number of detectors in the scanner_ptr
      with the actual number of views.
      \warning In the debug version, it is checked with an assert() that the number of
      detectors is an even multiple of the number of views. This is not checked in
      the normal version though.
   */
  inline int get_view_mashing_factor() const;

  //! Find which segment a particular ring difference belongs to
  /*!
    \return Succeeded::yes when a corresponding segment was found.
    */
  inline Succeeded
    get_segment_num_for_ring_difference(int& segment_num, const int ring_diff) const;

    //! Find to which segment and axial position a ring pair contributes
    /*!
      \a ring1, \a ring2 have to be between 0 and scanner.get_num_rings()-1.
      \return Succeeded::yes when a corresponding segment was found.
      \warning axial_pos_num returned might be outside the actual range in the proj_data_info.

      \For CTI data with span, this essentially implements a 'michelogram'.

      \warning Current implementation assumes that the axial positions start from 0 for
      the first ring-pair in the segment.

      \warning The implementation of this function currently assumes that the axial
      sampling is equal to the ring spacing for non-spanned data
      (i.e. no axial compression), while it is half the ring spacing for spanned data.
    */
  inline Succeeded
    get_segment_axial_pos_num_for_ring_pair(int& segment_num,
                                            int& axial_pos_num,
                                            const int ring1,
                                            const int ring2) const;

  //! Find all ring pairs that contribute to a segment and axial position
  /*!
    \a ring1, \a ring2 will be between 0 and scanner.get_num_rings()-1.
    
    \warning The implementation of this function currently assumes that the axial
    sampling is equal to the ring spacing for non-spanned data
    (i.e. no axial compression), while it is half the ring spacing for spanned data.
  */
  inline const RingNumPairs&
    get_all_ring_pairs_for_segment_axial_pos_num(const int segment_num,
						 const int axial_pos_num) const;
  //! Find the number of ring pairs that contribute to a segment and axial position
  /*!
    \warning The implementation of this function currently assumes that the axial
    sampling is equal to the ring spacing for non-spanned data
    (i.e. no axial compression), while it is half the ring spacing for spanned data.
  */
  inline unsigned
    get_num_ring_pairs_for_segment_axial_pos_num(const int segment_num,
						 const int axial_pos_num) const;

  //! Find a ring pair that contributes to a segment and axial position
  /*!
    \a ring1, \a ring2 will be between 0 and scanner.get_num_rings()-1.

    \warning Currently only works when no axial compression is used for the segment (i.e.
    min_ring_diff = max_ring_diff). Otherwise, a error() will be called.

    \warning The implementation of this function currently assumes that the axial
    sampling is equal to the ring spacing for non-spanned data
    (i.e. no axial compression), while it is half the ring spacing for spanned data.
  */
  void
    get_ring_pair_for_segment_axial_pos_num(int& ring1,
					    int& ring2,
					    const int segment_num,
                        const int axial_pos_num) const;

  virtual std::string parameter_info() const;

protected:

  //! a variable that is set if the data corresponds to physical rings in the scanner
  /*! This is (only) used to prevent get_segment_axial_pos_num_for_ring_pair() et al
      to go wild. Indeed, for cases where there's cylindrical sampling, but not
      really any physical rings associated to the sampling, those functions will
      return invalid information.

      The prime case where this is used is for data corresponding to (nearly)
      continuous detectors, such as DHCI systems, or the HiDAC.

      Ideally, this would be done by having a separate class for such systems which
      does not contain the ring-difference et al information. This seems to make
      the hierarchy too complicated though.

      \bug The value of this variable is currently set by checking if the scanner
      is a HiDAC scanner. This needs to be changed.
      if scanner is not HiDAC this variable is set to 1 otherwise to 0 (see constructor)
      */
  bool sampling_corresponds_to_physical_rings;

protected:
  virtual bool blindly_equals(const root_type * const) const = 0;

private:
  float azimuthal_angle_sampling;
  VectorWithOffset<float> ring_radius;
  float ring_spacing;
  VectorWithOffset<int> min_ring_diff;
  VectorWithOffset<int> max_ring_diff;

  /*
    Next members have to be mutable as they can be modified by const member
    functions. We need this because of the presence of set_min_ring_difference()
    which invalidates these precalculated arrays.
    If your compiler does not support mutable (and you don't want to upgrade
    it to something more sensible), your best bet is to remove the
    set_*ring_difference functions, and move the content of
    initialise_ring_diff_arrays() to the constructor. (Not recommended!)
  */

  //! This member will signal if the arrays below contain sensible info or not
  mutable bool ring_diff_arrays_computed;

  //! This member stores the offsets used in get_m()
  /*
  //! warning This is not used in generic geometry. m is found directly from lors.
  mutable VectorWithOffset<float> m_offset;
  */

  //! This member stores the offsets used in get_segment_axial_pos_num_for_ring_pair()
  mutable VectorWithOffset<int> ax_pos_num_offset;

  //! This member stores a table converting ring differences to segment numbers
  mutable VectorWithOffset<int> ring_diff_to_segment_num;

  //! This member stores a table converting segment/axial_pos to ring1+ring2
  mutable VectorWithOffset<VectorWithOffset<int> > segment_axial_pos_to_ring1_plus_ring2;

  //! This function sets all of the above
  void initialise_ring_diff_arrays() const;

  //! This function checks if max_ring_diff is different from min_ring_diff (set to 2).
  /*! in case of difference, there are 2 ax_pos per ring, i.e. an ax_pos between each two rings */
  inline int get_num_axial_poss_per_ring_inc(const int segment_num) const;

  //! This member will signal if the array below contain sensible info or not
  mutable bool segment_axial_pos_to_ring_pair_allocated;

  //! This member stores a table used by get_all_ring_pairs_for_segment_axial_pos_num()
  mutable VectorWithOffset< VectorWithOffset < shared_ptr<RingNumPairs> > >
    segment_axial_pos_to_ring_pair;

  //! allocate table
  void allocate_segment_axial_pos_to_ring_pair() const;

  //! initialise one element of the above table
  void compute_segment_axial_pos_to_ring_pair(const int segment_num, const int axial_pos_num) const;

  //! to be used in get LOR
  virtual void find_cartesian_coordinates_of_detection(CartesianCoordinate3D<float>& coord_1,
  													   CartesianCoordinate3D<float>& coord_2,
													   const Bin& bin) const = 0;

};


END_NAMESPACE_STIR

#include "stir/ProjDataInfoGeneric.inl"

#endif
