#include "GLViewer.h"

//xtern CMainApplication gMyApp;

// Perform the asymmetric frustum perspective projection for one eye's 
//      subfield.
// The projection is in the direction of the negative z axis.
//
// dfLeftBorder, dfRightBorder, dfBottomBorder, dfTopBorder = 
//      The coordinate range, in the z-axis target plane, which will be 
//      displayed on the screen. The ratio between (dfRightBorder-dfLeftBorder)
//      and (dfTopBorder-dfBottomBorder) should equal the aspect ratio of the 
//      scene. Also, dfLeftBorder must be less than dfRightBorder, and 
//      dfTopBorder must be less than dfBottomBorder.
//
// dfNearBorder, dfFarBorder = 
//      The z-coordinate values of the clipping planes. Since the projection is 
//      in the direction of the negative z axis, dfNearBorder needs to be 
//      greater than dfFarBorder. Any element with a z-coordinate value greater 
//      than dfNearBorder, or less than dfFarBorder, will be clipped out.
//
// dfTargetPlane = 
//      The z-coordinate value of the mid-target plane that will, by default,
//      project to zero parallax. This value should reside somewhere between
//      dfNearBorder and dfFarBorder.
//
// dfCameraToTargetDistance = 
//      The distance from the center of projection to the plane of zero 
//      parallax. This distance needs to be greater than the difference between
//      dfNearBorder and dfTargetPlane, in order for the near clipping plane
//      to lie in front of the camera.
//
// dfStereoMagnitudeAdj = 
//      The desired magnitude of the stereo effect. 0.0 would result in no 
//      stereo effect at all, 1.0 would be a good default value, 2.0 would 
//      be a very strong (perhaps uncomfortable) stereo effect. This value
//      should never be less than 0.0.
//
// dfParallaxBalanceAdj = 
//      The amount by which to affect the asymmetry of the projection frustum,
//      effectively adjusting the stereo parallax balance. 0.0 would result
//      in extreme negative parallax (with objects at infinite distance 
//      projecting to display surface), 1.0 would be a good default value
//      (dfTargetPlane will project to zero parallax at the display surface),
//      2.0 would result in considerable positive parallax (most of the scene
//      projecting behind the display surface). This value should never be less
//      than 0.0. When this value equals 0.0, the projection frustum is 
//      perfectly symmetrical.
//
// dfEyePosition = -1 for left eye projection, 1 for right eye projection

void GLViewer::StereoProjection( double dfLeftBorder,
								   double dfRightBorder,
								   double dfBottomBorder,
								   double dFVertical,
								   double dfNearBorder,
								   double dfFarBorder,
								   double dfTargetPlane,
								   double dfCameraToTargetDistance,
								   double dfStereoMagnitudeAdj,
								   double dfParallaxBalanceAdj,
								   int WhichEyeProjection )
{
	// the X & Y axis ranges, in the target Z plane
	double dfXRange = dfRightBorder - dfLeftBorder;
	double dfYRange = dFVertical - dfBottomBorder;  

	// midpoints of the X & Y axis ranges
	double dfXMidpoint = (dfRightBorder + dfLeftBorder) / 2.0;  
	double dfYMidpoint = (dFVertical + dfBottomBorder) / 2.0;  

	// convert clipping plane positions to distances in front of camera
	double dfCameraPlane = dfTargetPlane + dfCameraToTargetDistance;
	double dfNearClipDistance = dfCameraPlane - dfNearBorder;
	double dfFarClipDistance  = dfCameraPlane - dfFarBorder;

	// Determine the stereoscopic camera offset. A good rule of thumb is 
	//	    for the overall camera separation to equal about 7% of the 
	//	    window's X-axis range in the XY-plane of the target	
	//	    ("target" being mid-object or the center of interest in the 
	//	    scene). 
	double dfStereoCameraOffset = dfXRange * FCameraSeparation * dfStereoMagnitudeAdj;
	dfStereoCameraOffset /= 2.0;  // offset each camera by half the overall sep
	if (WhichEyeProjection == LEFT_EYE_PROJECTION) // left cam has neg offset
		dfStereoCameraOffset = -dfStereoCameraOffset; 
	    
		// Determine the amount by which the projection frustum will be made
		//	    asymmetrical in order to affect a nice parallax balance between
		//	    negative parallax (popping out of the display) and positive 
		//	    parallax (residing behind the display surface). With no frustum
		//	    asymmetry, everything resides in negative parallax.
	double dfFrustumAsymmetry = -dfStereoCameraOffset * dfParallaxBalanceAdj * 1000.0;

		// since glFrustum() maps the window borders based on the near clipping 
		//	    plane rather than the target plane, X and Y range values need 
		//	    to be adjusted by the ratio of those two distances
	double n_over_d = dfNearClipDistance / dfCameraToTargetDistance;
	dfXRange *= n_over_d;
	dfYRange *= n_over_d;
	dfFrustumAsymmetry *= n_over_d;
	    
		// determine the shape of the projection frustum; note that if 
		//	    FrustumRight doesn't equal -FrustumLeft, that makes this an
		//	    asymmetric frustum projection
	double FrustumTop = dfYRange / 2.0;
	double FrustumBottom = -dfYRange / 2.0;
	double FrustumRight = (dfXRange / 2.0) + dfFrustumAsymmetry;
	double FrustumLeft = (-dfXRange / 2.0) + dfFrustumAsymmetry;

	//glMatrixMode( GL_PROJECTION ); //needs to have been called already
	//glLoadIdentity();  // obtain a vanilla trans matrix to modify
	//gluPerspective( dViewingAngle, fAspect, 0.1f, 10000.0f );	// oblicz perspektywê dla okna

	// this matrix transformation performs the actual persp projection
	glFrustum (FrustumLeft, FrustumRight, FrustumBottom, FrustumTop, dfNearClipDistance, dfFarClipDistance);

	// this matrix transformation does two things: Translates the stereo 
	//      camera towards the left (left camera) or the right (right 
	//      camera), and also offsets the entire geometry such that the 
	//	    virtual camera is at (0.0, 0.0, 0.0) where glFrustum() expects 
	//	    it to be
	//glMatrixMode( GL_MODELVIEW );
	//glLoadIdentity();

	glTranslated (-dfXMidpoint - dfStereoCameraOffset, -dfYMidpoint, -dfCameraPlane);
}




