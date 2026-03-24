/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <crypto/hash.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <linux/string.h>
#include <linux/moduleparam.h>
#include <linux/jiffies.h>
#include <linux/timex.h>
#include <linux/interrupt.h>

struct test_iter {
	char algname[48];
	int flag;
};

/* alg name to test */
struct test_iter test_array[] = {
	{
		.algname = "md5",
	},
	{
		.algname = "sha1",
	},
	{
		.algname = "sha224",
	},
	{
		.algname = "sha256",
	},
	{
		.algname = "sha512",
	},
	{
		.algname = "ecb(arc4)",
	},
	{
		.algname = "ecb(aes)",
	},
	{
		.algname = "cbc(aes)",
	},
	{
		.algname = "ofb(aes)",
	},
#if 0
	{
		.algname = "ctr(aes)",
	},
#endif
	{
		.algname = "rfc3686(ctr(aes))",
	},
	{
		.algname = "ecb(des)",
	},
	{
		.algname = "cbc(des)",
	},
	{
		.algname = "ecb(des3_ede)",
	},
	{
		.algname = "cbc(des3_ede)",
	},
	{
		.algname = "hmac(md5)",
	},
	{
		.algname = "hmac(sha1)",
	},
	{
		.algname = "hmac(sha224)",
	},
	{
		.algname = "hmac(sha256)",
	},
	{
		.algname = "hmac(sha384)",
	},
	{
		.algname = "hmac(sha512)",
	},
	{
		.algname = "authenc(hmac(sha1),cbc(aes))",
	},
	{
		.algname = "authenc(hmac(sha256),cbc(aes))"
	},
	{
		.algname = "authenc(hmac(sha512),cbc(aes))"
	},
	{
		.algname = "authenc(hmac(sha256),cbc(des3_ede))",
	},
};

static int __init ltq_test_mod(void)
{
	int i, ret = 0;

	for (i = 0; i < ARRAY_SIZE(test_array); i++) {
		pr_info("Testing alg %s in test %d\n", 
				test_array[i].algname, i);
		ret = alg_test(test_array[i].algname,
						test_array[i].algname,
						0, 0);
		if (ret) {
			pr_info("One or more test case failed\n");
			return ret;
		} 
	}

	/* So that we don't keep this module in memory */
	pr_info("Module level test passed!\n");
	return -EAGAIN;
}

static void __exit ltq_test_exit(void)
{
	return;
}

module_init(ltq_test_mod);
module_exit(ltq_test_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Quick & dirty crypto testing module");
MODULE_AUTHOR("Mohammad Firdaus B Alias Thani");
