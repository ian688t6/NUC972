#include "mp4.h"

int read_stsd_box(mp4_bits_t *bs, stsd_box_t *stsdbox)
{
	box_header_t *boxhead;

	stsdbox->struct_size = stsdbox->boxheader->struct_size;

	stsdbox->entry_count = mp4_bs_read_u32(bs);
	stsdbox->struct_size += 4;

	if (stsdbox->entry_count > 1) {
		mp4_loge("stsd entry_count = %d\n", stsdbox->entry_count);
		return stsdbox->boxheader->size;
	}

	alloc_struct_box_header(&boxhead);
	read_box_header(bs, boxhead);

	if (boxhead->type == MP4_BOX_TYPE_mp4a) {
		mp4a_box_t *mp4abox;
		alloc_struct_mp4a_box(&mp4abox);
		memcpy(mp4abox->boxheader, boxhead, sizeof(box_header_t));
		stsdbox->struct_size += read_mp4a_box(bs, mp4abox);
		stsdbox->mp4abox = mp4abox;
	}
	if (boxhead->type == MP4_BOX_TYPE_avc1) {
		avc1_box_t *avc1box;
		alloc_struct_avc1_box(&avc1box);
		memcpy(avc1box->boxheader, boxhead, sizeof(box_header_t));
		stsdbox->struct_size += read_avc1_box(bs, avc1box);
		stsdbox->avc1box = avc1box;
	}
	if (boxhead->type == MP4_BOX_TYPE_sowt) {
		sowt_box_t *sowtbox;
		alloc_struct_sowt_box(&sowtbox);
		memcpy(sowtbox->boxheader, boxhead, sizeof(box_header_t));
		stsdbox->struct_size += read_sowt_box(bs, sowtbox);
		stsdbox->sowtbox = sowtbox;
	}

	free_struct_box_header(boxhead);
	return stsdbox->struct_size;
}

int write_stsd_box(mp4_bits_t *bs, stsd_box_t *stsdbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	stsdbox->struct_size = write_fullbox_header(bs, stsdbox->boxheader);

	mp4_bs_write_u32(bs, stsdbox->entry_count);
	stsdbox->struct_size += 4;

	if (stsdbox->mp4abox) {
		stsdbox->struct_size += write_mp4a_box(bs, stsdbox->mp4abox);
	}

	if (stsdbox->avc1box) {
		stsdbox->struct_size += write_avc1_box(bs, stsdbox->avc1box);
	}

	if (stsdbox->sowtbox) {
		stsdbox->struct_size += write_sowt_box(bs, stsdbox->sowtbox);
	}

	stsdbox->boxheader->size = stsdbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, stsdbox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return stsdbox->struct_size;
}

int alloc_struct_stsd_box(stsd_box_t **stsdbox)
{
	stsd_box_t * stsdbox_t = *stsdbox;
	if ((stsdbox_t = (stsd_box_t *)calloc(1, sizeof(stsd_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_fullbox_header(&(stsdbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*stsdbox = stsdbox_t;
	return 1;
}

int free_struct_stsd_box(stsd_box_t * stsdbox)
{
	if (stsdbox) {
		if (stsdbox->boxheader) {
			free_struct_fullbox_header(stsdbox->boxheader);
		}
		free(stsdbox);
		stsdbox = NULL;
	}
	return 1;
}
