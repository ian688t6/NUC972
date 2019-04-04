#ifndef __BOX_H__
#define __BOX_H__

/* Big Endian */
#define MP4_FOURCC( a, b, c, d ) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

typedef enum {
	/* 14496-12 */
	MP4_BOX_TYPE_co64 = MP4_FOURCC('c', 'o', '6', '4'),
	MP4_BOX_TYPE_cprt = MP4_FOURCC('c', 'p', 'r', 't'),
	MP4_BOX_TYPE_ctts = MP4_FOURCC('c', 't', 't', 's'),

	MP4_BOX_TYPE_dinf = MP4_FOURCC('d', 'i', 'n', 'f'),
	MP4_BOX_TYPE_dref = MP4_FOURCC('d', 'r', 'e', 'f'),

	MP4_BOX_TYPE_edts = MP4_FOURCC('e', 'd', 't', 's'),
	MP4_BOX_TYPE_elst = MP4_FOURCC('e', 'l', 's', 't'),

	MP4_BOX_TYPE_free = MP4_FOURCC('f', 'r', 'e', 'e'),
	MP4_BOX_TYPE_ftyp = MP4_FOURCC('f', 't', 'y', 'p'),

	MP4_BOX_TYPE_hdlr = MP4_FOURCC('h', 'd', 'l', 'r'),
	MP4_BOX_TYPE_hmhd = MP4_FOURCC('h', 'm', 'h', 'd'),

	MP4_BOX_TYPE_mdat = MP4_FOURCC('m', 'd', 'a', 't'),
	MP4_BOX_TYPE_mdhd = MP4_FOURCC('m', 'd', 'h', 'd'),
	MP4_BOX_TYPE_mdia = MP4_FOURCC('m', 'd', 'i', 'a'),
	MP4_BOX_TYPE_mehd = MP4_FOURCC('m', 'e', 'h', 'd'),
	MP4_BOX_TYPE_meta = MP4_FOURCC('m', 'e', 't', 'a'),
	MP4_BOX_TYPE_minf = MP4_FOURCC('m', 'i', 'n', 'f'),
	MP4_BOX_TYPE_moov = MP4_FOURCC('m', 'o', 'o', 'v'),
	MP4_BOX_TYPE_mvex = MP4_FOURCC('m', 'v', 'e', 'x'),
	MP4_BOX_TYPE_mvhd = MP4_FOURCC('m', 'v', 'h', 'd'),

	MP4_BOX_TYPE_nmhd = MP4_FOURCC('n', 'm', 'h', 'd'),

	MP4_BOX_TYPE_padb = MP4_FOURCC('p', 'a', 'd', 'b'),

	MP4_BOX_TYPE_rtp = MP4_FOURCC('r', 't', 'p', ' '),

	MP4_BOX_TYPE_sbpg = MP4_FOURCC('s', 'b', 'p', 'g'),
	MP4_BOX_TYPE_sdtp = MP4_FOURCC('s', 'd', 't', 'p'),
	MP4_BOX_TYPE_sgpd = MP4_FOURCC('s', 'g', 'p', 'd'),
	MP4_BOX_TYPE_smhd = MP4_FOURCC('s', 'm', 'h', 'd'),
	MP4_BOX_TYPE_skip = MP4_FOURCC('s', 'k', 'i', 'p'),
	MP4_BOX_TYPE_stbl = MP4_FOURCC('s', 't', 'b', 'l'),
	MP4_BOX_TYPE_stco = MP4_FOURCC('s', 't', 'c', 'o'),
	MP4_BOX_TYPE_stdp = MP4_FOURCC('s', 't', 'd', 'p'),
	MP4_BOX_TYPE_stsc = MP4_FOURCC('s', 't', 's', 'c'),
	MP4_BOX_TYPE_stsd = MP4_FOURCC('s', 't', 's', 'd'),
	MP4_BOX_TYPE_stsh = MP4_FOURCC('s', 't', 's', 'h'),
	MP4_BOX_TYPE_stss = MP4_FOURCC('s', 't', 's', 's'),
	MP4_BOX_TYPE_stsz = MP4_FOURCC('s', 't', 's', 'z'),
	MP4_BOX_TYPE_stz2 = MP4_FOURCC('s', 't', 'z', '2'),
	MP4_BOX_TYPE_stts = MP4_FOURCC('s', 't', 't', 's'),
	MP4_BOX_TYPE_snro = MP4_FOURCC('s', 'n', 'r', 'o'),
	MP4_BOX_TYPE_subs = MP4_FOURCC('s', 'u', 'b', 's'),

	MP4_BOX_TYPE_tims = MP4_FOURCC('t', 'i', 'm', 's'),
	MP4_BOX_TYPE_tkhd = MP4_FOURCC('t', 'k', 'h', 'd'),
	MP4_BOX_TYPE_trex = MP4_FOURCC('t', 'r', 'e', 'x'),
	MP4_BOX_TYPE_tref = MP4_FOURCC('t', 'r', 'e', 'f'),
	MP4_BOX_TYPE_trak = MP4_FOURCC('t', 'r', 'a', 'k'),
	MP4_BOX_TYPE_tsro = MP4_FOURCC('t', 's', 'r', 'o'),

	MP4_BOX_TYPE_udta = MP4_FOURCC('u', 'd', 't', 'a'),
	MP4_BOX_TYPE_url = MP4_FOURCC('u', 'r', 'l', ' '),
	MP4_BOX_TYPE_urn = MP4_FOURCC('u', 'r', 'n', ' '),
	MP4_BOX_TYPE_uuid = MP4_FOURCC('u', 'u', 'i', 'd'),

	MP4_BOX_TYPE_vmhd = MP4_FOURCC('v', 'm', 'h', 'd'),

	MP4_BOX_TYPE_sowt = MP4_FOURCC('s', 'o', 'w', 't'),

	/* 14496-14 */
	MP4_BOX_TYPE_esds = MP4_FOURCC('e', 's', 'd', 's'),
	MP4_BOX_TYPE_iods = MP4_FOURCC('i', 'o', 'd', 's'),

	MP4_BOX_TYPE_mp4a = MP4_FOURCC('m', 'p', '4', 'a'),
	MP4_BOX_TYPE_mp4s = MP4_FOURCC('m', 'p', '4', 's'),
	MP4_BOX_TYPE_mp4v = MP4_FOURCC('m', 'p', '4', 'v'),

	/* 14496-15 */
	MP4_BOX_TYPE_avc1 = MP4_FOURCC('a', 'v', 'c', '1'),
	MP4_BOX_TYPE_avcC = MP4_FOURCC('a', 'v', 'c', 'C'),
	MP4_BOX_TYPE_avcp = MP4_FOURCC('a', 'v', 'c', 'p'),
	MP4_BOX_TYPE_btrt = MP4_FOURCC('b', 't', 'r', 't'),
	MP4_BOX_TYPE_m4ds = MP4_FOURCC('m', '4', 'd', 's'),

	/* todo */
	MP4_BOX_TYPE_todo = MP4_FOURCC('t', 'o', 'd', 'o')
} MP4_BOX_TYPE;

typedef struct tag_box_header {
	uint32_t size;
	uint32_t type;
	uint64_t largesize;           //(size==1)  //(size==0) box extends to end of file
	uint8_t usertype[16];
	uint32_t struct_size;
} box_header_t;

typedef struct tag_fullbox_header {
	uint32_t size;
	uint32_t type;
	uint64_t largesize;           //(size==1)  //(size==0) box extends to end of file
	uint8_t usertype[16];
	uint8_t version;
	uint32_t flags;

	uint32_t struct_size;
} fullbox_header_t;

typedef struct tag_box {
	uint8_t *buf;
	uint32_t size;
} box_t;

#define BOX_HEDER_SIZE(head)		(head->size == 0 ? 8 : )

int read_box_header(struct mp4_bits *bs, box_header_t *boxheader);
int read_fullbox_header(struct mp4_bits *bs, fullbox_header_t *boxheader);

int write_box_header(mp4_bits_t *bs, box_header_t *boxheader);
int write_fullbox_header(mp4_bits_t *bs, fullbox_header_t *boxheader);

int alloc_struct_box_header(box_header_t ** boxheader);
int alloc_struct_fullbox_header(fullbox_header_t ** fullboxheader);
int free_struct_box_header(box_header_t * boxheader);
int free_struct_fullbox_header(fullbox_header_t * fullboxheader);
int alloc_struct_box(box_t ** box);
int free_struct_box(box_t * box);

#endif
