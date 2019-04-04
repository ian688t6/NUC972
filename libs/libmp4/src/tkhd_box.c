#include "mp4.h"

int read_tkhd_box(mp4_bits_t *bs, tkhd_box_t *tkhdbox)
{
	tkhdbox->struct_size = tkhdbox->boxheader->struct_size;

	if (tkhdbox->boxheader->version == 1) {
		tkhdbox->creation_time = mp4_bs_read_u64(bs);
		tkhdbox->modification_time = mp4_bs_read_u64(bs);
		tkhdbox->track_ID = mp4_bs_read_u32(bs);
		tkhdbox->reserved = mp4_bs_read_u32(bs);
		tkhdbox->duration = mp4_bs_read_u64(bs);
		tkhdbox->struct_size += 32;
	} else {
		tkhdbox->creation_time = mp4_bs_read_u32(bs);
		tkhdbox->modification_time = mp4_bs_read_u32(bs);
		tkhdbox->track_ID = mp4_bs_read_u32(bs);
		tkhdbox->reserved = mp4_bs_read_u32(bs);
		tkhdbox->duration = mp4_bs_read_u32(bs);
		tkhdbox->struct_size += 20;
	}

	tkhdbox->reserved_1[0] = mp4_bs_read_u32(bs);
	tkhdbox->reserved_1[1] = mp4_bs_read_u32(bs);
	tkhdbox->layer = mp4_bs_read_u16(bs);
	tkhdbox->alternate_group = mp4_bs_read_u16(bs);
	tkhdbox->volume = mp4_bs_read_u16(bs);
	tkhdbox->reserved_2 = mp4_bs_read_u16(bs);
	tkhdbox->struct_size += 16;

	tkhdbox->matrix[0] = mp4_bs_read_u32(bs);
	tkhdbox->matrix[1] = mp4_bs_read_u32(bs);
	tkhdbox->matrix[2] = mp4_bs_read_u32(bs);
	tkhdbox->matrix[3] = mp4_bs_read_u32(bs);
	tkhdbox->matrix[4] = mp4_bs_read_u32(bs);
	tkhdbox->matrix[5] = mp4_bs_read_u32(bs);
	tkhdbox->matrix[6] = mp4_bs_read_u32(bs);
	tkhdbox->matrix[7] = mp4_bs_read_u32(bs);
	tkhdbox->matrix[8] = mp4_bs_read_u32(bs);
	tkhdbox->struct_size += 36;

	tkhdbox->width = mp4_bs_read_u32(bs);
	tkhdbox->height = mp4_bs_read_u32(bs);
	tkhdbox->struct_size += 8;

	return tkhdbox->struct_size;
}

int write_tkhd_box(mp4_bits_t *bs, tkhd_box_t *tkhdbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	tkhdbox->struct_size = write_fullbox_header(bs, tkhdbox->boxheader);

	if (tkhdbox->boxheader->version == 1) {
		mp4_bs_write_u64(bs, tkhdbox->creation_time);
		mp4_bs_write_u64(bs, tkhdbox->modification_time);
		mp4_bs_write_u32(bs, tkhdbox->track_ID);
		mp4_bs_write_u32(bs, tkhdbox->reserved);
		mp4_bs_write_u64(bs, tkhdbox->duration);
		tkhdbox->struct_size += 32;
	} else {
		mp4_bs_write_u32(bs, tkhdbox->creation_time);
		mp4_bs_write_u32(bs, tkhdbox->modification_time);
		mp4_bs_write_u32(bs, tkhdbox->track_ID);
		mp4_bs_write_u32(bs, tkhdbox->reserved);
		mp4_bs_write_u32(bs, tkhdbox->duration);
		tkhdbox->struct_size += 20;
	}

	mp4_bs_write_u32(bs, tkhdbox->reserved_1[0]);
	mp4_bs_write_u32(bs, tkhdbox->reserved_1[1]);
	mp4_bs_write_u16(bs, tkhdbox->layer);
	mp4_bs_write_u16(bs, tkhdbox->alternate_group);
	mp4_bs_write_u16(bs, tkhdbox->volume);
	mp4_bs_write_u16(bs, tkhdbox->reserved_2);
	tkhdbox->struct_size += 16;

	mp4_bs_write_u32(bs, tkhdbox->matrix[0]);
	mp4_bs_write_u32(bs, tkhdbox->matrix[1]);
	mp4_bs_write_u32(bs, tkhdbox->matrix[2]);
	mp4_bs_write_u32(bs, tkhdbox->matrix[3]);
	mp4_bs_write_u32(bs, tkhdbox->matrix[4]);
	mp4_bs_write_u32(bs, tkhdbox->matrix[5]);
	mp4_bs_write_u32(bs, tkhdbox->matrix[6]);
	mp4_bs_write_u32(bs, tkhdbox->matrix[7]);
	mp4_bs_write_u32(bs, tkhdbox->matrix[8]);
	tkhdbox->struct_size += 36;

	mp4_bs_write_u32(bs, tkhdbox->width);
	mp4_bs_write_u32(bs, tkhdbox->height);
	tkhdbox->struct_size += 8;

	tkhdbox->boxheader->size = tkhdbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, tkhdbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return tkhdbox->struct_size;
}

int alloc_struct_tkhd_box(tkhd_box_t ** tkhdbox)
{
	tkhd_box_t * tkhdbox_t = *tkhdbox;
	if ((tkhdbox_t = (tkhd_box_t *)calloc(1, sizeof(tkhd_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(tkhdbox_t->boxheader));
	*tkhdbox = tkhdbox_t;
	return 1;
}

int free_struct_tkhd_box(tkhd_box_t * tkhdbox)
{
	if (tkhdbox) {
		free_struct_fullbox_header(tkhdbox->boxheader);
		free(tkhdbox);
		tkhdbox = NULL;
	}
	return 1;
}
