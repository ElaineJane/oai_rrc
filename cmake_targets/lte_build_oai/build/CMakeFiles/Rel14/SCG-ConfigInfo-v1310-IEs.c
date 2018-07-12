/*
 * Generated by asn1c-0.9.24 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-InterNodeDefinitions"
 * 	found in "fixed_grammar.asn"
 * 	`asn1c -gen-PER`
 */

#include "SCG-ConfigInfo-v1310-IEs.h"

static asn_TYPE_member_t asn_MBR_SCG_ConfigInfo_v1310_IEs_1[] = {
	{ ATF_POINTER, 6, offsetof(struct SCG_ConfigInfo_v1310_IEs, measResultSSTD_r13),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_MeasResultSSTD_r13,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"measResultSSTD-r13"
		},
	{ ATF_POINTER, 5, offsetof(struct SCG_ConfigInfo_v1310_IEs, sCellToAddModListMCG_Ext_r13),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SCellToAddModListExt_r13,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"sCellToAddModListMCG-Ext-r13"
		},
	{ ATF_POINTER, 4, offsetof(struct SCG_ConfigInfo_v1310_IEs, measResultServCellListSCG_Ext_r13),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_MeasResultServCellListSCG_Ext_r13,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"measResultServCellListSCG-Ext-r13"
		},
	{ ATF_POINTER, 3, offsetof(struct SCG_ConfigInfo_v1310_IEs, sCellToAddModListSCG_Ext_r13),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SCellToAddModListSCG_Ext_r13,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"sCellToAddModListSCG-Ext-r13"
		},
	{ ATF_POINTER, 2, offsetof(struct SCG_ConfigInfo_v1310_IEs, sCellToReleaseListSCG_Ext_r13),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SCellToReleaseListExt_r13,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"sCellToReleaseListSCG-Ext-r13"
		},
	{ ATF_POINTER, 1, offsetof(struct SCG_ConfigInfo_v1310_IEs, nonCriticalExtension),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SCG_ConfigInfo_v1330_IEs,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"nonCriticalExtension"
		},
};
static int asn_MAP_SCG_ConfigInfo_v1310_IEs_oms_1[] = { 0, 1, 2, 3, 4, 5 };
static ber_tlv_tag_t asn_DEF_SCG_ConfigInfo_v1310_IEs_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SCG_ConfigInfo_v1310_IEs_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* measResultSSTD-r13 at 11611 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* sCellToAddModListMCG-Ext-r13 at 11611 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* measResultServCellListSCG-Ext-r13 at 11612 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* sCellToAddModListSCG-Ext-r13 at 11613 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* sCellToReleaseListSCG-Ext-r13 at 11614 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* nonCriticalExtension at 11615 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SCG_ConfigInfo_v1310_IEs_specs_1 = {
	sizeof(struct SCG_ConfigInfo_v1310_IEs),
	offsetof(struct SCG_ConfigInfo_v1310_IEs, _asn_ctx),
	asn_MAP_SCG_ConfigInfo_v1310_IEs_tag2el_1,
	6,	/* Count of tags in the map */
	asn_MAP_SCG_ConfigInfo_v1310_IEs_oms_1,	/* Optional members */
	6, 0,	/* Root/Additions */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SCG_ConfigInfo_v1310_IEs = {
	"SCG-ConfigInfo-v1310-IEs",
	"SCG-ConfigInfo-v1310-IEs",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	SEQUENCE_decode_aper,
	SEQUENCE_encode_aper,
	SEQUENCE_compare,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SCG_ConfigInfo_v1310_IEs_tags_1,
	sizeof(asn_DEF_SCG_ConfigInfo_v1310_IEs_tags_1)
		/sizeof(asn_DEF_SCG_ConfigInfo_v1310_IEs_tags_1[0]), /* 1 */
	asn_DEF_SCG_ConfigInfo_v1310_IEs_tags_1,	/* Same as above */
	sizeof(asn_DEF_SCG_ConfigInfo_v1310_IEs_tags_1)
		/sizeof(asn_DEF_SCG_ConfigInfo_v1310_IEs_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SCG_ConfigInfo_v1310_IEs_1,
	6,	/* Elements count */
	&asn_SPC_SCG_ConfigInfo_v1310_IEs_specs_1	/* Additional specs */
};

