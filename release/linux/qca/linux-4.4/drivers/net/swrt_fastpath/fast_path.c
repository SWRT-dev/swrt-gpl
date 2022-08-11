#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <net/tcp.h>
#include <linux/udp.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/swrt_fastpath/fast_path.h>
#include <linux/inetdevice.h>
#include <linux/inet.h> 

#define SUPPORT_MAX_PORT	16

static unsigned int port[SUPPORT_MAX_PORT] = {
  445,138,139,137, 	/* Samba */
  111,2049,32777,32778,32779,32780,	/* NFS */
  8200,			/* miniDLNA*/
#if 0
  80,			/* Httpd */
  22,			/* SSH */
  20,21			/* FTP*/
#endif
};

static struct proc_dir_entry *proc_nf_filter_port = NULL;
static struct proc_dir_entry *proc_lan_ip = NULL;
static unsigned int lan_ip = 0;

typedef char *PSTRING;
static PSTRING __rstrtok;
static PSTRING r_strtok(PSTRING s,const PSTRING ct)
{
	PSTRING sbegin, send;

	sbegin  = s ? s : __rstrtok;
	if (!sbegin)
	{
		return NULL;
	}

	sbegin += strspn(sbegin,ct);
	if (*sbegin == '\0')
	{
		__rstrtok = NULL;
		return( NULL );
	}

	send = strpbrk( sbegin, ct);
	if (send && *send != '\0')
		*send++ = '\0';

	__rstrtok = send;

	return (sbegin);
}

int fast_path_nf_filter_port(struct sk_buff *skb){
	struct iphdr *iph = ip_hdr(skb);
	struct tcphdr *tcph = NULL;
	struct udphdr *udph = NULL;
	int i = 0;
	int sport = 0,dport = 0;

	if(port[0] == 0)
		return 0;
	
	if(lan_ip == 0 || (iph->saddr != lan_ip && iph->daddr != lan_ip))
		return 0;
	
	switch(iph->protocol){
		case IPPROTO_TCP:
			tcph = tcp_hdr(skb);
			dport = ntohs(tcph->dest);
			sport = ntohs(tcph->source);
			break;
		case IPPROTO_UDP:
			udph = udp_hdr(skb);
			dport = ntohs(udph->dest);
			sport = ntohs(udph->source);
			break;
		default:
			return 0;
	}
	
	for(;i < SUPPORT_MAX_PORT && port[i] != 0;i++){
		if(dport == port[i] || sport == port[i]){
			//printk("t>%u=%u ",ntohs(tcph->dest),ntohs(tcph->source));
			return 1;
		}
	}
	
	return 0;
}

EXPORT_SYMBOL_GPL(fast_path_nf_filter_port);

static int fast_path_proc_read(struct seq_file *seq, void *v)
{
	int i = 0;

	for(;i < sizeof(port)/sizeof(port[0]) && port[i] != 0;i++){
		seq_printf(seq,"%u ",port[i]);
	}
	seq_putc(seq,'\n');
	return 0;
}

static ssize_t fast_path_proc_write(struct file *file, const char __user *buffer, 
		      size_t count, loff_t *data)
{
	char buf[64];
	int len = count;
	char *p = NULL,*pp = NULL;
	int i = 0;

    if (len >= sizeof(buf)){
		printk("1:Too much input.");
		return -EFAULT;
    }

    if (copy_from_user(buf, buffer, len)){
        return -EFAULT;
    }

	buf[len] = 0;
	pp = buf;

	memset(port,0,sizeof(port));
	while((p = r_strtok(pp," "))){
			port[i] = simple_strtoul(p,NULL,10);
			pp += strlen(p)+1;
			if(port[i] == 0)
				break;
			i++;
			if(i > 32){
				printk("2:Too much input.");
				return -EFAULT;
			}	
	}
	
	return len;
}

static int proc_fast_path_open(struct inode *inode, struct file *file)
{
	return single_open(file, fast_path_proc_read, NULL);
}

static ssize_t ip_proc_write(struct file *file, const char __user *buffer, 
		      size_t count, loff_t *data)
{
	int len = count;
	char buf[32] = {0};
	
	if (copy_from_user(buf, buffer, len))
        return -EFAULT;
	
	lan_ip =  in_aton(buf);
	return len;
}

static int ip_proc_read(struct seq_file *seq, void *v)
{
	unsigned char *ip = (unsigned char *)&lan_ip;

	seq_printf(seq,"%u.%u.%u.%u\n",ip[0],ip[1],ip[2],ip[3]);
	return 0;
}

static int proc_ip_open(struct inode *inode, struct file *file)
{
	return single_open(file, ip_proc_read, NULL);
}

static const struct file_operations proc_fast_path_operations = {
	.open		= proc_fast_path_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.write 		= fast_path_proc_write,
	.release	= single_release,
};

static const struct file_operations proc_ip_operations = {
	.open		= proc_ip_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.write 		= ip_proc_write,
	.release	= single_release,
};

static int __init fast_path_init (void){
	
	proc_nf_filter_port = proc_create("nas", 0, NULL, &proc_fast_path_operations);
	if(proc_nf_filter_port == NULL){
		printk("filed to create fast_path filter port proc.");
		return 0;
	}
	
	proc_lan_ip = proc_create("lan_ip", 0, NULL, &proc_ip_operations);
	if(proc_lan_ip == NULL){
		printk("filed to create lan ip proc.");
		return 0;
	}
	
	printk("Q-Path Mass Storage accelerater v1.2 \n");
	
	return 0;
}

static void __exit fast_path_exit(void){
	remove_proc_entry("nas",NULL);
	remove_proc_entry("lan_ip",NULL);
	printk("Q-Path Mass Storage accelerater exit.\n");

}

module_init(fast_path_init);
module_exit(fast_path_exit);

MODULE_LICENSE("GPL");

