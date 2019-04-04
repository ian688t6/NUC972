#include "mp4.h"

int read_mvhd_box(mp4_bits_t *bs, mvhd_box_t *mvhdbox)
{
	mvhdbox->struct_size = mvhdbox->boxheader->struct_size;
	if (mvhdbox->boxheader->version == 1) {
		mvhdbox->creation_time = mp4_bs_read_u64(bs);
		mvhdbox->modification_time = mp4_bs_read_u64(bs);
		mvhdbox->timescale = mp4_bs_read_u32(bs);
		mvhdbox->duration = mp4_bs_read_u64(bs);
		mvhdbox->struct_size += 28;
	} else {
		mvhdbox->creation_time = mp4_bs_read_u32(bs);
		mvhdbox->modification_time = mp4_bs_read_u32(bs);
		mvhdbox->timescale = mp4_bs_read_u32(bs);
		mvhdbox->duration = mp4_bs_read_u32(bs);
		mvhdbox->struct_size += 16;
	}

	mvhdbox->rate = mp4_bs_read_u32(bs);
	mvhdbox->volume = mp4_bs_read_u16(bs);
	mvhdbox->reserved = mp4_bs_read_u16(bs);
	mvhdbox->reserved_1[0] = mp4_bs_read_u32(bs);
	mvhdbox->reserved_1[1] = mp4_bs_read_u32(bs);
	mvhdbox->struct_size += 16;

	mvhdbox->matrix[0] = mp4_bs_read_u32(bs);
	mvhdbox->matrix[1] = mp4_bs_read_u32(bs);
	mvhdbox->matrix[2] = mp4_bs_read_u32(bs);
	mvhdbox->matrix[3] = mp4_bs_read_u32(bs);
	mvhdbox->matrix[4] = mp4_bs_read_u32(bs);
	mvhdbox->matrix[5] = mp4_bs_read_u32(bs);
	mvhdbox->matrix[6] = mp4_bs_read_u32(bs);
	mvhdbox->matrix[7] = mp4_bs_read_u32(bs);
	mvhdbox->matrix[8] = mp4_bs_read_u32(bs);
	mvhdbox->struct_size += 36;

	mvhdbox->pre_defined[0] = mp4_bs_read_u32(bs);
	mvhdbox->pre_defined[1] = mp4_bs_read_u32(bs);
	mvhdbox->pre_defined[2] = mp4_bs_read_u32(bs);
	mvhdbox->pre_defined[3] = mp4_bs_read_u32(bs);
	mvhdbox->pre_defined[4] = mp4_bs_read_u32(bs);
	mvhdbox->pre_defined[5] = mp4_bs_read_u32(bs);

	mvhdbox->next_track_ID = mp4_bs_read_u32(bs);
	mvhdbox->struct_size += 28;

	return mvhdbox->struct_size;
}

int write_mvhd_box(mp4_bits_t *bs, mvhd_box_t *mvhdbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	mvhdbox->struct_size = write_fullbox_header(bs, mvhdbox->boxheader);

	if (mvhdbox->boxheader->version == 1) {
		mp4_bs_write_u64(bs, mvhdbox->creation_time);
		mp4_bs_write_u64(bs, mvhdbox->modification_time);
		mp4_bs_write_u32(bs, mvhdbox->timescale);
		mp4_bs_write_u64(bs, mvhdbox->duration);
		mvhdbox->struct_size += 28;
	} else {
		mp4_bs_write_u32(bs, mvhdbox->creation_time);
		mp4_bs_write_u32(bs, mvhdbox->modification_time);
		mp4_bs_write_u32(bs, mvhdbox->timescale);
		mp4_bs_write_u32(bs, mvhdbox->duration);
		mvhdbox->struct_size += 16;
	}

	mp4_bs_write_u32(bs, mvhdbox->rate);
	mp4_bs_write_u16(bs, mvhdbox->volume);
	mp4_bs_write_u16(bs, mvhdbox->reserved);
	mp4_bs_write_u32(bs, mvhdbox->reserved_1[0]);
	mp4_bs_write_u32(bs, mvhdbox->reserved_1[1]);
	mvhdbox->struct_size += 16;

	mp4_bs_write_u32(bs, mvhdbox->matrix[0]);
	mp4_bs_write_u32(bs, mvhdbox->matrix[1]);
	mp4_bs_write_u32(bs, mvhdbox->matrix[2]);
	mp4_bs_write_u32(bs, mvhdbox->matrix[3]);
	mp4_bs_write_u32(bs, mvhdbox->matrix[4]);
	mp4_bs_write_u32(bs, mvhdbox->matrix[5]);
	mp4_bs_write_u32(bs, mvhdbox->matrix[6]);
	mp4_bs_write_u32(bs, mvhdbox->matrix[7]);
	mp4_bs_write_u32(bs, mvhdbox->matrix[8]);
	mvhdbox->struct_size += 36;

	mp4_bs_write_u32(bs, mvhdbox->pre_defined[0]);
	mp4_bs_write_u32(bs, mvhdbox->pre_defined[1]);
	mp4_bs_write_u32(bs, mvhdbox->pre_defined[2]);
	mp4_bs_write_u32(bs, mvhdbox->pre_defined[3]);
	mp4_bs_write_u32(bs, mvhdbox->pre_defined[4]);
	mp4_bs_write_u32(bs, mvhdbox->pre_defined[5]);

	mp4_bs_write_u32(bs, mvhdbox->next_track_ID);
	mvhdbox->struct_size += 28;

	mvhdbox->boxheader->size = mvhdbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, mvhdbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return mvhdbox->struct_size;
}

int alloc_struct_mvhd_box(mvhd_box_t ** mvhdbox)
{
	mvhd_box_t * mvhdbox_t = *mvhdbox;
	if ((mvhdbox_t = (mvhd_box_t *)calloc(1, sizeof(mvhd_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(mvhdbox_t->boxheader));
	*mvhdbox = mvhdbox_t;
	return 1;
}

int free_struct_mvhd_box(mvhd_box_t * mvhdbox)
{
	if (mvhdbox) {
		free_struct_fullbox_header(mvhdbox->boxheader);
		free(mvhdbox);
		mvhdbox = NULL;
	}
	return 1;
}
