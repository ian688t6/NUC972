#include "mp4.h"

int read_mp4a_box(mp4_bits_t *bs, mp4a_box_t *mp4abox)
{
	fullbox_header_t *fullboxhead;
	esds_box_t *esdsbox;

	mp4abox->struct_size = mp4abox->boxheader->struct_size;

	mp4abox->reseved[0] = mp4_bs_read_u8(bs);
	mp4abox->reseved[1] = mp4_bs_read_u8(bs);
	mp4abox->reseved[2] = mp4_bs_read_u8(bs);
	mp4abox->reseved[3] = mp4_bs_read_u8(bs);
	mp4abox->reseved[4] = mp4_bs_read_u8(bs);
	mp4abox->reseved[5] = mp4_bs_read_u8(bs);
	mp4abox->data_reference_index = mp4_bs_read_u16(bs);
	mp4abox->reseved1[0] = mp4_bs_read_u32(bs);
	mp4abox->reseved1[1] = mp4_bs_read_u32(bs);
	mp4abox->chanlecount = mp4_bs_read_u16(bs);
	mp4abox->samplesize = mp4_bs_read_u16(bs);
	mp4abox->reseved2 = mp4_bs_read_u32(bs);
	mp4abox->samplerate = mp4_bs_read_u16(bs);
	mp4abox->reseved3 = mp4_bs_read_u16(bs);

	mp4abox->struct_size += 28;

	alloc_struct_fullbox_header(&fullboxhead);
	read_fullbox_header(bs, fullboxhead);
	alloc_struct_esds_box(&esdsbox);

	memcpy(esdsbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
	mp4abox->struct_size += read_esds_box(bs, esdsbox);
	mp4abox->esdsbox = esdsbox;

	free_struct_fullbox_header(fullboxhead);

	return mp4abox->struct_size;
}

int write_mp4a_box(mp4_bits_t *bs, mp4a_box_t *mp4abox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	mp4abox->struct_size = write_box_header(bs, mp4abox->boxheader);

	mp4_bs_write_u8(bs, mp4abox->reseved[0]);
	mp4_bs_write_u8(bs, mp4abox->reseved[1]);
	mp4_bs_write_u8(bs, mp4abox->reseved[2]);
	mp4_bs_write_u8(bs, mp4abox->reseved[2]);
	mp4_bs_write_u8(bs, mp4abox->reseved[4]);
	mp4_bs_write_u8(bs, mp4abox->reseved[5]);
	mp4_bs_write_u16(bs, mp4abox->data_reference_index);
	mp4_bs_write_u32(bs, mp4abox->reseved1[0]);
	mp4_bs_write_u32(bs, mp4abox->reseved1[1]);

	mp4_bs_write_u16(bs, mp4abox->chanlecount);
	mp4_bs_write_u16(bs, mp4abox->samplesize);
	mp4_bs_write_u32(bs, mp4abox->reseved2);
	mp4_bs_write_u16(bs, mp4abox->samplerate);
	mp4_bs_write_u16(bs, mp4abox->reseved3);

	mp4abox->struct_size += 28;

	if (mp4abox->esdsbox) {
		mp4abox->struct_size += write_esds_box(bs, mp4abox->esdsbox);
	}

	mp4abox->boxheader->size = mp4abox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, mp4abox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return mp4abox->struct_size;
}

int alloc_struct_mp4a_box(mp4a_box_t **mp4abox)
{
	mp4a_box_t *mp4abox_t = *mp4abox;

	if ((mp4abox_t = (mp4a_box_t *)calloc(1, sizeof(mp4a_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(mp4abox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*mp4abox = mp4abox_t;
	return 1;
}

int free_struct_mp4a_box(mp4a_box_t *mp4abox)
{
	if (mp4abox) {
		if (mp4abox->boxheader) {
			free_struct_box_header(mp4abox->boxheader);
		}
		free(mp4abox);
		mp4abox = NULL;
	}

	return 1;
}
