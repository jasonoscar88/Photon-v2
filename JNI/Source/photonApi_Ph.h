/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class photonApi_Ph */

#ifndef _Included_photonApi_Ph
#define _Included_photonApi_Ph
#ifdef __cplusplus
extern "C" {
#endif
#undef photonApi_Ph_PH_BRUTE_FORCE_RENDERER_TYPE
#define photonApi_Ph_PH_BRUTE_FORCE_RENDERER_TYPE 1L
#undef photonApi_Ph_PH_IMPORTANCE_RENDERER_TYPE
#define photonApi_Ph_PH_IMPORTANCE_RENDERER_TYPE 2L
#undef photonApi_Ph_PH_MT_IMPORTANCE_RENDERER_TYPE
#define photonApi_Ph_PH_MT_IMPORTANCE_RENDERER_TYPE 3L
#undef photonApi_Ph_PH_HDR_FRAME_TYPE
#define photonApi_Ph_PH_HDR_FRAME_TYPE 50L
#undef photonApi_Ph_PH_DEFAULT_CAMERA_TYPE
#define photonApi_Ph_PH_DEFAULT_CAMERA_TYPE 100L
#undef photonApi_Ph_PH_PIXEL_JITTER_SAMPLE_GENERATOR_TYPE
#define photonApi_Ph_PH_PIXEL_JITTER_SAMPLE_GENERATOR_TYPE 150L
/*
 * Class:     photonApi_Ph
 * Method:    phStart
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_photonApi_Ph_phStart
  (JNIEnv *, jclass);

/*
 * Class:     photonApi_Ph
 * Method:    phExit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phExit
  (JNIEnv *, jclass);

/*
 * Class:     photonApi_Ph
 * Method:    phCreateRenderer
 * Signature: (LphotonApi/LongRef;II)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateRenderer
  (JNIEnv *, jclass, jobject, jint, jint);

/*
 * Class:     photonApi_Ph
 * Method:    phDeleteRenderer
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteRenderer
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phCreateFilm
 * Signature: (LphotonApi/LongRef;II)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateFilm
  (JNIEnv *, jclass, jobject, jint, jint);

/*
 * Class:     photonApi_Ph
 * Method:    phDeleteFilm
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteFilm
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phCreateCamera
 * Signature: (LphotonApi/LongRef;I)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateCamera
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     photonApi_Ph
 * Method:    phDeleteCamera
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteCamera
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phCreateFrame
 * Signature: (LphotonApi/LongRef;III)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateFrame
  (JNIEnv *, jclass, jobject, jint, jint, jint);

/*
 * Class:     photonApi_Ph
 * Method:    phDeleteFrame
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteFrame
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phCreateWorld
 * Signature: (LphotonApi/LongRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateWorld
  (JNIEnv *, jclass, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phDeleteWorld
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteWorld
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phCreateSampleGenerator
 * Signature: (LphotonApi/LongRef;II)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateSampleGenerator
  (JNIEnv *, jclass, jobject, jint, jint);

/*
 * Class:     photonApi_Ph
 * Method:    phDeleteSampleGenerator
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteSampleGenerator
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phRender
 * Signature: (JJJ)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phRender
  (JNIEnv *, jclass, jlong, jlong, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phLoadTestScene
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phLoadTestScene
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phSetCameraFilm
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetCameraFilm
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phSetRendererSampleGenerator
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetRendererSampleGenerator
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phCookWorld
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCookWorld
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phDevelopFilm
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDevelopFilm
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phGetFrameData
 * Signature: (JLphotonApi/FloatArrayRef;LphotonApi/IntRef;LphotonApi/IntRef;LphotonApi/IntRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetFrameData
  (JNIEnv *, jclass, jlong, jobject, jobject, jobject, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phSetCameraPosition
 * Signature: (JFFF)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetCameraPosition
  (JNIEnv *, jclass, jlong, jfloat, jfloat, jfloat);

/*
 * Class:     photonApi_Ph
 * Method:    phSetCameraDirection
 * Signature: (JFFF)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetCameraDirection
  (JNIEnv *, jclass, jlong, jfloat, jfloat, jfloat);

/*
 * Class:     photonApi_Ph
 * Method:    printTestMessage
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_printTestMessage
  (JNIEnv *, jclass);

/*
 * Class:     photonApi_Ph
 * Method:    genTestHdrFrame
 * Signature: (LphotonApi/FloatArrayRef;LphotonApi/IntRef;LphotonApi/IntRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_genTestHdrFrame
  (JNIEnv *, jclass, jobject, jobject, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phQueryRendererPercentageProgress
 * Signature: (JLphotonApi/FloatRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phQueryRendererPercentageProgress
  (JNIEnv *, jclass, jlong, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phQueryRendererSampleFrequency
 * Signature: (JLphotonApi/FloatRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phQueryRendererSampleFrequency
  (JNIEnv *, jclass, jlong, jobject);

#ifdef __cplusplus
}
#endif
#endif
