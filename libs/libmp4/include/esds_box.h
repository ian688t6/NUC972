#ifndef __ESDS_BOX_H__
#define __ESDS_BOX_H__

typedef struct tag_esds_box {
	fullbox_header_t *boxheader;
	uint8_t tag; //8   = 0x03
	uint8_t Length_Field; //8   Length Field
	uint16_t ES_ID;  //bit(16)   ES_ID:
	uint8_t streamDependenceFlag; //bit(1)  steamDependenceFlag
	uint8_t URL_Flag;    //bit(1)  :URL_Flag
	uint8_t OCRstreamFlag; //bit(1)
	uint8_t streamPriority; //bit(5)

	uint8_t DecoderConfigDescriptor_tag; //8  = 0x04
	uint8_t Length_Field_1;      //8 = 0x11
	uint8_t objectTypeIndication; //8 = 0x40  14496-1 Table8, 0x40Audio ISO/IEC 14496-3
	uint8_t streamType; //6   Audio Stream, 14496-1 Table9
	uint8_t upStream;    //1
	uint8_t reserved;    //1
	uint32_t bufferSizeDB;    //24
	uint32_t maxBitrate;  //32
	uint32_t avgBitrate; //32

	uint8_t DecSpecificInfotag; //8 = 0x05
	uint8_t Length_Field_2; //8  = 0x02
	uint8_t audioObjectType_2_GASpecificConfig; //5         //AAC-LC = 0x02
	uint8_t samplingFrequencyIndex; //4                     //44100 = 0x04
	uint8_t channelConfiguration; //4                       // = 2
	uint8_t cpConfig; //2                                   
	uint8_t directMapping; // 1

	uint8_t SLConfigDescrTag; //8  = 0x06
	uint8_t Length_Field_3; //8    = 0x01
	uint8_t predefined; //8  predefined 0x02 Reserved for use in MP4 files

	uint32_t struct_size;
} esds_box_t;

int read_esds_box(mp4_bits_t *bs, esds_box_t *esdsbox);
int write_esds_box(mp4_bits_t *bs, esds_box_t *esdsbox);
int alloc_struct_esds_box(esds_box_t **esdsbox);
int free_struct_esds_box(esds_box_t *esdsbox);

#endif
