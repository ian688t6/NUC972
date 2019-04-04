#include "mp4.h"

int read_minf_box(mp4_bits_t *bs, minf_box_t *minfbox)
{
	box_header_t *boxhead;
	fullbox_header_t *fullboxhead;

	alloc_struct_box_header(&boxhead);
	alloc_struct_fullbox_header(&fullboxhead);

	minfbox->struct_size = minfbox->boxheader->struct_size;

	while (minfbox->struct_size < minfbox->boxheader->size) {
		read_box_header(bs, boxhead);

		if (boxhead->type == MP4_BOX_TYPE_vmhd) {
			vmhd_box_t *vmhdbox;

			alloc_struct_vmhd_box(&vmhdbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(vmhdbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			minfbox->struct_size += read_vmhd_box(bs, vmhdbox);
			minfbox->vmhdbox = vmhdbox;
		} else if (boxhead->type == MP4_BOX_TYPE_smhd) {
			smhd_box_t *smhdbox;

			alloc_struct_smhd_box(&smhdbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(smhdbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			minfbox->struct_size += read_smhd_box(bs, smhdbox);
			minfbox->smhdbox = smhdbox;
		} else if (boxhead->type == MP4_BOX_TYPE_hmhd) {
			hmhd_box_t *hmhdbox;

			alloc_struct_hmhd_box(&hmhdbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(hmhdbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			minfbox->struct_size += read_hmhd_box(bs, hmhdbox);
			minfbox->hmhdbox = hmhdbox;
		} else if (boxhead->type == MP4_BOX_TYPE_nmhd) {
			nmhd_box_t *nmhdbox;

			alloc_struct_nmhd_box(&nmhdbox);
			mp4_bs_seek(bs, mp4_bs_get_position(bs) - boxhead->struct_size);
			read_fullbox_header(bs, fullboxhead);
			memcpy(nmhdbox->boxheader, fullboxhead, sizeof(fullbox_header_t));
			minfbox->struct_size += read_nmhd_box(bs, nmhdbox);
			minfbox->nmhdbox = nmhdbox;
		} else if (boxhead->type == MP4_BOX_TYPE_dinf) {
			dinf_box_t *dinfbox;
			alloc_struct_dinf_box(&dinfbox);
			memcpy(dinfbox->boxheader, boxhead, sizeof(box_header_t));
			minfbox->struct_size += read_dinf_box(bs, dinfbox);
			minfbox->dinfbox = dinfbox;
		} else if (boxhead->type == MP4_BOX_TYPE_stbl) {
			stbl_box_t *stblbox;
			alloc_struct_stbl_box(&stblbox);
			memcpy(stblbox->boxheader, boxhead, sizeof(box_header_t));
			minfbox->struct_size += read_stbl_box(bs, stblbox);
			minfbox->stblbox = stblbox;
		} else {
			mp4_bs_skip_bytes(bs, boxhead->size - boxhead->struct_size);
			minfbox->struct_size += boxhead->size;
		}
	}

	free_struct_box_header(boxhead);
	free_struct_fullbox_header(fullboxhead);

	return minfbox->struct_size;
}

int write_minf_box(mp4_bits_t *bs, minf_box_t *minfbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	minfbox->struct_size = write_box_header(bs, minfbox->boxheader);

	if (minfbox->vmhdbox) {
		minfbox->struct_size += write_vmhd_box(bs, minfbox->vmhdbox);
	}
	if (minfbox->smhdbox) {
		minfbox->struct_size += write_smhd_box(bs, minfbox->smhdbox);
	}
	if (minfbox->hmhdbox) {
		minfbox->struct_size += write_hmhd_box(bs, minfbox->hmhdbox);
	}
	if (minfbox->nmhdbox) {
		minfbox->struct_size += write_nmhd_box(bs, minfbox->nmhdbox);
	}
	if (minfbox->dinfbox) {
		minfbox->struct_size += write_dinf_box(bs, minfbox->dinfbox);
	}
	if (minfbox->stblbox) {
		minfbox->struct_size += write_stbl_box(bs, minfbox->stblbox);
	}

	minfbox->boxheader->size = minfbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, minfbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return minfbox->struct_size;
}

int alloc_struct_minf_box(minf_box_t **minfbox)
{
	minf_box_t *minfbox_t = *minfbox;
	if ((minfbox_t = (minf_box_t *)calloc(1, sizeof(minf_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(minfbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*minfbox = minfbox_t;
	return 1;
}

int free_struct_Minf_box(minf_box_t *minfbox)
{
	if (minfbox) {
		if (minfbox->boxheader) {
			free_struct_box_header(minfbox->boxheader);
		}
		free(minfbox);
		minfbox = NULL;
	}
	return 1;
}
