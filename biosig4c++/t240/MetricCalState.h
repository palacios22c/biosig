/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "FEF-IntermediateDraft"
 * 	found in "../annexb-snacc-122001.asn1"
 */

#ifndef	_MetricCalState_H_
#define	_MetricCalState_H_


#include <asn_application.h>

/* Including external dependencies */
#include <INTEGER.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum MetricCalState {
	MetricCalState_not_calibrated	= 0,
	MetricCalState_cal_required	= 1,
	MetricCalState_calibrated	= 2
} e_MetricCalState;

/* MetricCalState */
typedef INTEGER_t	 MetricCalState_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MetricCalState;
asn_struct_free_f MetricCalState_free;
asn_struct_print_f MetricCalState_print;
asn_constr_check_f MetricCalState_constraint;
ber_type_decoder_f MetricCalState_decode_ber;
der_type_encoder_f MetricCalState_encode_der;
xer_type_decoder_f MetricCalState_decode_xer;
xer_type_encoder_f MetricCalState_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _MetricCalState_H_ */