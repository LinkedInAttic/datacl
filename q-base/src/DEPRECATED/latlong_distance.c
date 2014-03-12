#include <stdio.h>
#include <math.h>
#include "constants.h"
#include "macros.h"
#include "latlong_distance.h"

// START FUNC DECL
int 
latlong_distance(
	 float lat1,
	 float long1,
	 float lat2,
	 float long2,
	 float *ptr_dist
	 )
// STOP FUNC DECL
{
  int status = 0;
  double degrees_to_radians, phi1, phi2, theta1, theta2, xcos, arc;

  if ( ( lat1 > 90 ) || ( lat1 < -90 ) ) { go_BYE(-1); }
  if ( ( lat2 > 90 ) || ( lat2 < -90 ) ) { go_BYE(-1); }
  if ( ( long1 > 180 ) || ( long1 < -180 ) ) { go_BYE(-1); }
  if ( ( long2 > 180 ) || ( long2 < -180 ) ) { go_BYE(-1); }
  /* This computation is borrowed from Heyning's implementation. It is a 
     verbatim translation from Python to PHP.  */
  degrees_to_radians = 3.141592 / 180.0;
        
  // phi = 90 - latitude
  phi1 = (90.0 - lat1) * degrees_to_radians;
  phi2 = (90.0 - lat2) * degrees_to_radians;

  // theta = longitude
  theta1 = long1 * degrees_to_radians;
  theta2 = long2 * degrees_to_radians;

  /* Compute spherical distance from spherical coordinates.        

  For two locations in spherical coordinates 
  (1, theta, phi) and 
  (1, theta, phi), 

  cosine( arc length ) = sin phi sin phi' cos(theta-theta') + cos phi cos phi'

  distance = rho * arc length

  */
  xcos = (sin(phi1) * sin(phi2) * 
	 cos(theta1 - theta2) + cos(phi1) * cos(phi2));
  arc = acos(xcos);

  // Multiply arc by the radius of the earth in miles
  *ptr_dist = arc * 3959;
 BYE:
  return(status);
}
