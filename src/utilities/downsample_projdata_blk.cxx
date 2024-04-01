#include "stir/ProjData.h" 
#include "stir/ProjDataInfo.h" 
#include "stir/ProjDataInterfile.h" 
#include "stir/ProjDataInfoBlocksOnCylindricalNoArcCorr.h"
#include "stir/stir_math.h" 
#include "stir/extend_boost.h" 
#include "stir/DetectionPosition.h" 
#include "stir/DetectionPositionPair.h" 
#include "stir/Sinogram.h"
#include "stir/Bin.h" 
#include "stir/LORCoordinates.h" 
#include <boost/format.hpp> 
#include <iostream> 
USING_NAMESPACE_STIR 
typedef float elem_type;
typedef SegmentByView<elem_type> segment_type;
typedef boost::tuple<int, int, int> det_coord; 
typedef unsigned int uint;
int main(int argc, char * argv[]){ 
    if (argc != 5) 
    { 
        std::cerr << "insufficient number of arguements\n example: downsample_projdata projdata.hs 2 3 output_filename.hs \n This will downsample the file called projdata.hs's detector by factor of 2 and downsample ring by factor of 3"; 
        return 0; 
    } 
    int downsample_detector_scale = std::stoi(argv[2]); 
    int downsample_ring_scale = std::stoi(argv[3]); 
    std::string out_filename = std::string(argv[4]);
    // read the full resolution from file 
    shared_ptr<ProjData> full_resolution_proj_data_sptr = ProjData::read_from_file(std::string(argv[1])); 
    // get scanner information from full res 
    shared_ptr<const ProjDataInfo> full_res_proj_data_info_sptr = full_resolution_proj_data_sptr->get_proj_data_info_sptr(); 
    const Scanner *const old_scanner_ptr = full_res_proj_data_info_sptr->get_scanner_ptr(); 
    shared_ptr<Scanner> new_scanner_sptr( new Scanner(*old_scanner_ptr));
    
    
    // downsample scanner 
    int new_num_detector = int(old_scanner_ptr->get_num_detectors_per_ring()/downsample_detector_scale); 
    float new_det_spacing = float(old_scanner_ptr->get_transaxial_crystal_spacing()*downsample_detector_scale); 
    new_scanner_sptr->set_num_detectors_per_ring(new_num_detector); 
    new_scanner_sptr->set_transaxial_crystal_spacing(new_det_spacing); 

    float new_num_arccorrected_bins = new_scanner_sptr->get_default_num_arccorrected_bins()/downsample_detector_scale + 5; 
    new_scanner_sptr->set_default_num_arccorrected_bins(ceil(new_num_arccorrected_bins + 0.5f)); 
    new_scanner_sptr->set_max_num_non_arccorrected_bins(ceil(new_num_arccorrected_bins + 0.5f)); 

    new_scanner_sptr->set_default_bin_size(new_scanner_sptr->get_effective_ring_radius() * _PI / new_num_detector); 
    new_scanner_sptr->set_num_transaxial_crystals_per_block(old_scanner_ptr->get_num_transaxial_crystals_per_block()/downsample_detector_scale); 
    int new_num_ring = int(old_scanner_ptr->get_num_rings()/downsample_ring_scale); 
    float new_ring_spacing = float(old_scanner_ptr->get_axial_crystal_spacing()*downsample_ring_scale); 
    new_scanner_sptr->set_num_rings(new_num_ring); 
    new_scanner_sptr->set_axial_crystal_spacing(new_ring_spacing); 
    new_scanner_sptr->set_ring_spacing(new_ring_spacing); 
    new_scanner_sptr->set_num_axial_crystals_per_block(old_scanner_ptr->get_num_axial_crystals_per_block()/downsample_ring_scale); 
    float delta_ring = new_scanner_sptr->get_num_rings()-1; 
    new_scanner_sptr->set_up(); 
    shared_ptr<ProjDataInfo> downsample_proj_data_info_sptr( 
                                                            ProjDataInfo::ProjDataInfoCTI(new_scanner_sptr, 
                                                            1, delta_ring, 
                                                            new_scanner_sptr->get_num_detectors_per_ring()/2, 
                                                            new_scanner_sptr->get_max_num_non_arccorrected_bins(), 
                                                            false)); 
    // create a crystal map between full res and low res 
    boost::coord_map<uint, uint> full_res_to_low_res_map; 
    uint full_res_det_num = old_scanner_ptr->get_num_detectors_per_ring(); 
    uint full_res_ring_num = old_scanner_ptr->get_num_rings(); 
    uint full_res_layer_num = old_scanner_ptr->get_num_detector_layers(); 
    
    //create a map between the high res and low res scanner.
    for (uint det = 0; det < full_res_det_num; det++) 
    { 
        for (uint ring = 0; ring < full_res_ring_num; ring++) 
        { 
            for (uint layer = 0; layer < full_res_layer_num; layer++) 
            {  
                std::vector<uint> full_res_det = {det, ring, layer}; 
                std::vector<uint> low_res_det = {uint(det/downsample_detector_scale), uint(ring/downsample_ring_scale), uint(layer)}; 
                full_res_to_low_res_map[full_res_det] = low_res_det; 
            } 
        } 
    } 
    std::cout << "down casting projdata info\n";
    // cast the proj_data_info down to the appropriate child class
    const ProjDataInfoBlocksOnCylindricalNoArcCorr * full_res_proj_data_info_blk_ptr =
        dynamic_cast<const ProjDataInfoBlocksOnCylindricalNoArcCorr *>(full_res_proj_data_info_sptr.get());
    const ProjDataInfoBlocksOnCylindricalNoArcCorr * low_res_proj_data_info_blk_ptr =
        dynamic_cast<const ProjDataInfoBlocksOnCylindricalNoArcCorr *>(downsample_proj_data_info_sptr.get());
    // create segment and load segment into memory
    VectorWithOffset<segment_type *> 
    low_res_segments (low_res_proj_data_info_blk_ptr->get_min_segment_num(), 
	      low_res_proj_data_info_blk_ptr->get_max_segment_num());
    int start_segment_index = low_res_proj_data_info_blk_ptr->get_min_segment_num();
    int end_segment_index = low_res_proj_data_info_blk_ptr->get_max_segment_num();
    
    for (int seg=start_segment_index ; seg<=end_segment_index; seg++)
    {
        low_res_segments[seg] = new SegmentByView<elem_type>(downsample_proj_data_info_sptr->get_empty_segment_by_view(seg));
        for(int view = downsample_proj_data_info_sptr->get_min_view_num(); view <= downsample_proj_data_info_sptr->get_max_view_num(); view++){
            for(int ax = downsample_proj_data_info_sptr->get_min_axial_pos_num(seg); ax <= downsample_proj_data_info_sptr->get_max_axial_pos_num(seg); ax++){
                for(int tang = downsample_proj_data_info_sptr->get_min_tangential_pos_num(); tang <= downsample_proj_data_info_sptr->get_max_tangential_pos_num(); tang++){
                    (*low_res_segments[seg])[view][ax][tang] = 0.f;
                    //std::cout << seg << ",\t" << view << ",\t" << ax << ",\t" << tang << ",\t" << (*low_res_segments[seg])[view][ax][tang] << "\n";
                }
            }
        }
    }
    // create a new ProjData to stores data
    shared_ptr<ExamInfo> exam_info_sptr(new ExamInfo); 
    shared_ptr<ProjData> downsample_projdata_sptr; 
    downsample_projdata_sptr.reset(new ProjDataInterfile(exam_info_sptr, 
    low_res_proj_data_info_blk_ptr->create_shared_clone(), 
    "test_downsample.hs", 
    std::ios::in | std::ios::out | std::ios::trunc)); 
    for (int seg=start_segment_index; seg<=end_segment_index; seg++)
    {
        downsample_projdata_sptr->set_segment(*low_res_segments[seg]);
    }

    // now loop through all possible LORS in full res and combine the count to low res scanner. 
    stir::DetectionPositionPair<> full_res_det_pos_pair; 
    stir::DetectionPositionPair<> low_res_det_pos_pair; 
    std::cout << "loading full res sinogram\n";
    Sinogram<float> full_res_sino = full_resolution_proj_data_sptr->get_sinogram(0, 0);
    std::cout << "looping thorugh all bins\n";

    /*
    uint low_res_det_num = new_scanner_ptr->get_num_detectors_per_ring(); 
    uint low_res_ring_num = new_scanner_ptr->get_num_rings(); 
    uint low_res_layer_num = new_scanner_ptr->get_num_detector_layers(); 

    for(uint ringA=0; ringA < low_res_ring_num; ringA++){
        for(uint detA=0; detA < low_res_det_num; detA++){
            for(uint ringB=0; ringB < low_res_ring_num; ringB++){
                for (uint detB = 0; detB < low_res_ring_num; detB++)
                {
                    
                }
            }
        }
    }
    */
    for(uint ringA = 0; ringA < full_res_ring_num; ringA++) 
    { 
        for (uint detA = 0; detA < full_res_det_num; detA++) 
        { 
            for (uint layerA = 0; layerA < full_res_layer_num; layerA++) 
            { 
                std::cout << detA << ",\t" << ringA << "\n";
                for(uint ringB = 0; ringB < full_res_ring_num; ringB++) 
                { 
                    for(uint detB = 0; detB < full_res_det_num; detB++) 
                    {  
                        for (uint layerB = 0; layerB < full_res_layer_num; layerB++) 
                        { 
                            if(detA==detB && ringA==ringB && layerA==layerB){continue;}; 
                            stir::DetectionPosition<> full_res_det_pos1 = DetectionPosition<>(detA, ringA, layerA); 
                            stir::DetectionPosition<> full_res_det_pos2 = DetectionPosition<>(detB, ringB, layerB); 
                            full_res_det_pos_pair.pos2() = full_res_det_pos2; 
                            full_res_det_pos_pair.pos1() = full_res_det_pos1; 
                            Bin ful_res_b;
                            full_res_proj_data_info_blk_ptr->get_bin_for_det_pos_pair(ful_res_b, full_res_det_pos_pair);
                            if(ful_res_b.view_num() >= full_res_proj_data_info_sptr->get_min_view_num() 
                                && ful_res_b.view_num() <= full_res_proj_data_info_sptr->get_max_view_num()
                                && ful_res_b.axial_pos_num() >= full_res_proj_data_info_sptr->get_min_axial_pos_num(ful_res_b.segment_num())
                                && ful_res_b.axial_pos_num() <= full_res_proj_data_info_sptr->get_max_axial_pos_num(ful_res_b.segment_num())
                                && ful_res_b.tangential_pos_num() >= full_res_proj_data_info_sptr->get_min_tangential_pos_num()
                                && ful_res_b.tangential_pos_num() <= full_res_proj_data_info_sptr->get_max_tangential_pos_num()){
                                Sinogram<float> sino = full_resolution_proj_data_sptr->get_sinogram(ful_res_b.axial_pos_num(), ful_res_b.segment_num());
                                if(full_res_sino[ful_res_b.view_num()][ful_res_b.view_num()] != 0.f){}
                                //std::cout << "full res:\t" << ful_res_b.segment_num() << ", \t" << ful_res_b.view_num() << ",\t" << ful_res_b.axial_pos_num() << ",\t" << ful_res_b.tangential_pos_num() << ",\t" << full_res_sino[ful_res_b.view_num()][ful_res_b.view_num()] << "\n";
                                std::vector<uint> full_res_det1 = {detA, ringA, layerA}; 
                                std::vector<uint> full_res_det2 = {detB, ringB, layerB}; 
                                std::vector<uint> low_res_det1 = full_res_to_low_res_map[full_res_det1];
                                std::vector<uint> low_res_det2 = full_res_to_low_res_map[full_res_det2];
                                stir::DetectionPosition<> low_res_det_pos1 = DetectionPosition<>(low_res_det1[0], low_res_det1[1], low_res_det1[2]); 
                                stir::DetectionPosition<> low_res_det_pos2 = DetectionPosition<>(low_res_det2[0], low_res_det2[1], low_res_det2[2]); 
                                low_res_det_pos_pair.pos1()=low_res_det_pos1;
                                low_res_det_pos_pair.pos2()=low_res_det_pos2;
                                Bin low_res_b;
                                low_res_proj_data_info_blk_ptr->get_bin_for_det_pos_pair(low_res_b, low_res_det_pos_pair);
                                (*low_res_segments[low_res_b.segment_num()])[low_res_b.view_num()][low_res_b.axial_pos_num()][low_res_b.tangential_pos_num()] = (*low_res_segments[low_res_b.segment_num()])[low_res_b.view_num()][low_res_b.axial_pos_num()][low_res_b.tangential_pos_num()] + full_res_sino[ful_res_b.view_num()][ful_res_b.view_num()];
                                if((*low_res_segments[low_res_b.segment_num()])[low_res_b.view_num()][low_res_b.axial_pos_num()][low_res_b.tangential_pos_num()]!=0.f){std::cout << "low res:\t" << low_res_b.segment_num() << ", \t" << low_res_b.view_num() << ", \t" << low_res_b.axial_pos_num() << ", \t" << low_res_b.tangential_pos_num() << ",\t" << (*low_res_segments[low_res_b.segment_num()])[low_res_b.view_num()][low_res_b.axial_pos_num()][low_res_b.tangential_pos_num()] << "\n";}
                            }
                        }
                    } 
                } 
            } 
        } 
    } 
    std::cout << "saving projdata\n";
    // save the low res proj data 
    std::cout << "finish writing segment to projdata\n";
    downsample_projdata_sptr->write_to_file("test.hs");
    return 0; 
} 
 