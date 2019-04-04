#include "mp4.h"

int read_moov_box(mp4_bits_t *bs, moov_box_t *moovbox)
{
	box_header_t *boxhead;
	fullbox_header_t *fullboxhead;

	alloc_struct_box_header(&boxhead);
	alloc_struct_fullbox_header(&fullboxhead);

	moovbox->struct_size = moovbox->boxheader->struct_size;

	while (moovbox->struct_size < moovbox->boxheader->size) {
		read_box_header(bs, boxhead);

		if (boxhead->type == MP4_BOX_TYPE_mvhd) {
			mvhd_box_t *mvhdbox;

			alloc_struct_mvhd_box(&mvhdbox);

			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(mvhdbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			moovbox->struct_size += read_mvhd_box(bs, mvhdbox);

			moovbox->mvhdbox = mvhdbox;
		} else if (boxhead->type == MP4_BOX_TYPE_trak) {
			trak_box_t *trakbox;

			alloc_struct_trak_box(&trakbox);
			memcpy(trakbox->boxheader, boxhead, sizeof(box_header_t));
			moovbox->struct_size += read_trak_box(bs, trakbox);
			if (trakbox->mdiabox->hdlrbox->handler_type == MP4_FOURCC('s', 'o', 'u', 'n')) {
				moovbox->soun_trakbox = trakbox;
			}
			if (trakbox->mdiabox->hdlrbox->handler_type == MP4_FOURCC('v', 'i', 'd', 'e')) {
				moovbox->vide_trakbox = trakbox;
			}
		} else {
			mp4_bs_skip_bytes(bs, boxhead->size - boxhead->struct_size);
			moovbox->struct_size += boxhead->size;
		}
	}

	free_struct_box_header(boxhead);
	free_struct_fullbox_header(fullboxhead);

	return moovbox->struct_size;
}

int write_moov_box(mp4_bits_t *bs, moov_box_t *moovbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	moovbox->struct_size = write_box_header(bs, moovbox->boxheader);

	if (moovbox->mvhdbox) {
		moovbox->struct_size += write_mvhd_box(bs, moovbox->mvhdbox);
	}
	if (moovbox->vide_trakbox) {
		moovbox->struct_size += write_trak_box(bs, moovbox->vide_trakbox);
	}
	if (moovbox->soun_trakbox) {
		moovbox->struct_size += write_trak_box(bs, moovbox->soun_trakbox);
	}

	moovbox->boxheader->size = moovbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, moovbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return moovbox->struct_size;
}

int alloc_struct_moov_box(moov_box_t **moovbox)
{
	moov_box_t *moovbox_t = *moovbox;
	if ((moovbox_t = (moov_box_t *)calloc(1, sizeof(moov_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(moovbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*moovbox = moovbox_t;
	return 1;
}

int free_struct_moov_box(moov_box_t *moovbox)
{
	if (moovbox) {
		if (moovbox->boxheader) {
			free_struct_box_header(moovbox->boxheader);
		}

		free(moovbox);
		moovbox = NULL;
	}
	return 1;
}
