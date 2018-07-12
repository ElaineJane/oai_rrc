/*
 * Generated by asn1c-0.9.24 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "fixed_grammar.asn"
 * 	`asn1c -gen-PER`
 */

#include "SC-MTCH-SchedulingInfo-BR-r14.h"

static int
onDurationTimerSCPTM_r14_2_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_NativeEnumerated.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
static void
onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NativeEnumerated.free_struct;
	td->print_struct   = asn_DEF_NativeEnumerated.print_struct;
	td->ber_decoder    = asn_DEF_NativeEnumerated.ber_decoder;
	td->der_encoder    = asn_DEF_NativeEnumerated.der_encoder;
	td->xer_decoder    = asn_DEF_NativeEnumerated.xer_decoder;
	td->xer_encoder    = asn_DEF_NativeEnumerated.xer_encoder;
	td->uper_decoder   = asn_DEF_NativeEnumerated.uper_decoder;
	td->uper_encoder   = asn_DEF_NativeEnumerated.uper_encoder;
	td->aper_decoder   = asn_DEF_NativeEnumerated.aper_decoder;
	td->aper_encoder   = asn_DEF_NativeEnumerated.aper_encoder;
	td->compare        = asn_DEF_NativeEnumerated.compare;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NativeEnumerated.per_constraints;
	td->elements       = asn_DEF_NativeEnumerated.elements;
	td->elements_count = asn_DEF_NativeEnumerated.elements_count;
     /* td->specifics      = asn_DEF_NativeEnumerated.specifics;	// Defined explicitly */
}

static void
onDurationTimerSCPTM_r14_2_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

static int
onDurationTimerSCPTM_r14_2_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

static asn_dec_rval_t
onDurationTimerSCPTM_r14_2_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

static asn_enc_rval_t
onDurationTimerSCPTM_r14_2_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

static asn_dec_rval_t
onDurationTimerSCPTM_r14_2_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

static asn_enc_rval_t
onDurationTimerSCPTM_r14_2_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static asn_dec_rval_t
onDurationTimerSCPTM_r14_2_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

static asn_enc_rval_t
onDurationTimerSCPTM_r14_2_encode_uper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->uper_encoder(td, constraints, structure, per_out);
}

static asn_enc_rval_t
onDurationTimerSCPTM_r14_2_encode_aper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->aper_encoder(td, constraints, structure, per_out);
}

static asn_comp_rval_t * 
onDurationTimerSCPTM_r14_2_compare(asn_TYPE_descriptor_t *td1,
		const void *structure1,
		asn_TYPE_descriptor_t *td2,
		const void *structure2) {
	asn_comp_rval_t * res  = NULL;
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td1);
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td2);
	res = td1->compare(td1, structure1, td2, structure2);
	return res;
}

static asn_dec_rval_t
onDurationTimerSCPTM_r14_2_decode_aper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	onDurationTimerSCPTM_r14_2_inherit_TYPE_descriptor(td);
	return td->aper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

static int
drx_InactivityTimerSCPTM_r14_11_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_NativeEnumerated.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
static void
drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NativeEnumerated.free_struct;
	td->print_struct   = asn_DEF_NativeEnumerated.print_struct;
	td->ber_decoder    = asn_DEF_NativeEnumerated.ber_decoder;
	td->der_encoder    = asn_DEF_NativeEnumerated.der_encoder;
	td->xer_decoder    = asn_DEF_NativeEnumerated.xer_decoder;
	td->xer_encoder    = asn_DEF_NativeEnumerated.xer_encoder;
	td->uper_decoder   = asn_DEF_NativeEnumerated.uper_decoder;
	td->uper_encoder   = asn_DEF_NativeEnumerated.uper_encoder;
	td->aper_decoder   = asn_DEF_NativeEnumerated.aper_decoder;
	td->aper_encoder   = asn_DEF_NativeEnumerated.aper_encoder;
	td->compare        = asn_DEF_NativeEnumerated.compare;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NativeEnumerated.per_constraints;
	td->elements       = asn_DEF_NativeEnumerated.elements;
	td->elements_count = asn_DEF_NativeEnumerated.elements_count;
     /* td->specifics      = asn_DEF_NativeEnumerated.specifics;	// Defined explicitly */
}

static void
drx_InactivityTimerSCPTM_r14_11_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

static int
drx_InactivityTimerSCPTM_r14_11_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

static asn_dec_rval_t
drx_InactivityTimerSCPTM_r14_11_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

static asn_enc_rval_t
drx_InactivityTimerSCPTM_r14_11_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

static asn_dec_rval_t
drx_InactivityTimerSCPTM_r14_11_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

static asn_enc_rval_t
drx_InactivityTimerSCPTM_r14_11_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static asn_dec_rval_t
drx_InactivityTimerSCPTM_r14_11_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

static asn_enc_rval_t
drx_InactivityTimerSCPTM_r14_11_encode_uper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->uper_encoder(td, constraints, structure, per_out);
}

static asn_enc_rval_t
drx_InactivityTimerSCPTM_r14_11_encode_aper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->aper_encoder(td, constraints, structure, per_out);
}

static asn_comp_rval_t * 
drx_InactivityTimerSCPTM_r14_11_compare(asn_TYPE_descriptor_t *td1,
		const void *structure1,
		asn_TYPE_descriptor_t *td2,
		const void *structure2) {
	asn_comp_rval_t * res  = NULL;
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td1);
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td2);
	res = td1->compare(td1, structure1, td2, structure2);
	return res;
}

static asn_dec_rval_t
drx_InactivityTimerSCPTM_r14_11_decode_aper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	drx_InactivityTimerSCPTM_r14_11_inherit_TYPE_descriptor(td);
	return td->aper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

static int
memb_sf10_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 9)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf20_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 19)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf32_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 31)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf40_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 39)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf64_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 63)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf80_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 79)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf128_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 127)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf160_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 159)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf256_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 255)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf320_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 319)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf512_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 511)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf640_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 639)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf1024_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 1023)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf2048_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 2047)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf4096_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 4095)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_sf8192_constraint_28(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 8191)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_per_constraints_t asn_PER_type_onDurationTimerSCPTM_r14_constr_2 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 3,  3,  0,  7 }	/* (0..7) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_type_drx_InactivityTimerSCPTM_r14_constr_11 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 4,  4,  0,  15 }	/* (0..15) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf10_constr_29 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 4,  4,  0,  9 }	/* (0..9) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf20_constr_30 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 5,  5,  0,  19 }	/* (0..19) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf32_constr_31 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 5,  5,  0,  31 }	/* (0..31) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf40_constr_32 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 6,  6,  0,  39 }	/* (0..39) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf64_constr_33 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 6,  6,  0,  63 }	/* (0..63) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf80_constr_34 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 7,  7,  0,  79 }	/* (0..79) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf128_constr_35 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 7,  7,  0,  127 }	/* (0..127) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf160_constr_36 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 8,  8,  0,  159 }	/* (0..159) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf256_constr_37 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 8,  8,  0,  255 }	/* (0..255) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf320_constr_38 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 9,  9,  0,  319 }	/* (0..319) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf512_constr_39 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 9,  9,  0,  511 }	/* (0..511) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf640_constr_40 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 10,  10,  0,  639 }	/* (0..639) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf1024_constr_41 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 10,  10,  0,  1023 }	/* (0..1023) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf2048_constr_42 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 11,  11,  0,  2047 }	/* (0..2047) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf4096_constr_43 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 12,  12,  0,  4095 }	/* (0..4095) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sf8192_constr_44 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 13,  13,  0,  8191 }	/* (0..8191) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_type_schedulingPeriodStartOffsetSCPTM_r14_constr_28 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 4,  4,  0,  15 }	/* (0..15) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_INTEGER_enum_map_t asn_MAP_onDurationTimerSCPTM_r14_value2enum_2[] = {
	{ 0,	6,	"psf300" },
	{ 1,	6,	"psf400" },
	{ 2,	6,	"psf500" },
	{ 3,	6,	"psf600" },
	{ 4,	6,	"psf800" },
	{ 5,	7,	"psf1000" },
	{ 6,	7,	"psf1200" },
	{ 7,	7,	"psf1600" }
};
static unsigned int asn_MAP_onDurationTimerSCPTM_r14_enum2value_2[] = {
	5,	/* psf1000(5) */
	6,	/* psf1200(6) */
	7,	/* psf1600(7) */
	0,	/* psf300(0) */
	1,	/* psf400(1) */
	2,	/* psf500(2) */
	3,	/* psf600(3) */
	4	/* psf800(4) */
};
static asn_INTEGER_specifics_t asn_SPC_onDurationTimerSCPTM_r14_specs_2 = {
	asn_MAP_onDurationTimerSCPTM_r14_value2enum_2,	/* "tag" => N; sorted by tag */
	asn_MAP_onDurationTimerSCPTM_r14_enum2value_2,	/* N => "tag"; sorted by N */
	8,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1,	/* Strict enumeration */
	0,	/* Native long size */
	0
};
static ber_tlv_tag_t asn_DEF_onDurationTimerSCPTM_r14_tags_2[] = {
	(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_onDurationTimerSCPTM_r14_2 = {
	"onDurationTimerSCPTM-r14",
	"onDurationTimerSCPTM-r14",
	onDurationTimerSCPTM_r14_2_free,
	onDurationTimerSCPTM_r14_2_print,
	onDurationTimerSCPTM_r14_2_constraint,
	onDurationTimerSCPTM_r14_2_decode_ber,
	onDurationTimerSCPTM_r14_2_encode_der,
	onDurationTimerSCPTM_r14_2_decode_xer,
	onDurationTimerSCPTM_r14_2_encode_xer,
	onDurationTimerSCPTM_r14_2_decode_uper,
	onDurationTimerSCPTM_r14_2_encode_uper,
	onDurationTimerSCPTM_r14_2_decode_aper,
	onDurationTimerSCPTM_r14_2_encode_aper,
	onDurationTimerSCPTM_r14_2_compare,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_onDurationTimerSCPTM_r14_tags_2,
	sizeof(asn_DEF_onDurationTimerSCPTM_r14_tags_2)
		/sizeof(asn_DEF_onDurationTimerSCPTM_r14_tags_2[0]) - 1, /* 1 */
	asn_DEF_onDurationTimerSCPTM_r14_tags_2,	/* Same as above */
	sizeof(asn_DEF_onDurationTimerSCPTM_r14_tags_2)
		/sizeof(asn_DEF_onDurationTimerSCPTM_r14_tags_2[0]), /* 2 */
	&asn_PER_type_onDurationTimerSCPTM_r14_constr_2,
	0, 0,	/* Defined elsewhere */
	&asn_SPC_onDurationTimerSCPTM_r14_specs_2	/* Additional specs */
};

static asn_INTEGER_enum_map_t asn_MAP_drx_InactivityTimerSCPTM_r14_value2enum_11[] = {
	{ 0,	4,	"psf0" },
	{ 1,	4,	"psf1" },
	{ 2,	4,	"psf2" },
	{ 3,	4,	"psf4" },
	{ 4,	4,	"psf8" },
	{ 5,	5,	"psf16" },
	{ 6,	5,	"psf32" },
	{ 7,	5,	"psf64" },
	{ 8,	6,	"psf128" },
	{ 9,	6,	"psf256" },
	{ 10,	5,	"ps512" },
	{ 11,	7,	"psf1024" },
	{ 12,	7,	"psf2048" },
	{ 13,	7,	"psf4096" },
	{ 14,	7,	"psf8192" },
	{ 15,	8,	"psf16384" }
};
static unsigned int asn_MAP_drx_InactivityTimerSCPTM_r14_enum2value_11[] = {
	10,	/* ps512(10) */
	0,	/* psf0(0) */
	1,	/* psf1(1) */
	11,	/* psf1024(11) */
	8,	/* psf128(8) */
	5,	/* psf16(5) */
	15,	/* psf16384(15) */
	2,	/* psf2(2) */
	12,	/* psf2048(12) */
	9,	/* psf256(9) */
	6,	/* psf32(6) */
	3,	/* psf4(3) */
	13,	/* psf4096(13) */
	7,	/* psf64(7) */
	4,	/* psf8(4) */
	14	/* psf8192(14) */
};
static asn_INTEGER_specifics_t asn_SPC_drx_InactivityTimerSCPTM_r14_specs_11 = {
	asn_MAP_drx_InactivityTimerSCPTM_r14_value2enum_11,	/* "tag" => N; sorted by tag */
	asn_MAP_drx_InactivityTimerSCPTM_r14_enum2value_11,	/* N => "tag"; sorted by N */
	16,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1,	/* Strict enumeration */
	0,	/* Native long size */
	0
};
static ber_tlv_tag_t asn_DEF_drx_InactivityTimerSCPTM_r14_tags_11[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_drx_InactivityTimerSCPTM_r14_11 = {
	"drx-InactivityTimerSCPTM-r14",
	"drx-InactivityTimerSCPTM-r14",
	drx_InactivityTimerSCPTM_r14_11_free,
	drx_InactivityTimerSCPTM_r14_11_print,
	drx_InactivityTimerSCPTM_r14_11_constraint,
	drx_InactivityTimerSCPTM_r14_11_decode_ber,
	drx_InactivityTimerSCPTM_r14_11_encode_der,
	drx_InactivityTimerSCPTM_r14_11_decode_xer,
	drx_InactivityTimerSCPTM_r14_11_encode_xer,
	drx_InactivityTimerSCPTM_r14_11_decode_uper,
	drx_InactivityTimerSCPTM_r14_11_encode_uper,
	drx_InactivityTimerSCPTM_r14_11_decode_aper,
	drx_InactivityTimerSCPTM_r14_11_encode_aper,
	drx_InactivityTimerSCPTM_r14_11_compare,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_drx_InactivityTimerSCPTM_r14_tags_11,
	sizeof(asn_DEF_drx_InactivityTimerSCPTM_r14_tags_11)
		/sizeof(asn_DEF_drx_InactivityTimerSCPTM_r14_tags_11[0]) - 1, /* 1 */
	asn_DEF_drx_InactivityTimerSCPTM_r14_tags_11,	/* Same as above */
	sizeof(asn_DEF_drx_InactivityTimerSCPTM_r14_tags_11)
		/sizeof(asn_DEF_drx_InactivityTimerSCPTM_r14_tags_11[0]), /* 2 */
	&asn_PER_type_drx_InactivityTimerSCPTM_r14_constr_11,
	0, 0,	/* Defined elsewhere */
	&asn_SPC_drx_InactivityTimerSCPTM_r14_specs_11	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_schedulingPeriodStartOffsetSCPTM_r14_28[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf10),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf10_constraint_28,
		&asn_PER_memb_sf10_constr_29,
		0,
		"sf10"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf20),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf20_constraint_28,
		&asn_PER_memb_sf20_constr_30,
		0,
		"sf20"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf32),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf32_constraint_28,
		&asn_PER_memb_sf32_constr_31,
		0,
		"sf32"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf40),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf40_constraint_28,
		&asn_PER_memb_sf40_constr_32,
		0,
		"sf40"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf64),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf64_constraint_28,
		&asn_PER_memb_sf64_constr_33,
		0,
		"sf64"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf80),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf80_constraint_28,
		&asn_PER_memb_sf80_constr_34,
		0,
		"sf80"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf128),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf128_constraint_28,
		&asn_PER_memb_sf128_constr_35,
		0,
		"sf128"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf160),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf160_constraint_28,
		&asn_PER_memb_sf160_constr_36,
		0,
		"sf160"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf256),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf256_constraint_28,
		&asn_PER_memb_sf256_constr_37,
		0,
		"sf256"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf320),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf320_constraint_28,
		&asn_PER_memb_sf320_constr_38,
		0,
		"sf320"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf512),
		(ASN_TAG_CLASS_CONTEXT | (10 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf512_constraint_28,
		&asn_PER_memb_sf512_constr_39,
		0,
		"sf512"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf640),
		(ASN_TAG_CLASS_CONTEXT | (11 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf640_constraint_28,
		&asn_PER_memb_sf640_constr_40,
		0,
		"sf640"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf1024),
		(ASN_TAG_CLASS_CONTEXT | (12 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf1024_constraint_28,
		&asn_PER_memb_sf1024_constr_41,
		0,
		"sf1024"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf2048),
		(ASN_TAG_CLASS_CONTEXT | (13 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf2048_constraint_28,
		&asn_PER_memb_sf2048_constr_42,
		0,
		"sf2048"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf4096),
		(ASN_TAG_CLASS_CONTEXT | (14 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf4096_constraint_28,
		&asn_PER_memb_sf4096_constr_43,
		0,
		"sf4096"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, choice.sf8192),
		(ASN_TAG_CLASS_CONTEXT | (15 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_sf8192_constraint_28,
		&asn_PER_memb_sf8192_constr_44,
		0,
		"sf8192"
		},
};
static asn_TYPE_tag2member_t asn_MAP_schedulingPeriodStartOffsetSCPTM_r14_tag2el_28[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* sf10 at 10088 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* sf20 at 10089 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* sf32 at 10090 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* sf40 at 10091 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* sf64 at 10092 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* sf80 at 10093 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* sf128 at 10094 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* sf160 at 10095 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* sf256 at 10096 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 }, /* sf320 at 10097 */
    { (ASN_TAG_CLASS_CONTEXT | (10 << 2)), 10, 0, 0 }, /* sf512 at 10098 */
    { (ASN_TAG_CLASS_CONTEXT | (11 << 2)), 11, 0, 0 }, /* sf640 at 10099 */
    { (ASN_TAG_CLASS_CONTEXT | (12 << 2)), 12, 0, 0 }, /* sf1024 at 10100 */
    { (ASN_TAG_CLASS_CONTEXT | (13 << 2)), 13, 0, 0 }, /* sf2048 at 10101 */
    { (ASN_TAG_CLASS_CONTEXT | (14 << 2)), 14, 0, 0 }, /* sf4096 at 10102 */
    { (ASN_TAG_CLASS_CONTEXT | (15 << 2)), 15, 0, 0 } /* sf8192 at 10103 */
};
static asn_CHOICE_specifics_t asn_SPC_schedulingPeriodStartOffsetSCPTM_r14_specs_28 = {
	sizeof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14),
	offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, _asn_ctx),
	offsetof(struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14, present),
	sizeof(((struct SC_MTCH_SchedulingInfo_BR_r14__schedulingPeriodStartOffsetSCPTM_r14 *)0)->present),
	asn_MAP_schedulingPeriodStartOffsetSCPTM_r14_tag2el_28,
	16,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_schedulingPeriodStartOffsetSCPTM_r14_28 = {
	"schedulingPeriodStartOffsetSCPTM-r14",
	"schedulingPeriodStartOffsetSCPTM-r14",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	CHOICE_decode_uper,
	CHOICE_encode_uper,
	CHOICE_decode_aper,
	CHOICE_encode_aper,
	CHOICE_compare,
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	&asn_PER_type_schedulingPeriodStartOffsetSCPTM_r14_constr_28,
	asn_MBR_schedulingPeriodStartOffsetSCPTM_r14_28,
	16,	/* Elements count */
	&asn_SPC_schedulingPeriodStartOffsetSCPTM_r14_specs_28	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_SC_MTCH_SchedulingInfo_BR_r14_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14, onDurationTimerSCPTM_r14),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_onDurationTimerSCPTM_r14_2,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"onDurationTimerSCPTM-r14"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14, drx_InactivityTimerSCPTM_r14),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_drx_InactivityTimerSCPTM_r14_11,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"drx-InactivityTimerSCPTM-r14"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SC_MTCH_SchedulingInfo_BR_r14, schedulingPeriodStartOffsetSCPTM_r14),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_schedulingPeriodStartOffsetSCPTM_r14_28,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"schedulingPeriodStartOffsetSCPTM-r14"
		},
};
static ber_tlv_tag_t asn_DEF_SC_MTCH_SchedulingInfo_BR_r14_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SC_MTCH_SchedulingInfo_BR_r14_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* onDurationTimerSCPTM-r14 at 10081 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* drx-InactivityTimerSCPTM-r14 at 10084 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* schedulingPeriodStartOffsetSCPTM-r14 at 10088 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SC_MTCH_SchedulingInfo_BR_r14_specs_1 = {
	sizeof(struct SC_MTCH_SchedulingInfo_BR_r14),
	offsetof(struct SC_MTCH_SchedulingInfo_BR_r14, _asn_ctx),
	asn_MAP_SC_MTCH_SchedulingInfo_BR_r14_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	2,	/* Start extensions */
	4	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SC_MTCH_SchedulingInfo_BR_r14 = {
	"SC-MTCH-SchedulingInfo-BR-r14",
	"SC-MTCH-SchedulingInfo-BR-r14",
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
	asn_DEF_SC_MTCH_SchedulingInfo_BR_r14_tags_1,
	sizeof(asn_DEF_SC_MTCH_SchedulingInfo_BR_r14_tags_1)
		/sizeof(asn_DEF_SC_MTCH_SchedulingInfo_BR_r14_tags_1[0]), /* 1 */
	asn_DEF_SC_MTCH_SchedulingInfo_BR_r14_tags_1,	/* Same as above */
	sizeof(asn_DEF_SC_MTCH_SchedulingInfo_BR_r14_tags_1)
		/sizeof(asn_DEF_SC_MTCH_SchedulingInfo_BR_r14_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SC_MTCH_SchedulingInfo_BR_r14_1,
	3,	/* Elements count */
	&asn_SPC_SC_MTCH_SchedulingInfo_BR_r14_specs_1	/* Additional specs */
};
