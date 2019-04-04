#ifndef __MINF_BOX_H__
#define __MINF_BOX_H__

typedef struct tag_minf_box {
	box_header_t *boxheader;
	vmhd_box_t *vmhdbox;
	smhd_box_t *smhdbox;
	hmhd_box_t *hmhdbox;
	nmhd_box_t *nmhdbox;
	dinf_box_t *dinfbox;
	stbl_box_t *stblbox;

	uint32_t struct_size;
} minf_box_t;

int read_minf_box(mp4_bits_t *bs, minf_box_t *minfbox);
int write_minf_box(mp4_bits_t *bs, minf_box_t *minfbox);
int alloc_struct_minf_box(minf_box_t **minfbox);
int free_struct_Minf_box(minf_box_t *minfbox);

#endif
