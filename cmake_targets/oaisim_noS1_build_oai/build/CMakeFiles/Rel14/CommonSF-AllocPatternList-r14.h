/*
 * Generated by asn1c-0.9.24 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "fixed_grammar.asn"
 * 	`asn1c -gen-PER`
 */

#ifndef	_CommonSF_AllocPatternList_r14_H_
#define	_CommonSF_AllocPatternList_r14_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct MBSFN_SubframeConfig_v1430;

/* CommonSF-AllocPatternList-r14 */
typedef struct CommonSF_AllocPatternList_r14 {
	A_SEQUENCE_OF(struct MBSFN_SubframeConfig_v1430) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CommonSF_AllocPatternList_r14_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CommonSF_AllocPatternList_r14;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "MBSFN-SubframeConfig-v1430.h"

#endif	/* _CommonSF_AllocPatternList_r14_H_ */
#include <asn_internal.h>