# C O P Y		F I L E S		T O		O B E C T

mkdir object
mkdir object/sim
mkdir object/data
mkdir object/recon
cp templates/* object/sim
cp -r data object
cd object/sim

# G E N E R A T E	A T T E N U A T I O N		&	P H A N T O M		I M A G E

echo 'generating attenuation image'

generate_image ../../par/generate_atten.par > my_atten.log 2>&1

echo 'generating ACF'

calculate_attenuation_coefficients --ACF my_acfs.hs moliner_atten.hv BPET_Leipzig5_blk.hs > my_create_acfs.log 2>&1

echo 'generating phantom'

generate_image ../../par/generate_phantom.par > my_moliner_phantom.log 2>&1

echo 'forward projecting phantom'

forward_project my_line_integrals.hs moliner_phantom.hv BPET_Leipzig5_blk.hs ../../par/forward_projector_proj_matrix_ray_tracing.par > my_create_line_integrals.log 2>&1

# G E N E R A T E	B L O C K	S C A T T E R

export TEMPLATE=scatter_low_res_blk.hs
export ATTEN_IMAGE=moliner_atten.hv
export ACTIVITY_IMAGE=moliner_phantom.hv
export OUTPUT_PREFIX=moliner_scatter_low_res

echo 'simulating scatter'

simulate_scatter ../../par/scatter_simulation.par > ${OUTPUT_PREFIX}.log 2>&1

stir_math -s --mult APf.hs my_acfs.hs my_line_integrals.hs > APf.log 2>&1

create_tail_mask_from_ACFs --safety-margin 4 --ACF-threshold 0.01 --ACF-filename my_acfs.hs --output-filename tail_mask > tailmask.log 2>&1

stir_math -s --accumulate --including-first --times-scalar -1 --add-scalar 1 tail_mask.hs >  inverted_tail_mask.log 2>&1

echo 'moliner_scatter_low_res.hs is in object/sim'

#read  -n 1 -p "===  ATTENTION: Change the moliner_scatter.hs and APf.hs file in /object/sim so 'Scanner geometry (BlocksOnCylindrical/Cylindrical/Generic)  := Cylindrical' and press enter" mainmenuinput

#echo "===  Upsampling scatter"
#upsample_and_fit_single_scatter \
#--min-scale-factor 1.1829508281614145 \
#--max-scale-factor 1.3801092995216502 \
#--remove-interleaving 0 \
#--half-filter-width 1 \
#--output-filename ${OUTPUT_PREFIX}_pre_norm \
#--data-to-fit APf.hs \
#--data-to-scale ${OUTPUT_PREFIX}.hs \
#--weights tail_mask.hs > ${OUTPUT_PREFIX}_pre_norm.log 2>&1
#if [ $? -ne 0 ]; then
#  echo "Error upsampling scatter"
#  exit 1
#fi
