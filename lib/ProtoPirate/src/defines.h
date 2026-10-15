#pragma once
#include <furi.h>

// **** User Adjustable ****
//#define ENABLE_WELCOME_SCREEN
//#define ENABLE_TIMING_TUNER_SCENE
#define ENABLE_SUB_DECODE_SCENE
#define ENABLE_EMULATE_FEATURE
#define ENABLE_MODELS_DATABASE
#define ENABLE_REMOTE_ANALYZER

// **** DO NOT TOUCH! ****
#if defined(ENABLE_EMULATE_FEATURE) && !defined(PROTOPIRATE_PROTOCOL_RX_ONLY)
#define PROTOPIRATE_WITH_ENCODER 1
#else
#define PROTOPIRATE_WITH_ENCODER 0
#endif

#ifndef PROTOPIRATE_PROTOCOL_TX_ONLY
#define PROTOPIRATE_WITH_DECODER 1
#else
#define PROTOPIRATE_WITH_DECODER 0
#endif

//Do we want the FALs packaged inside the app or not?
#define EMBEDDED_FALS

// ****Add logs back for debugging and reporting problems ****
#define REMOVE_LOGS

// **** DO NOT TOUCH! ****
#ifdef REMOVE_LOGS

// Undefine existing macros
#undef FURI_LOG_E
#undef FURI_LOG_W
#undef FURI_LOG_I
#undef FURI_LOG_D
#undef FURI_LOG_T
// Define empty macros
#define FURI_LOG_E(tag, format, ...)
#define FURI_LOG_W(tag, format, ...)
#define FURI_LOG_I(tag, format, ...)
#define FURI_LOG_D(tag, format, ...)
#define FURI_LOG_T(tag, format, ...)

#endif // REMOVE_LOGS
