#include "mp4.h"

int read_trak_box(mp4_bits_t *bs, trak_box_t *trakbox)
{
	box_header_t *boxhead;
	fullbox_header_t *fullboxhead;

	alloc_struct_box_header(&boxhead);
	alloc_struct_fullbox_header(&fullboxhead);

	trakbox->struct_size = trakbox->boxheader->struct_size;

	while (trakbox->struct_size < trakbox->boxheader->size) {
		read_box_header(bs, boxhead);

		if (boxhead->type == MP4_BOX_TYPE_tkhd) {
			tkhd_box_t *tkhdbox;

			alloc_struct_tkhd_box(&tkhdbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(tkhdbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			trakbox->struct_size += read_tkhd_box(bs, tkhdbox);

			trakbox->tkhdbox = tkhdbox;
		} else if (boxhead->type == MP4_BOX_TYPE_mdia) {
			mdia_box_t *mdiabox;

			alloc_struct_mdia_box(&mdiabox);
			memcpy(mdiabox->boxheader, boxhead, sizeof(box_header_t));
			trakbox->struct_size += read_mdia_box(bs, mdiabox);
			trakbox->mdiabox = mdiabox;
		} else {
			mp4_bs_skip_bytes(bs, boxhead->size - boxhead->struct_size);
			trakbox->struct_size += boxhead->size;
		}
	}

	free_struct_box_header(boxhead);
	free_struct_fullbox_header(fullboxhead);

	return trakbox->struct_size;
}

int write_trak_box(mp4_bits_t *bs, trak_box_t * trakbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	trakbox->struct_size = write_box_header(bs, trakbox->boxheader);

	if (trakbox->tkhdbox) {
		trakbox->struct_size += write_tkhd_box(bs, trakbox->tkhdbox);
	}
	if (trakbox->mdiabox) {
		trakbox->struct_size += write_mdia_box(bs, trakbox->mdiabox);
	}

	trakbox->boxheader->size = trakbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, trakbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return trakbox->struct_size;
}

int alloc_struct_trak_box(trak_box_t **trakbox)
{
	trak_box_t * trakbox_t = *trakbox;

	if ((trakbox_t = (trak_box_t *)calloc(1, sizeof(trak_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(trakbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}

	*trakbox = trakbox_t;
	return 1;
}

int free_struct_trak_box(trak_box_t *trakbox)
{
	if (trakbox) {
		if (trakbox->boxheader) {
			free_struct_box_header(trakbox->boxheader);
		}
		free(trakbox);
		trakbox = NULL;
	}
	return 1;
}
