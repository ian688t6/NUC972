#include "mp4.h"

int read_esds_box(mp4_bits_t *bs, esds_box_t *esdsbox)
{
	uint16_t temp;

	esdsbox->struct_size = esdsbox->boxheader->struct_size;

	esdsbox->tag = mp4_bs_read_u8(bs);
	esdsbox->Length_Field = mp4_bs_read_u8(bs);
	esdsbox->ES_ID = mp4_bs_read_u16(bs);
	temp = mp4_bs_read_u8(bs);
	esdsbox->streamDependenceFlag = (temp & 0x80) ? 1 : 0;
	esdsbox->URL_Flag = (temp & 0x40) ? 1 : 0;
	esdsbox->OCRstreamFlag = (temp & 0x20) ? 1 : 0;
	esdsbox->streamPriority = temp & 0x1f;
	esdsbox->struct_size += 5;

	esdsbox->DecoderConfigDescriptor_tag = mp4_bs_read_u8(bs);
	esdsbox->Length_Field_1 = mp4_bs_read_u8(bs);
	esdsbox->objectTypeIndication = mp4_bs_read_u8(bs);
	temp = mp4_bs_read_u8(bs);
	esdsbox->streamType = (temp >> 2) & 0x3f;
	esdsbox->upStream = (temp & 0x02) ? 1 : 0;
	esdsbox->bufferSizeDB = mp4_bs_read_u24(bs);
	esdsbox->maxBitrate = mp4_bs_read_u32(bs);
	esdsbox->avgBitrate = mp4_bs_read_u32(bs);
	esdsbox->struct_size += 15;

	esdsbox->DecSpecificInfotag = mp4_bs_read_u8(bs);
	esdsbox->Length_Field_2 = mp4_bs_read_u8(bs);
	temp = mp4_bs_read_u16(bs);
	esdsbox->audioObjectType_2_GASpecificConfig = (temp >> 11) & 0x1f;
	esdsbox->samplingFrequencyIndex = (temp >> 7) & 0xf;
	esdsbox->channelConfiguration = (temp >> 3) & 0xf;
	esdsbox->cpConfig = (temp >> 1) & 0x3;
	esdsbox->directMapping = temp & 0x1;
	esdsbox->struct_size += 4;

	esdsbox->SLConfigDescrTag = mp4_bs_read_u8(bs);
	esdsbox->Length_Field_3 = mp4_bs_read_u8(bs);
	esdsbox->predefined = mp4_bs_read_u8(bs);
	esdsbox->struct_size += 3;

	return esdsbox->struct_size;
}

int write_esds_box(mp4_bits_t *bs, esds_box_t *esdsbox)
{
	uint64_t pos, tail_pos;
	int32_t temp;

	pos = mp4_bs_get_position(bs);

	esdsbox->struct_size = write_fullbox_header(bs, esdsbox->boxheader);

	mp4_bs_write_u8(bs, esdsbox->tag);
	mp4_bs_write_u8(bs, esdsbox->Length_Field);
	mp4_bs_write_u16(bs, esdsbox->ES_ID);
	temp = esdsbox->streamDependenceFlag ? 0x80 : 0x00;
	temp |= esdsbox->URL_Flag ? 0x40 : 0x00;
	temp |= esdsbox->OCRstreamFlag ? 0x20 : 0x00;
	temp |= esdsbox->streamPriority & 0x1f;
	mp4_bs_write_u8(bs, temp);
	esdsbox->struct_size += 5;

	mp4_bs_write_u8(bs, esdsbox->DecoderConfigDescriptor_tag);
	mp4_bs_write_u8(bs, esdsbox->Length_Field_1);
	mp4_bs_write_u8(bs, esdsbox->objectTypeIndication);

	temp = (esdsbox->streamType << 2) | (esdsbox->upStream << 1);
	mp4_bs_write_u8(bs, temp);
	mp4_bs_write_u24(bs, esdsbox->bufferSizeDB);
	mp4_bs_write_u32(bs, esdsbox->maxBitrate);
	mp4_bs_write_u32(bs, esdsbox->avgBitrate);
	esdsbox->struct_size += 15;

	mp4_bs_write_u8(bs, esdsbox->DecSpecificInfotag);
	mp4_bs_write_u8(bs, esdsbox->Length_Field_2);

	temp = (esdsbox->audioObjectType_2_GASpecificConfig << 11) | (esdsbox->samplingFrequencyIndex << 7) | (esdsbox->channelConfiguration << 3)
	                | (esdsbox->cpConfig << 1) | esdsbox->directMapping;
	mp4_bs_write_u16(bs, temp);
	esdsbox->struct_size += 4;

	mp4_bs_write_u8(bs, esdsbox->SLConfigDescrTag);
	mp4_bs_write_u8(bs, esdsbox->Length_Field_3);
	mp4_bs_write_u8(bs, esdsbox->predefined);
	esdsbox->struct_size += 3;

	esdsbox->boxheader->size = esdsbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, esdsbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return esdsbox->struct_size;
}

int alloc_struct_esds_box(esds_box_t **esdsbox)
{
	esds_box_t * esdsbox_t = *esdsbox;
	if ((esdsbox_t = (esds_box_t *)calloc(1, sizeof(esds_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(esdsbox_t->boxheader));
	*esdsbox = esdsbox_t;
	return 1;
}

int free_struct_esds_box(esds_box_t *esdsbox)
{
	if (esdsbox) {
		free_struct_fullbox_header(esdsbox->boxheader);
		free(esdsbox);
		esdsbox = NULL;
	}
	return 1;
}
