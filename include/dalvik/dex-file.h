#ifndef JATO__DALVIK_DEX_FILE_H
#define JATO__DALVIK_DEX_FILE_H

#include <stdint.h>

/*
 * http://www.netmite.com/android/mydroid/dalvik/docs/dex-format.html
 */

extern uint8_t DEX_FILE_MAGIC[8];

#define DEX_ENDIAN_CONSTANT 0x12345678UL

struct dex_class_def {
	uint32_t		dx_class_idx;
	uint32_t		dx_access_flags;
	uint32_t		dx_superclass_idx;
	uint32_t		dx_interfaces_off;
	uint32_t		dx_source_file_idx;
	uint32_t		dx_annotations_off;
	uint32_t		dx_class_data_off;
	uint32_t		dx_static_values_off;
};

struct dex_file_header {
	uint8_t			dx_magic[8];
	uint32_t		dx_checksum;
	uint8_t			dx_signature[20];
	uint32_t		dx_file_size;
	uint32_t		dx_header_size;
	uint32_t		dx_endian_tag;
	uint32_t		dx_link_size;
	uint32_t		dx_link_off;
	uint32_t		dx_map_off;
	uint32_t		dx_string_ids_size;
	uint32_t		dx_string_ids_off;
	uint32_t		dx_type_ids_size;
	uint32_t		dx_type_ids_off;
	uint32_t		dx_proto_ids_size;
	uint32_t		dx_proto_ids_off;
	uint32_t		dx_field_ids_size;
	uint32_t		dx_field_ids_off;
	uint32_t		dx_method_ids_size;
	uint32_t		dx_method_ids_off;
	uint32_t		dx_class_defs_size;
	uint32_t		dx_class_defs_off;
	uint32_t		dx_data_size;
	uint32_t		dx_data_off;
};

struct dex_file {
	int			dx_fd;
	void			*dx_mmap;
	uint64_t		dx_size;
	struct dex_file_header	*dx_header;
};

struct dex_file *dex_open(const char *filename);
void dex_close(struct dex_file *self);

#endif /* JATO__DALVIK_DEX_FILE_H */
