#include "mp4.h"

int read_avcc_box(mp4_bits_t *bs, avcc_box_t *avccbox)
{
	avccbox->struct_size = avccbox->boxheader->struct_size;

	avccbox->configurationVersion = mp4_bs_read_u8(bs);
	avccbox->AVCProfileIndication = mp4_bs_read_u8(bs);
	avccbox->profile_compatibility = mp4_bs_read_u8(bs);
	avccbox->AVCLevelIndication = mp4_bs_read_u8(bs);
	avccbox->lengthSizeMinusOne = mp4_bs_read_u8(bs) & 0x03;

	avccbox->numOfSequenceParameterSets = mp4_bs_read_u8(bs) & 0x1F;
//	avccbox->sequenceParameterSetLength = (uint16_t *)malloc(sizeof(uint16_t) * avccbox->numOfSequenceParameterSets);
//	avccbox->sequenceParameterSetNALUnit = (uint8_t **)((malloc(sizeof(uint8_t *) * avccbox->numOfSequenceParameterSets)));

	avccbox->struct_size += 6;
	//for (i = 0; i < avccbox->numOfSequenceParameterSets; i ++)
	//{
	avccbox->sequenceParameterSetLength = mp4_bs_read_u16(bs);
	avccbox->struct_size += 2;

	avccbox->sequenceParameterSetNALUnit = (uint8_t *)malloc(avccbox->sequenceParameterSetLength);
	mp4_bs_read_data(bs, (char *)avccbox->sequenceParameterSetNALUnit, avccbox->sequenceParameterSetLength);
	avccbox->struct_size += avccbox->sequenceParameterSetLength;
	//}

	avccbox->numOfPictureParameterSets = mp4_bs_read_u8(bs);
//	avccbox->pictureParameterSetLength = (uint16_t *)malloc(sizeof(uint16_t) * avccbox->numOfPictureParameterSets);
//	avccbox->pictureParameterSetNALUnit = (uint8_t **)((malloc(sizeof(uint8_t *) * avccbox->numOfPictureParameterSets)));
	avccbox->struct_size += 1;
//	for (i = 0; i < avccbox->numOfPictureParameterSets; i ++)
//	{
	avccbox->pictureParameterSetLength = mp4_bs_read_u16(bs);
	avccbox->struct_size += 2;
	avccbox->pictureParameterSetNALUnit = (uint8_t *)malloc(avccbox->pictureParameterSetLength);
	mp4_bs_read_data(bs, (char *)avccbox->pictureParameterSetNALUnit, avccbox->pictureParameterSetLength);
	avccbox->struct_size += avccbox->pictureParameterSetLength;
//	}

	return avccbox->struct_size;
}

int write_avcc_box(mp4_bits_t *bs, avcc_box_t *avccbox)
{
	uint64_t pos, tail_pos;
	int i;

	pos = mp4_bs_get_position(bs);

	avccbox->struct_size = write_box_header(bs, avccbox->boxheader);

	mp4_bs_write_u8(bs, avccbox->configurationVersion);
	mp4_bs_write_u8(bs, avccbox->AVCProfileIndication);
	mp4_bs_write_u8(bs, avccbox->profile_compatibility);
	mp4_bs_write_u8(bs, avccbox->AVCLevelIndication);

	mp4_bs_write_u8(bs, avccbox->lengthSizeMinusOne & 0x03);

	mp4_bs_write_u8(bs, avccbox->numOfSequenceParameterSets & 0x1F);

	avccbox->struct_size += 6;
	for (i = 0; i < avccbox->numOfSequenceParameterSets; i++) {
		mp4_bs_write_u16(bs, avccbox->sequenceParameterSetLength);
		avccbox->struct_size += 2;

		mp4_bs_write_data(bs, (char *)avccbox->sequenceParameterSetNALUnit, avccbox->sequenceParameterSetLength);
		avccbox->struct_size += avccbox->sequenceParameterSetLength;
	}

	mp4_bs_write_u8(bs, avccbox->numOfPictureParameterSets);
	avccbox->struct_size += 1;
	for (i = 0; i < avccbox->numOfPictureParameterSets; i++) {
		mp4_bs_write_u16(bs, avccbox->pictureParameterSetLength);
		avccbox->struct_size += 2;
		mp4_bs_write_data(bs, (char *)avccbox->pictureParameterSetNALUnit, avccbox->pictureParameterSetLength);
		avccbox->struct_size += avccbox->pictureParameterSetLength;
	}

	avccbox->boxheader->size = avccbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, avccbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return avccbox->struct_size;
}

int alloc_struct_avcc_box(avcc_box_t **avccbox)
{
	avcc_box_t * avccbox_t = *avccbox;

	if ((avccbox_t = (avcc_box_t *)calloc(1, sizeof(avcc_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(avccbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}

	*avccbox = avccbox_t;
	return 1;
}

int free_struct_avcc_box(avcc_box_t *avccbox)
{
	//int i;

	if (avccbox) {
		if (avccbox->boxheader) {
			free_struct_box_header(avccbox->boxheader);
		}

//		for (i = 0; i < avccbox->numOfSequenceParameterSets; i ++)
//		{
		if (avccbox->sequenceParameterSetNALUnit) {
			free(avccbox->sequenceParameterSetNALUnit);
			avccbox->sequenceParameterSetNALUnit = NULL;
		}
//		}

//		for (i = 0; i < avccbox->numOfPictureParameterSets; i ++)
//		{
		if (avccbox->pictureParameterSetNALUnit) {
			free(avccbox->pictureParameterSetNALUnit);
			avccbox->pictureParameterSetNALUnit = NULL;
		}
//		}

		free(avccbox);
		avccbox = NULL;
	}
	return 1;
}
