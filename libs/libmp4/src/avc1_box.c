#include "mp4.h"

int read_avc1_box(mp4_bits_t *bs, avc1_box_t *avc1box)
{
	box_header_t *boxhead;
	avcc_box_t *avccbox;

	avc1box->struct_size = avc1box->boxheader->struct_size;

	avc1box->reserved[0] = mp4_bs_read_u8(bs);
	avc1box->reserved[1] = mp4_bs_read_u8(bs);
	avc1box->reserved[2] = mp4_bs_read_u8(bs);
	avc1box->reserved[3] = mp4_bs_read_u8(bs);
	avc1box->reserved[4] = mp4_bs_read_u8(bs);
	avc1box->reserved[5] = mp4_bs_read_u8(bs);

	avc1box->data_reference_index = mp4_bs_read_u16(bs);
	avc1box->pre_defined = mp4_bs_read_u16(bs);
	avc1box->reserved1 = mp4_bs_read_u16(bs);
	avc1box->struct_size += 12;

	avc1box->pre_defined1[0] = mp4_bs_read_u32(bs);
	avc1box->pre_defined1[1] = mp4_bs_read_u32(bs);
	avc1box->pre_defined1[2] = mp4_bs_read_u32(bs);
	avc1box->struct_size += 12;

	avc1box->width = mp4_bs_read_u16(bs);
	avc1box->height = mp4_bs_read_u16(bs);
	avc1box->struct_size += 4;

	avc1box->horizresolution = mp4_bs_read_u32(bs);
	avc1box->vertresolution = mp4_bs_read_u32(bs);
	avc1box->reserved2 = mp4_bs_read_u32(bs);
	avc1box->frame_count = mp4_bs_read_u16(bs);
	avc1box->struct_size += 14;

	mp4_bs_read_data(bs, (char *)avc1box->compressorname, 32);
	avc1box->depth = mp4_bs_read_u16(bs);
	avc1box->pre_defined2 = mp4_bs_read_u16(bs);

	avc1box->struct_size += 36;

	alloc_struct_box_header(&boxhead);
	read_box_header(bs, boxhead);

	alloc_struct_avcc_box(&avccbox);
	memcpy(avccbox->boxheader, boxhead, sizeof(box_header_t));
	avc1box->struct_size += read_avcc_box(bs, avccbox);
	avc1box->avccbox = avccbox;

	free_struct_box_header(boxhead);

	return avc1box->struct_size;
}

int write_avc1_box(mp4_bits_t *bs, avc1_box_t *avc1box)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	avc1box->struct_size = write_box_header(bs, avc1box->boxheader);

	mp4_bs_write_u8(bs, avc1box->reserved[0]);
	mp4_bs_write_u8(bs, avc1box->reserved[1]);
	mp4_bs_write_u8(bs, avc1box->reserved[2]);
	mp4_bs_write_u8(bs, avc1box->reserved[3]);
	mp4_bs_write_u8(bs, avc1box->reserved[4]);
	mp4_bs_write_u8(bs, avc1box->reserved[5]);

	mp4_bs_write_u16(bs, avc1box->data_reference_index);
	mp4_bs_write_u16(bs, avc1box->pre_defined);
	mp4_bs_write_u16(bs, avc1box->reserved1);
	avc1box->struct_size += 12;

	mp4_bs_write_u32(bs, avc1box->pre_defined1[0]);
	mp4_bs_write_u32(bs, avc1box->pre_defined1[1]);
	mp4_bs_write_u32(bs, avc1box->pre_defined1[2]);
	avc1box->struct_size += 12;

	mp4_bs_write_u16(bs, avc1box->width);
	mp4_bs_write_u16(bs, avc1box->height);
	avc1box->struct_size += 4;

	mp4_bs_write_u32(bs, avc1box->horizresolution);
	mp4_bs_write_u32(bs, avc1box->vertresolution);
	mp4_bs_write_u32(bs, avc1box->reserved2);
	mp4_bs_write_u16(bs, avc1box->frame_count);
	avc1box->struct_size += 14;

	mp4_bs_write_data(bs, (char *)avc1box->compressorname, 32);
	mp4_bs_write_u16(bs, avc1box->depth);
	mp4_bs_write_u16(bs, avc1box->pre_defined2);

	avc1box->struct_size += 36;

	if (avc1box->avccbox) {
		avc1box->struct_size += write_avcc_box(bs, avc1box->avccbox);
	}

	avc1box->boxheader->size = avc1box->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, avc1box->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return avc1box->struct_size;
}

int alloc_struct_avc1_box(avc1_box_t **avc1box)
{
	avc1_box_t *avc1box_t = *avc1box;

	if ((avc1box_t = (avc1_box_t *)calloc(1, sizeof(avc1_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(avc1box_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*avc1box = avc1box_t;
	return 1;
}

int free_struct_avc1_box(avc1_box_t *avc1box)
{
	if (avc1box) {
		if (avc1box->boxheader) {
			free_struct_box_header(avc1box->boxheader);
		}
		free(avc1box);
		avc1box = NULL;
	}

	return 1;

}
