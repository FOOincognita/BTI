#ifndef _BTIUTIL_H
#define _BTIUTIL_H

#include "BTICard.h"
#include "BTI717.h"
#include "BTI429.h"

#include <stdint.h>
#include <strings.h>

//	Definitions

#define MAX_WPS 8192

#ifndef	TRUE
#define	TRUE 1
#endif

#ifndef	FALSE
#define	FALSE 0
#endif

#ifndef BOOL
#define BOOL int
#endif

//	Structures and Enumerations

typedef struct datafile_t {
	uint16_t streamactivity;		//see ACTSTREAM_ below
	uint16_t channel;
	uint16_t wps;
	uint16_t updatedSubfrm;
	uint32_t timestamp;				//last time file was updated
	uint32_t timestamph;			//last time file was updated
	uint16_t sfdata[4][MAX_WPS];	//allocate max.
} datafile_t;

typedef struct chan717Info_t {
		int        valid;
		int        channel;
		int        wps;
		uint32_t   lastupdate;		//Time of last update
		datafile_t datafile;		//Most recent SF data info
		uint32_t   recordcnt;		//Records for this channel
		uint32_t   errorcnt;		//Total errors for this channel
} chan717Info_t;

typedef struct chan429Info_t {
	int        valid;
	int        channel;
	int        btx;
	uint32_t   timestamp;			//
	uint32_t   timestamph;			//
	uint32_t   recordcnt;			//Records for this channel
	uint32_t   errorcnt;			//Total errors for this channel
	uint32_t   msg;
	uint32_t   lastupdate;			//Time of last update
} chan429Info_t;

typedef struct {
	uint16_t type;					//
	uint16_t count;					//
	uint32_t timestamp;				//
	uint32_t timestamph;			//
	uint16_t activity;				//
	uint16_t subframe;				//Valid in all versions
	uint16_t resv;					//(future: superframe) Valid in all versions
	uint16_t datacount;				//
	uint16_t data[8192];			//Variable length (don't exceed data[datacount-1])
} SEQRECORD717SF;

#define SEQTYPE_717SF 0x0009		//Sequential record type is ARINC 717 Subframe

#ifndef LPSEQRECORD717SF
typedef SEQRECORD717SF * LPSEQRECORD717SF;
#endif

enum {
	ACTSTREAM_SERVERFAULT=0x1,
	ACTSTREAM_SUBF1VALID=0x10,
	ACTSTREAM_SUBF2VALID=0x20,
	ACTSTREAM_SUBF3VALID=0x40,
	ACTSTREAM_SUBF4VALID=0x80,
};

////////////////////////////////////////////////////////////
// BTIDriver Sequential Record Methods
////////////////////////////////////////////////////////////

ERRVAL BTIUTIL_SeqFindCheckValidType(UINT16 seqtype) {
	seqtype &= SEQTYPE_MASK;

//	if (seqtype == SEQTYPE_1553) return(ERR_NONE);
	if (seqtype == SEQTYPE_429)  return(ERR_NONE);
//	if (seqtype == SEQTYPE_717)  return(ERR_NONE);
	if (seqtype == SEQTYPE_717SF)return(ERR_NONE);
//	if (seqtype == SEQTYPE_708)  return(ERR_NONE);
//	if (seqtype == SEQTYPE_CSDB) return(ERR_NONE);
//	if (seqtype == SEQTYPE_DIO)  return(ERR_NONE);
//	if (seqtype == SEQTYPE_EBR)  return(ERR_NONE);
//	if (seqtype == SEQTYPE_USER) return(ERR_NONE);

	return(ERR_SEQTYPE);
}

ERRVAL BTIUTIL_SeqFindNext(LPUINT16 *pRecord,LPUINT16 seqtype,LPSEQFINDINFO sfinfo) {
	ERRVAL errval;
	LPUINT16 pSeqBuf;

	if (!sfinfo) return(ERR_SEQFINDINFO);

	for (pSeqBuf = sfinfo->pRecNext; pSeqBuf < sfinfo->pRecLast;) {
		// Check for a known record type
		errval = BTIUTIL_SeqFindCheckValidType(pSeqBuf[0]);
		if (errval) return(errval);

		//Advance the pointer in the sfinfo struct
		sfinfo->pRecNext += pSeqBuf[1];

		if (pRecord) *pRecord = pSeqBuf;
		if (seqtype) *seqtype = pSeqBuf[0] & SEQTYPE_MASK;

		return(ERR_NONE);
	}
	return(ERR_SEQNEXT);
}

#endif //_BTIUTIL_H
