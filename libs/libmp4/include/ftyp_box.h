#ifndef __FTYP_BOX_H__
#define __FTYP_BOX_H__

typedef struct tag_ftyp_box {
	box_header_t *boxheader;
	uint32_t major_brand;
	uint32_t minor_version;
	uint32_t *compatible_brands;  // to end of the box   
	uint32_t brands;
	uint32_t struct_size;
} ftyp_box_t;

int read_ftyp_box(mp4_bits_t *bs, ftyp_box_t *ftypbox);
int write_ftyp_box(mp4_bits_t *bs, ftyp_box_t * ftypbox);
int alloc_struct_ftyp_box(ftyp_box_t ** ftypbox);
int free_struct_ftyp_box(ftyp_box_t * ftypbox);

#endif
