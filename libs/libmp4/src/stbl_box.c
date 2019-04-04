#include "mp4.h"

int read_stbl_box(mp4_bits_t *bs, stbl_box_t *stblbox)
{
	box_header_t *boxhead;
	fullbox_header_t *fullboxhead;

	alloc_struct_box_header(&boxhead);
	alloc_struct_fullbox_header(&fullboxhead);

	stblbox->struct_size = stblbox->boxheader->struct_size;

	while (stblbox->struct_size < stblbox->boxheader->size) {
		read_box_header(bs, boxhead);

		if (boxhead->type == MP4_BOX_TYPE_stco) {
			stco_box_t *stcobox;

			alloc_struct_stco_box(&stcobox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(stcobox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			stblbox->struct_size += read_stco_box(bs, stcobox);
			stblbox->stcobox = stcobox;
		} else if (boxhead->type == MP4_BOX_TYPE_stsd) {
			stsd_box_t *stsdbox;

			alloc_struct_stsd_box(&stsdbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(stsdbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			stblbox->struct_size += read_stsd_box(bs, stsdbox);
			stblbox->stsdbox = stsdbox;
		} else if (boxhead->type == MP4_BOX_TYPE_stsc) {
			stsc_box_t *stscbox;

			alloc_struct_stsc_box(&stscbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(stscbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			stblbox->struct_size += read_stsc_box(bs, stscbox);
			stblbox->stscbox = stscbox;
		} else if (boxhead->type == MP4_BOX_TYPE_stsz) {
			stsz_box_t *stszbox;

			alloc_struct_stsz_box(&stszbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(stszbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			stblbox->struct_size += read_stsz_box(bs, stszbox);
			stblbox->stszbox = stszbox;
		} else if (boxhead->type == MP4_BOX_TYPE_stts) {
			stts_box_t *sttsbox;

			alloc_struct_stts_box(&sttsbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(sttsbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			stblbox->struct_size += read_stts_box(bs, sttsbox);
			stblbox->sttsbox = sttsbox;
		} else if (boxhead->type == MP4_BOX_TYPE_ctts) {
			ctts_box_t *cttsbox;

			alloc_struct_ctts_box(&cttsbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(cttsbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			stblbox->struct_size += read_ctts_box(bs, cttsbox);
			stblbox->cttsbox = cttsbox;
		} else if (boxhead->type == MP4_BOX_TYPE_stss) {
			stss_box_t *stssbox;

			alloc_struct_stss_box(&stssbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(stssbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			stblbox->struct_size += read_stss_box(bs, stssbox);
			stblbox->stssbox = stssbox;
		} else {
			mp4_bs_skip_bytes(bs, boxhead->size - boxhead->struct_size);
			stblbox->struct_size += boxhead->size;
		}
	}

	free_struct_box_header(boxhead);
	free_struct_fullbox_header(fullboxhead);

	return stblbox->struct_size;
}

int write_stbl_box(mp4_bits_t *bs, stbl_box_t *stblbox)
{
	uint64_t stblbox_pos, tail_pos;

	stblbox_pos = mp4_bs_get_position(bs);

	stblbox->struct_size = write_box_header(bs, stblbox->boxheader);

	if (stblbox->stsdbox) {
		stblbox->struct_size += write_stsd_box(bs, stblbox->stsdbox);
	}
	if (stblbox->cttsbox) {
		stblbox->struct_size += write_ctts_box(bs, stblbox->cttsbox);
	}
	if (stblbox->sttsbox) {
		stblbox->struct_size += write_stts_box(bs, stblbox->sttsbox);
	}
	if (stblbox->stssbox) {
		stblbox->struct_size += write_stss_box(bs, stblbox->stssbox);
	}
	if (stblbox->stscbox) {
		stblbox->struct_size += write_stsc_box(bs, stblbox->stscbox);
	}
	if (stblbox->stszbox) {
		stblbox->struct_size += write_stsz_box(bs, stblbox->stszbox);
	}
	if (stblbox->stcobox) {
		stblbox->struct_size += write_stco_box(bs, stblbox->stcobox);
	}

	stblbox->boxheader->size = stblbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, stblbox_pos);
	mp4_bs_write_u32(bs, stblbox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return stblbox->struct_size;
}

int alloc_struct_stbl_box(stbl_box_t **stblbox)
{
	stbl_box_t *stblbox_t = *stblbox;
	if ((stblbox_t = (stbl_box_t *)calloc(1, sizeof(stbl_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(stblbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*stblbox = stblbox_t;
	return 1;
}

int free_struct_stbl_box(stbl_box_t *stblbox)
{
	if (stblbox) {
		if (stblbox->boxheader) {
			free_struct_box_header(stblbox->boxheader);
		}
		free(stblbox);
		stblbox = NULL;
	}
	return 1;
}
