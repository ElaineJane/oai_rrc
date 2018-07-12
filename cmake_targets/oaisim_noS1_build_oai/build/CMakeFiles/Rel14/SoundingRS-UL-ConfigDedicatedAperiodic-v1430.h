/*
 * Generated by asn1c-0.9.24 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "fixed_grammar.asn"
 * 	`asn1c -gen-PER`
 */

#ifndef	_SoundingRS_UL_ConfigDedicatedAperiodic_v1430_H_
#define	_SoundingRS_UL_ConfigDedicatedAperiodic_v1430_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NULL.h>
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SoundingRS_UL_ConfigDedicatedAperiodic_v1430_PR {
	SoundingRS_UL_ConfigDedicatedAperiodic_v1430_PR_NOTHING,	/* No components present */
	SoundingRS_UL_ConfigDedicatedAperiodic_v1430_PR_release,
	SoundingRS_UL_ConfigDedicatedAperiodic_v1430_PR_setup
} SoundingRS_UL_ConfigDedicatedAperiodic_v1430_PR;

/* SoundingRS-UL-ConfigDedicatedAperiodic-v1430 */
typedef struct SoundingRS_UL_ConfigDedicatedAperiodic_v1430 {
	SoundingRS_UL_ConfigDedicatedAperiodic_v1430_PR present;
	union SoundingRS_UL_ConfigDedicatedAperiodic_v1430_u {
		NULL_t	 release;
		struct SoundingRS_UL_ConfigDedicatedAperiodic_v1430__setup {
			long	*srs_SubframeIndication_r14	/* OPTIONAL */;
			
			/* Context for parsing across buffer boundaries */
			asn_struct_ctx_t _asn_ctx;
		} setup;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SoundingRS_UL_ConfigDedicatedAperiodic_v1430_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SoundingRS_UL_ConfigDedicatedAperiodic_v1430;

#ifdef __cplusplus
}
#endif

#endif	/* _SoundingRS_UL_ConfigDedicatedAperiodic_v1430_H_ */
#include <asn_internal.h>