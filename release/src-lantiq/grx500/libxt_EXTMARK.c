#include <stdbool.h>
#include <stdio.h>
#include <xtables.h>
#include <linux/netfilter/xt_EXTMARK.h>

/* Version 0 */
struct xt_extmark_target_info {
	unsigned long extmark;
};

/* Version 1 */
enum {
	XT_EXTMARK_SET=0,
	XT_EXTMARK_AND,
	XT_EXTMARK_OR,
};

struct xt_extmark_target_info_v1 {
	unsigned long extmark;
	uint8_t mode;
};

enum {
	O_SET_EXTMARK = 0,
	O_AND_EXTMARK,
	O_OR_EXTMARK,
	O_XOR_EXTMARK,
	O_SET_XEXTMARK,
	F_SET_EXTMARK  = 1 << O_SET_EXTMARK,
	F_AND_EXTMARK  = 1 << O_AND_EXTMARK,
	F_OR_EXTMARK   = 1 << O_OR_EXTMARK,
	F_XOR_EXTMARK  = 1 << O_XOR_EXTMARK,
	F_SET_XEXTMARK = 1 << O_SET_XEXTMARK,
	F_ANY       = F_SET_EXTMARK | F_AND_EXTMARK | F_OR_EXTMARK |
	              F_XOR_EXTMARK | F_SET_XEXTMARK,
};

static void EXTMARK_help(void)
{
	printf(
"EXTMARK target options:\n"
"  --set-mark value                   Set extended nfmark value\n"
"  --and-mark value                   Binary AND the extended nfmark with value\n"
"  --or-mark  value                   Binary OR  the extended nfmark with value\n");
}

static const struct xt_option_entry EXTMARK_opts[] = {
	{.name = "set-mark", .id = O_SET_EXTMARK, .type = XTTYPE_UINT32,
	 .excl = F_ANY},
	{.name = "and-mark", .id = O_AND_EXTMARK, .type = XTTYPE_UINT32,
	 .excl = F_ANY},
	{.name = "or-mark", .id = O_OR_EXTMARK, .type = XTTYPE_UINT32,
	 .excl = F_ANY},
	XTOPT_TABLEEND,
};

static const struct xt_option_entry extmark_tg_opts[] = {
	{.name = "set-xmark", .id = O_SET_XEXTMARK, .type = XTTYPE_MARKMASK32,
	 .excl = F_ANY},
	{.name = "set-mark", .id = O_SET_EXTMARK, .type = XTTYPE_MARKMASK32,
	 .excl = F_ANY},
	{.name = "and-mark", .id = O_AND_EXTMARK, .type = XTTYPE_UINT32,
	 .excl = F_ANY},
	{.name = "or-mark", .id = O_OR_EXTMARK, .type = XTTYPE_UINT32,
	 .excl = F_ANY},
	{.name = "xor-mark", .id = O_XOR_EXTMARK, .type = XTTYPE_UINT32,
	 .excl = F_ANY},
	XTOPT_TABLEEND,
};

static void extmark_tg_help(void)
{
	printf(
"EXTMARK target options:\n"
"  --set-xmark value[/mask]  Clear bits in mask and XOR value into nfextmark\n"
"  --set-mark value[/mask]   Clear bits in mask and OR value into nfextmark\n"
"  --and-mark bits           Binary AND the nfextmark with bits\n"
"  --or-mark bits            Binary OR the nfextmark with bits\n"
"  --xor-mask bits           Binary XOR the nfextmark with bits\n"
"\n");
}

static void EXTMARK_parse_v0(struct xt_option_call *cb)
{
	struct xt_extmark_target_info *extmarkinfo = cb->data;

	xtables_option_parse(cb);
	switch (cb->entry->id) {
	case O_SET_EXTMARK:
		extmarkinfo->extmark = cb->val.mark;
		break;
	default:
		xtables_error(PARAMETER_PROBLEM,
			   "EXTMARK target: kernel too old for --%s",
			   cb->entry->name);
	}
}

static void EXTMARK_check(struct xt_fcheck_call *cb)
{
	if (cb->xflags == 0)
		xtables_error(PARAMETER_PROBLEM,
		           "EXTMARK target: Parameter --set/and/or-mark"
			   " is required");
}

static void EXTMARK_parse_v1(struct xt_option_call *cb)
{
	struct xt_extmark_target_info_v1 *extmarkinfo = cb->data;

	xtables_option_parse(cb);
	switch (cb->entry->id) {
	case O_SET_EXTMARK:
	        extmarkinfo->mode = XT_EXTMARK_SET;
		break;
	case O_AND_EXTMARK:
	        extmarkinfo->mode = XT_EXTMARK_AND;
		break;
	case O_OR_EXTMARK:
	        extmarkinfo->mode = XT_EXTMARK_OR;
		break;
	}
	extmarkinfo->extmark = cb->val.u32;
}

static void extmark_tg_parse(struct xt_option_call *cb)
{
	struct xt_extmark_tginfo2 *info = cb->data;

	xtables_option_parse(cb);
	switch (cb->entry->id) {
	case O_SET_XEXTMARK:
		info->extmark = cb->val.mark;
		info->mask = cb->val.mask;
		break;
	case O_SET_EXTMARK:
		info->extmark = cb->val.mark;
		info->mask = cb->val.mark | cb->val.mask;
		break;
	case O_AND_EXTMARK:
		info->extmark = 0;
		info->mask = ~cb->val.u32;
		break;
	case O_OR_EXTMARK:
		info->extmark = info->mask = cb->val.u32;
		break;
	case O_XOR_EXTMARK:
		info->extmark = cb->val.u32;
		info->mask = 0;
		break;
	}
}

static void extmark_tg_check(struct xt_fcheck_call *cb)
{
	if (cb->xflags == 0)
		xtables_error(PARAMETER_PROBLEM, "EXTMARK: One of the --set-xmark, "
		           "--{and,or,xor,set}-mark options is required");
}

static void
print_extmark(unsigned long extmark)
{
	printf(" 0x%lx", extmark);
}

static void EXTMARK_print_v0(const void *ip,
                          const struct xt_entry_target *target, int numeric)
{
	const struct xt_extmark_target_info *extmarkinfo =
		(const struct xt_extmark_target_info *)target->data;
	printf(" EXTMARK set");
	print_extmark(extmarkinfo->extmark);
}

static void EXTMARK_save_v0(const void *ip, const struct xt_entry_target *target)
{
	const struct xt_extmark_target_info *extmarkinfo =
		(const struct xt_extmark_target_info *)target->data;

	printf(" --set-mark");
	print_extmark(extmarkinfo->extmark);
}

static void EXTMARK_print_v1(const void *ip, const struct xt_entry_target *target,
                          int numeric)
{
	const struct xt_extmark_target_info_v1 *extmarkinfo =
		(const struct xt_extmark_target_info_v1 *)target->data;

	switch (extmarkinfo->mode) {
	case XT_EXTMARK_SET:
		printf(" EXTMARK set");
		break;
	case XT_EXTMARK_AND:
		printf(" EXTMARK and");
		break;
	case XT_EXTMARK_OR: 
		printf(" EXTMARK or");
		break;
	}
	print_extmark(extmarkinfo->extmark);
}

static void extmark_tg_print(const void *ip, const struct xt_entry_target *target,
                          int numeric)
{
	const struct xt_extmark_tginfo2 *info = (const void *)target->data;

	if (info->extmark == 0)
		printf(" MARK and 0x%x", (unsigned int)(uint32_t)~info->mask);
	else if (info->extmark == info->mask)
		printf(" MARK or 0x%x", info->extmark);
	else if (info->mask == 0)
		printf(" MARK xor 0x%x", info->extmark);
	else if (info->mask == 0xffffffffU)
		printf(" MARK set 0x%x", info->extmark);
	else
		printf(" EXTMARK xset 0x%x/0x%x", info->extmark, info->mask);
}

static void EXTMARK_save_v1(const void *ip, const struct xt_entry_target *target)
{
	const struct xt_extmark_target_info_v1 *extmarkinfo =
		(const struct xt_extmark_target_info_v1 *)target->data;

	switch (extmarkinfo->mode) {
	case XT_EXTMARK_SET:
		printf(" --set-mark");
		break;
	case XT_EXTMARK_AND:
		printf(" --and-mark");
		break;
	case XT_EXTMARK_OR: 
		printf(" --or-mark");
		break;
	}
	print_extmark(extmarkinfo->extmark);
}

static void extmark_tg_save(const void *ip, const struct xt_entry_target *target)
{
	const struct xt_extmark_tginfo2 *info = (const void *)target->data;

	printf(" --set-xmark 0x%x/0x%x", info->extmark, info->mask);
}

static struct xtables_target extmark_tg_reg[] = {
	{
		.family        = NFPROTO_UNSPEC,
		.name          = "EXTMARK",
		.version       = XTABLES_VERSION,
		.revision      = 0,
		.size          = XT_ALIGN(sizeof(struct xt_extmark_target_info)),
		.userspacesize = XT_ALIGN(sizeof(struct xt_extmark_target_info)),
		.help          = EXTMARK_help,
		.print         = EXTMARK_print_v0,
		.save          = EXTMARK_save_v0,
		.x6_parse      = EXTMARK_parse_v0,
		.x6_fcheck     = EXTMARK_check,
		.x6_options    = EXTMARK_opts,
	},
	{
		.family        = NFPROTO_IPV4,
		.name          = "EXTMARK",
		.version       = XTABLES_VERSION,
		.revision      = 1,
		.size          = XT_ALIGN(sizeof(struct xt_extmark_target_info_v1)),
		.userspacesize = XT_ALIGN(sizeof(struct xt_extmark_target_info_v1)),
		.help          = EXTMARK_help,
		.print         = EXTMARK_print_v1,
		.save          = EXTMARK_save_v1,
		.x6_parse      = EXTMARK_parse_v1,
		.x6_fcheck     = EXTMARK_check,
		.x6_options    = EXTMARK_opts,
	},
	{
		.version       = XTABLES_VERSION,
		.name          = "EXTMARK",
		.revision      = 2,
		.family        = NFPROTO_UNSPEC,
		.size          = XT_ALIGN(sizeof(struct xt_extmark_tginfo2)),
		.userspacesize = XT_ALIGN(sizeof(struct xt_extmark_tginfo2)),
		.help          = extmark_tg_help,
		.print         = extmark_tg_print,
		.save          = extmark_tg_save,
		.x6_parse      = extmark_tg_parse,
		.x6_fcheck     = extmark_tg_check,
		.x6_options    = extmark_tg_opts,
	},
};

void _init(void)
{
	xtables_register_targets(extmark_tg_reg, ARRAY_SIZE(extmark_tg_reg));
}
