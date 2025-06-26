/*
 * toa_utils.h - traffic ordering agent shared utility functions header file
 *
 * This is the traffic ordering agent, which takes care of assigning a scheduler
 * and priority level to associated stations.
 *
 * Copyright (C) 2014, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: toa_utils.h 462558 2014-03-18 01:10:07Z $
 *
 */
#if !defined(__toa_utils_h__)
#define __toa_utils_h__

typedef struct toa_attribute {
	struct toa_attribute *next;
	const char *name;
	const char *value;
} toa_attribute;

typedef struct toa_record {
	struct toa_record *next;
	char		*key;
	toa_attribute	*attributes;
} toa_record;

typedef toa_record toa_station;
typedef toa_record toa_ssid;

typedef struct {
	toa_station	*station_settings;	/* per-station settings */
	toa_ssid	*bss_settings;		/* per-BSS settings */
	toa_record	*default_settings;	/* default settings */
} toa_settings;

typedef struct {
	int		id;
	const char	*name;
} toa_value_map;

typedef enum {
	ATTR_ERROR = 0,
	ATTR_TYPE,
	ATTR_PRIO,
	ATTR_NAME
} toa_attribute_id;

typedef enum {
	STA_TYPE_ERROR = 0,
	STA_TYPE_VIDEO,
	STA_TYPE_DATA,
	STA_TYPE_PUBLIC
} toa_station_type;

typedef enum {
	STA_PRIO_ERROR   = 0,
	STA_PRIO_HIGHEST = 1,
	STA_PRIO_HIGHER  = 2,
	STA_PRIO_HIGH    = 3,
	STA_PRIO_LOW     = 13,
	STA_PRIO_DEFAULT = STA_PRIO_LOW,
	STA_PRIO_LOWER   = 14,
	STA_PRIO_LOWEST  = 15
} toa_station_priority;

extern const toa_value_map attribute_map[];
extern const toa_value_map sta_type_map[];
extern const toa_value_map sta_prio_map[];

extern toa_settings *toa_load_settings();
extern int toa_save_settings(toa_settings *);
extern void toa_free_settings(toa_settings *);
extern toa_record *toa_match_record(toa_record **list_head, const char *key, int create);

extern int toa_add_attribute(toa_attribute **headp, char *token);
extern void toa_move_attributes(toa_attribute **dst, toa_attribute **src);
extern toa_attribute *toa_get_attribute_by_id(toa_attribute *head, toa_attribute_id aid);
extern void toa_flush_attributes(toa_attribute **headp);

extern int toa_keyword_to_id(const toa_value_map *map, const char *kw);
extern const char *toa_id_to_keyword(const toa_value_map *map, int id);
extern void toa_log_error(const char *fmtstr, ...);

#endif /* __toa__utils_h__ */
