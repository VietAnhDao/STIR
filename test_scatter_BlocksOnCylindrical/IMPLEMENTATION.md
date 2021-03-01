## HOW TO IMPLEMENT BlocksOnCylindrical IN SCATTER

To make Scatter work for BlocksOnCylindrical:

    1.  change all files asociated with ScatterSimulation.cxx which has the key word "ProjDataInfoCylindricalNoArcCor"
        to "ProjDataInfoBlocksOnCylindricalNoArcCorr".
    2.  Change in the import "#include "stir/ProjDataInfoBlocksOnCylindricalNoArcCorr.h"" into import section of code.
    3.  Change the intefile so the spacing between crystal in transaxial direction is "well spaced out" (meaning no
        large gaps between blocks or buckets).
    4.  Comment out two line at 408 & 409 preventing it to shift coordinate to center scanner in z-coordinate direction
        since our crystal map for BlocksOnCylindrical is already centered in z-direction.


NOTE: this early implementation to make BlocksOnCylindrical geometry work on ScatterSimulate. Once changed it wont work for Cylindrical geometry, this will be fixed later on.