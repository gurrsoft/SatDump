/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "FANSACTwoWayDataLinkCommunications"
 * 	found in "../../../libacars.asn1/fans-cpdlc.asn1"
 * 	`asn1c -fcompound-names -fincludes-quoted -gen-PER`
 */

#ifndef	_FANSLegDistance_H_
#define	_FANSLegDistance_H_


#include "asn_application.h"

/* Including external dependencies */
#include "FANSLegDistanceEnglish.h"
#include "FANSLegDistanceMetric.h"
#include "constr_CHOICE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum FANSLegDistance_PR {
	FANSLegDistance_PR_NOTHING,	/* No components present */
	FANSLegDistance_PR_legDistanceEnglish,
	FANSLegDistance_PR_legDistanceMetric
} FANSLegDistance_PR;

/* FANSLegDistance */
typedef struct FANSLegDistance {
	FANSLegDistance_PR present;
	union FANSLegDistance_u {
		FANSLegDistanceEnglish_t	 legDistanceEnglish;
		FANSLegDistanceMetric_t	 legDistanceMetric;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} FANSLegDistance_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_FANSLegDistance;

#ifdef __cplusplus
}
#endif

#endif	/* _FANSLegDistance_H_ */
#include "asn_internal.h"