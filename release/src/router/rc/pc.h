#ifndef __PC_H__
#define __PC_H__ 1

#define MIN_DAY 0
#define MAX_DAY 6

#define MIN_HOUR 0
#define MAX_HOUR 23

#ifdef RTCONFIG_PC_SCHED_V3
#define MIN_MIN 0
#define MAX_MIN 59
#endif

//#define BLOCKLOCAL

#define iptables_chk_mac "-m mac --mac-source"
#define iptables_chk_ip "-s"
extern char *datestr[];

typedef struct pc_event pc_event_s;
struct pc_event{
	char e_name[32];
#ifdef RTCONFIG_SCHED_V2
	int day_of_week;
	sched_v2_type type;
#endif
	int start_day;
	int end_day;
	int start_hour;
	int end_hour;
	int start_min;
	int end_min;
#ifdef RTCONFIG_PC_REWARD
	int end_year;
	int end_mon;
	int end_sec;
	int access;
	time_t ts;
	int utc;
#endif
	pc_event_s *next;
};

#ifdef RTCONFIG_PC_REWARD
enum {
	PC_REWARD_ACCESS_BLOCKED,
	PC_REWARD_ACCESS_ALLOWED
};
#endif

enum {
	INITIAL,
	NONBLOCK,
	BLOCKED,
	DTIME
};

typedef struct pc pc_s;
struct pc{
	int enabled;
	char state;
	char prev_state;
	int dtimes;
	char device[32];
	char mac[18];
	pc_event_s *events;
	unsigned long long timestamp;
	pc_s *next;
};

#ifdef RTCONFIG_MULTILAN_CFG
#define SDN_PARENTAL_CTRL_BLOCK_ALL			(1 << 0)
#define SDN_PARENTAL_CTRL_BLOCK_INTERNET	(1 << 1)
#define SDN_PARENTAL_CTRL_TIME_SCHED		(1 << 2)
#define SDN_PARENTAL_CTRL_REWARD			(1 << 3)
#define SDN_PARENTAL_CTRL_REDIRECT			(1 << 4)
#ifdef RTCONFIG_ISP_OPTUS
#define SDN_PARENTAL_CTRL_OP_BLOCK_INTERNET	(1 << 5)
#endif
#endif

#ifdef RTCONFIG_SCHED_V2
char *get_pc_date_str(int day_of_week, int over_one_day, char *buf, int buf_size);
pc_event_s *get_event_list_by_sched_v2(pc_event_s **target_list, char *sched_v2_str);
#endif

extern pc_s *get_all_pc_list(pc_s **pc_list);
extern pc_s *op_get_all_pc_list(pc_s **pc_list);
extern pc_s *get_all_pc_tmp_list(pc_s **pc_list);
extern pc_s *get_all_pc_reward_list(pc_s **pc_list);

extern pc_event_s *initial_event(pc_event_s **target_e);
extern void free_event_list(pc_event_s **target_list);
extern pc_event_s *cp_event(pc_event_s **dest, const pc_event_s *src);
extern void print_event_list(pc_event_s *e_list);

extern pc_s *initial_pc(pc_s **target_pc);
extern void free_pc_list(pc_s **target_list);
extern pc_s *cp_pc(pc_s **dest, const pc_s *src);
extern int is_same_pc_list(pc_s *pc_list1, pc_s *pc_list2);
extern void print_pc_list(pc_s *pc_list);

extern pc_s *match_enabled_pc_list(pc_s *pc_list, pc_s **target_list, int enabled);
#ifdef RTCONFIG_PC_SCHED_V3
extern pc_s *match_daytime_pc_list(pc_s *pc_list, pc_s **target_list, int target_day, int target_hour, int target_min);
#else
extern pc_s *match_daytime_pc_list(pc_s *pc_list, pc_s **target_list, int target_day, int target_hour);
#endif
#ifdef RTCONFIG_CONNTRACK
#ifdef RTCONFIG_PC_SCHED_V3
extern int cleantrack_daytime_pc_list(pc_s *pc_list, int target_day, int target_hour, int target_min, int verb);
#else
extern int cleantrack_daytime_pc_list(pc_s *pc_list, int target_day, int target_hour, int verb);
#endif
#ifdef RTCONFIG_ISP_OPTUS
extern int op_cleantrack_pc_list(pc_s *pc_list, int verb);
#endif /* RTCONFIG_ISP_OPTUS */
#endif
extern void config_daytime_string(pc_s *pc_list, FILE *fp, char *lan_if, char *logaccept, char *logdrop, int temp);
extern void config_pause_block_string(pc_s *pc_list, FILE *fp, char *lan_if, char *logaccept, char *logdrop, int temp);
#ifdef RTCONFIG_PC_REWARD
extern void config_pc_reward_string(pc_s *pc_list, FILE *fp, char *lan_if);
#endif
#ifdef RTCONFIG_ISP_OPTUS
// For optus customization
#define CHAIN_OPTUS_PAUSE "OPTUS_PAUSE"
extern pc_s *op_get_all_pc_list(pc_s **pc_list);
extern void op_config_pause_block_string(pc_s *pc_list, FILE *fp, char *lan_if, char *logaccept, char *logdrop, int temp);
#endif /* RTCONFIG_ISP_OPTUS */
extern int count_pc_rules(pc_s *pc_list, int enabled);
extern int count_event_rules(pc_event_s *event_list);

void config_block_all_string(FILE *fp, char *lan_if);
#ifdef RTCONFIG_MULTILAN_CFG
void handle_sdn_block_all_string(FILE *fp);
void handle_sdn_parental_ctrl_related(int feature, pc_s *pc_list, FILE *fp, char *logaccept, char *logdrop, int temp);
int check_chilli_ip(char *ifname);
int get_ip_from_chilli(char *mac, char *ip, int ip_len);
#endif
#endif // #ifndef __PC_H__
