#include "mp4.h"

int read_mdhd_box(mp4_bits_t *bs, mdhd_box_t *mdhdbox)
{
	mdhdbox->struct_size = mdhdbox->boxheader->struct_size;
	if (mdhdbox->boxheader->version == 1) {
		mdhdbox->creation_time = mp4_bs_read_u64(bs);
		mdhdbox->modification_time = mp4_bs_read_u64(bs);
		mdhdbox->timescale = mp4_bs_read_u32(bs);
		mdhdbox->duration = mp4_bs_read_u64(bs);
		mdhdbox->struct_size += 28;
	} else {
		mdhdbox->creation_time = mp4_bs_read_u32(bs);
		mdhdbox->modification_time = mp4_bs_read_u32(bs);
		mdhdbox->timescale = mp4_bs_read_u32(bs);
		mdhdbox->duration = mp4_bs_read_u32(bs);
		mdhdbox->struct_size += 16;
	}

	mdhdbox->language = mp4_bs_read_u16(bs) & 0x7fff;
	mdhdbox->pre_defined = mp4_bs_read_u16(bs);
	mdhdbox->struct_size += 4;

	return mdhdbox->struct_size;
}

int write_mdhd_box(mp4_bits_t *bs, mdhd_box_t *mdhdbox)
{
	uint64_t stcobox_pos, tail_pos;

	stcobox_pos = mp4_bs_get_position(bs);

	mdhdbox->struct_size = write_fullbox_header(bs, mdhdbox->boxheader);

	if (mdhdbox->boxheader->version == 1) {
		mp4_bs_write_u64(bs, mdhdbox->creation_time);
		mp4_bs_write_u64(bs, mdhdbox->modification_time);
		mp4_bs_write_u32(bs, mdhdbox->timescale);
		mp4_bs_write_u64(bs, mdhdbox->duration);
		mdhdbox->struct_size += 28;
	} else {
		mp4_bs_write_u32(bs, mdhdbox->creation_time);
		mp4_bs_write_u32(bs, mdhdbox->modification_time);
		mp4_bs_write_u32(bs, mdhdbox->timescale);
		mp4_bs_write_u32(bs, mdhdbox->duration);
		mdhdbox->struct_size += 16;
	}

	mp4_bs_write_u16(bs, mdhdbox->language & 0x7fff);
	mp4_bs_write_u16(bs, mdhdbox->pre_defined);
	mdhdbox->struct_size += 4;

	mdhdbox->boxheader->size = mdhdbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, stcobox_pos);
	mp4_bs_write_u32(bs, mdhdbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return mdhdbox->struct_size;
}

int alloc_struct_mdhd_box(mdhd_box_t **mdhdbox)
{
	mdhd_box_t * mdhdbox_t = *mdhdbox;
	if ((mdhdbox_t = (mdhd_box_t *)calloc(1, sizeof(mdhd_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(mdhdbox_t->boxheader));
	*mdhdbox = mdhdbox_t;
	return 1;
}

int free_struct_mdhd_box(mdhd_box_t *mdhdbox)
{
	if (mdhdbox) {
		free_struct_fullbox_header(mdhdbox->boxheader);
		free(mdhdbox);
		mdhdbox = NULL;
	}
	return 1;
}
