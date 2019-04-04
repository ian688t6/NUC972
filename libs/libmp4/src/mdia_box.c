#include "mp4.h"

int read_mdia_box(mp4_bits_t *bs, mdia_box_t *mdiabox)
{
	box_header_t *boxhead;
	fullbox_header_t *fullboxhead;

	alloc_struct_box_header(&boxhead);
	alloc_struct_fullbox_header(&fullboxhead);

	mdiabox->struct_size = mdiabox->boxheader->struct_size;

	while (mdiabox->struct_size < mdiabox->boxheader->size) {
		read_box_header(bs, boxhead);

		if (boxhead->type == MP4_BOX_TYPE_mdhd) {
			mdhd_box_t *mdhdbox;

			alloc_struct_mdhd_box(&mdhdbox);

			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(mdhdbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			mdiabox->struct_size += read_mdhd_box(bs, mdhdbox);

			mdiabox->mdhdbox = mdhdbox;
		} else if (boxhead->type == MP4_BOX_TYPE_hdlr) {
			hdlr_box_t *hdlrbox;

			alloc_struct_hdlr_box(&hdlrbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(hdlrbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			mdiabox->struct_size += read_hdlr_box(bs, hdlrbox);

			mdiabox->hdlrbox = hdlrbox;
		} else if (boxhead->type == MP4_BOX_TYPE_minf) {
			minf_box_t *minfbox;
			alloc_struct_minf_box(&minfbox);
			memcpy(minfbox->boxheader, boxhead, sizeof(box_header_t));
			mdiabox->struct_size += read_minf_box(bs, minfbox);
			mdiabox->minfbox = minfbox;
		} else {
			mp4_bs_skip_bytes(bs, boxhead->size - boxhead->struct_size);
			mdiabox->struct_size += boxhead->size;
		}
	}

	free_struct_box_header(boxhead);
	free_struct_fullbox_header(fullboxhead);

	return mdiabox->struct_size;
}

int write_mdia_box(mp4_bits_t *bs, mdia_box_t *mdiabox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	mdiabox->struct_size = write_box_header(bs, mdiabox->boxheader);

	if (mdiabox->mdhdbox) {
		mdiabox->struct_size += write_mdhd_box(bs, mdiabox->mdhdbox);
	}
	if (mdiabox->hdlrbox) {
		mdiabox->struct_size += write_hdlr_box(bs, mdiabox->hdlrbox);
	}
	if (mdiabox->minfbox) {
		mdiabox->struct_size += write_minf_box(bs, mdiabox->minfbox);
	}

	mdiabox->boxheader->size = mdiabox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, mdiabox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return mdiabox->struct_size;
}

int alloc_struct_mdia_box(mdia_box_t **mdiabox)
{
	mdia_box_t * mdiabox_t = *mdiabox;

	if ((mdiabox_t = (mdia_box_t *)calloc(1, sizeof(mdia_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(mdiabox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*mdiabox = mdiabox_t;
	return 1;
}

int free_struct_mdia_box(mdia_box_t *mdiabox)
{
	if (mdiabox) {
		if (mdiabox->boxheader) {
			free_struct_box_header(mdiabox->boxheader);
		}
		free(mdiabox);
		mdiabox = NULL;
	}
	return 1;
}
