/*
 * Copyright (C) 2009 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <linux/ctype.h>
#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/root_dev.h>
#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/smp.h>

#include <asm/bootinfo.h>
#include <asm/pgtable-32.h>
#include <asm/tlbflush.h>
#include <asm/brcmstb/brcmstb.h>
#include <asm/r4kcache.h>
#include <asm/fw/cfe/cfe_api.h>
#include <asm/fw/cfe/cfe_error.h>

unsigned long brcm_dram0_size_mb = 0;

static u8 brcm_primary_macaddr[6] = { 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef };

unsigned long __initdata cfe_seal = 0;
unsigned long __initdata cfe_entry;
unsigned long __initdata cfe_handle;

/***********************************************************************
 * CFE bootloader queries
 ***********************************************************************/

static int __init hex(char ch)
{
	if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	if (ch >= '0' && ch <= '9')
		return ch-'0';
	if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	return -1;
}

static int __init hex16(const char *b)
{
	int d0, d1;

	d0 = hex(b[0]);
	d1 = hex(b[1]);
	if((d0 == -1) || (d1 == -1))
		return(-1);
	return((d0 << 4) | d1);
}

void __init cfe_die(char *fmt, ...)
{
	char msg[128];
	va_list ap;
	int handle;
	volatile unsigned int count;

	va_start(ap, fmt);
	vsprintf(msg, fmt, ap);
	strcat(msg, "\r\n");

	if (cfe_seal != CFE_EPTSEAL)
		goto no_cfe;

	handle = cfe_getstdhandle(CFE_STDHANDLE_CONSOLE);
	if (handle < 0)
		goto no_cfe;

	cfe_write(handle, msg, strlen(msg));

	for (count = 0; count < 0x7fffffff; count++) { }
	cfe_exit(0, 1);
	while(1) { }

no_cfe:
	/* probably won't print anywhere useful */
	printk("%s", msg);
	BUG();

	va_end(ap);
}

static inline int __init parse_eth0_hwaddr(const char *buf, u8 *out)
{
	int i, t;
	u8 addr[6];

	for(i = 0; i < 6; i++) {
		t = hex16(buf);
		if(t == -1)
			return(-1);
		addr[i] = t;
		buf += 3;
	}
	memcpy(out, addr, 6);

	return(0);
}

static inline int __init parse_ulong(const char *buf, unsigned long *val)
{
	char *endp;
	unsigned long tmp;

	tmp = simple_strtoul(buf, &endp, 0);
	if(*endp == 0) {
		*val = tmp;
		return(0);
	}
	return(-1);
}

static inline int __init parse_hex(const char *buf, unsigned long *val)
{
	char *endp;
	unsigned long tmp;

	tmp = simple_strtoul(buf, &endp, 16);
	if(*endp == 0) {
		*val = tmp;
		return(0);
	}
	return(-1);
}

static inline int __init parse_boardname(const char *buf, void *slop)
{
#if defined(CONFIG_BCM7401) || defined(CONFIG_BCM7400) || \
	defined(CONFIG_BCM7403) || defined(CONFIG_BCM7405)
	/* autodetect 97455, 97456, 97458, 97459 DOCSIS boards */
	if(strncmp("BCM9745", buf, 7) == 0)
		brcm_docsis_platform = 1;
#endif

#if defined(CONFIG_BCM7420)
	/* BCM97420CBMB */
	brcm_docsis_platform = 1;
#endif

#if defined(CONFIG_BCM7401)
	/*
	 * 7402, 7453, 7454 - no SATA
	 * 7453, 7454 disable the second USB PHY (no SW impact)
	 * 7454 disables the ENET PHY (no SW impact)
	 */
	if(strcmp("BCM97402", buf) == 0)
		brcm_sata_enabled = 0;
#endif

#if defined(CONFIG_BCM7403)
	/* 7404, 7452 - no SATA */
	if(strcmp("BCM97404", buf) == 0)
		brcm_sata_enabled = 0;
#endif

#if defined(CONFIG_BCM7405)
	/* autodetect 97405-MSG board (special MII configuration) */
	if(strstr(buf, "_MSG") != NULL)
		brcm_enet_no_mdio = 1;
#endif

	strcpy(brcm_cfe_boardname, buf);
	return(0);
}

static inline int __init parse_string(const char *buf, char *dst)
{
	strcpy(dst, buf);
	return(0);
}

static char __initdata cfe_buf[COMMAND_LINE_SIZE];

static void __init cfe_read_configuration(void)
{
	int fetched = 0;

	printk(KERN_INFO "Fetching vars from bootloader... ");
	if (cfe_seal != CFE_EPTSEAL) {
		printk("none present, using defaults.\n");
		return;
	}

#define DPRINTK(...) do { } while(0)
//#define DPRINTK(...) printk(__VA_ARGS__)

#define FETCH(name, fn, arg) do { \
	if (cfe_getenv(name, cfe_buf, COMMAND_LINE_SIZE) == CFE_OK) { \
		DPRINTK("Fetch var '%s' = '%s'\n", name, cfe_buf); \
		fn(cfe_buf, arg); \
		fetched++; \
	} else { \
		DPRINTK("Could not fetch var '%s'\n", name); \
	} \
	} while(0)

	FETCH("ETH0_HWADDR", parse_eth0_hwaddr, brcm_primary_macaddr);
	FETCH("DRAM0_SIZE", parse_ulong, &brcm_dram0_size_mb);
	FETCH("CFE_BOARDNAME", parse_boardname, NULL);
	FETCH("BOOT_FLAGS", parse_string, arcs_cmdline);

	FETCH("LINUX_FFS_STARTAD", parse_hex, &brcm_mtd_rootfs_start);
	FETCH("LINUX_FFS_SIZE", parse_hex, &brcm_mtd_rootfs_len);
	FETCH("LINUX_PART_STARTAD", parse_hex, &brcm_mtd_kernel_start);
	FETCH("LINUX_PART_SIZE", parse_hex, &brcm_mtd_kernel_len);
	FETCH("OCAP_PART_STARTAD", parse_hex, &brcm_mtd_ocap_start);
	FETCH("OCAP_PART_SIZE", parse_hex, &brcm_mtd_ocap_len);
	FETCH("FLASH_SIZE", parse_ulong, &brcm_mtd_flash_size_mb);
	FETCH("FLASH_TYPE", parse_string, brcm_mtd_flash_type);

	printk("found %d vars.\n", fetched);
}

/***********************************************************************
 * Early printk
 ***********************************************************************/

void prom_putchar(char x)
{
	/* not used */
}

static inline void __init setup_early_3250(unsigned long base_pa)
{
	extern void __init bcm3250_early_console(unsigned long);

	bcm3250_early_console(base_pa);
}

static inline void __init setup_early_16550(unsigned long base_pa)
{
	char args[64];
	extern int __init setup_early_serial8250_console(char *);

	sprintf(args, "uart,mmio,0x%08lx,115200n8", base_pa);
	setup_early_serial8250_console(args);
}

static void __init brcm_setup_early_printk(void)
{
#ifdef CONFIG_EARLY_PRINTK
	char *arg = strstr(arcs_cmdline, "console=");
	int dev = CONFIG_BRCM_CONSOLE_DEVICE;
	const unsigned long base[] = {
		BCHP_UARTA_REG_START, BCHP_UARTB_REG_START,
#ifdef CONFIG_BRCM_HAS_UARTC
		BCHP_UARTC_REG_START,
#endif
		0, 0,
	};

	/*
	 * quick command line parse to pick the early printk console
	 * valid formats:
	 *   console=ttyS0,115200
	 *   console=0,115200
	 */
	while (arg && *arg != '\0' && *arg != ' ') {
		if ((*arg >= '0') && (*arg <= '3')) {
			dev = *arg - '0';
			if (base[dev] == 0)
				dev = 0;
			break;
		}
		arg++;
	}

#if   defined(CONFIG_BRCM_HAS_3250)
	setup_early_3250(BCHP_PHYSICAL_OFFSET + base[dev]);
#elif defined(CONFIG_BRCM_HAS_16550)
	setup_early_16550(BCHP_PHYSICAL_OFFSET + base[dev]);
#endif
#endif /* CONFIG_EARLY_PRINTK */
}

/***********************************************************************
 * Main entry point
 ***********************************************************************/

void __init prom_init(void)
{
	cfe_init(cfe_handle, cfe_entry);

	bchip_check_compat();
	board_pinmux_setup();

	bchip_mips_setup();

	/* default to SATA (where available) or MTD rootfs */
#ifdef CONFIG_BRCM_HAS_SATA
	ROOT_DEV = Root_SDA1;
#else
	ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, 0);
#endif
	root_mountflags &= ~MS_RDONLY;

	bchip_set_features();

#if defined(CONFIG_BRCM_IKOS_DEBUG)
	strcpy(arcs_cmdline, "debug initcall_debug");
#elif ! defined(CONFIG_BRCM_IKOS)
	cfe_read_configuration();
#endif
	brcm_setup_early_printk();

	/* provide "ubiroot" alias to reduce typing */
	if (strstr(arcs_cmdline, "ubiroot"))
		strcat(arcs_cmdline, " ubi.mtd=rootfs rootfstype=ubifs "
			"root=ubi0:rootfs");

	printk("Options: sata=%d enet=%d emac_1=%d no_mdio=%d docsis=%d "
		"pci=%d smp=%d moca=%d\n",
		brcm_sata_enabled, brcm_enet_enabled, brcm_emac_1_enabled,
		brcm_enet_no_mdio, brcm_docsis_platform,
		brcm_pci_enabled, brcm_smp_enabled, brcm_moca_enabled);

	bchip_early_setup();

	board_get_ram_size(&brcm_dram0_size_mb);

#ifdef CONFIG_BRCM_UPPER_MEMORY
	if (brcm_dram0_size_mb > BRCM_MAX_LOWER_MB) {
		brcm_upper_tlb_setup();
		add_memory_region(0, BRCM_MAX_LOWER_MB << 20, BOOT_MEM_RAM);
		add_memory_region(UPPERMEM_START,
			(brcm_dram0_size_mb - BRCM_MAX_LOWER_MB) << 20,
			BOOT_MEM_RAM);
	} else {
		add_memory_region(0, brcm_dram0_size_mb << 20, BOOT_MEM_RAM);
	}
#else
	if(brcm_dram0_size_mb > BRCM_MAX_LOWER_MB) {
		printk("BRCM_UPPER_MEMORY is off; reducing memory to %d MB\n",
			BRCM_MAX_LOWER_MB);
		brcm_dram0_size_mb = BRCM_MAX_LOWER_MB;
	}
	add_memory_region(0, brcm_dram0_size_mb << 20, BOOT_MEM_RAM);
#endif

#ifdef CONFIG_SMP
	register_smp_ops(&brcmstb_smp_ops);
#endif
}

/***********************************************************************
 * Hooks for upper memory support and SMP TP1 boot
 ***********************************************************************/

#define UNIQUE_ENTRYHI(idx) (CKSEG0 + ((idx) << (PAGE_SHIFT + 1)))
#define ENTRYLO_CACHED(paddr) \
	(((paddr & PAGE_MASK) | \
	       (_PAGE_PRESENT | __READABLE | __WRITEABLE | _PAGE_GLOBAL | \
		_CACHE_CACHABLE_NONCOHERENT)) >> 6)

#define ENTRYLO_UNCACHED(paddr) \
	(((paddr & PAGE_MASK) | \
	       (_PAGE_PRESENT | __READABLE | __WRITEABLE | _PAGE_GLOBAL | \
		_CACHE_UNCACHED)) >> 6)

extern void tlb_init(void);
extern void build_tlb_refill_handler(void);

void brcm_tlb_init(void)
{
#ifdef CONFIG_BRCM_UPPER_MEMORY
	if (smp_processor_id() == 0) {
		tlb_init();
		add_wired_entry(ENTRYLO_CACHED(UPPERMEM_START),
			ENTRYLO_UNCACHED(UPPERMEM_START), CAC_BASE_UPPER,
			PM_256M);
	} else {
		/* bypass tlb_init() / probe_tlb() for secondary CPU */
		cpu_data[smp_processor_id()].tlbsize = cpu_data[0].tlbsize;
		build_tlb_refill_handler();
	}
#else
	tlb_init();
#endif
}

/*
 * Initialize upper memory TLB entries
 *
 * On TP1 this must happen before we set up $sp/$gp .  It is always
 * possible for stacks, task_structs, thread_info's, and other
 * important structures to be allocated out of upper memory so
 * this happens early on.
 */
asmlinkage __cpuinit void brcm_upper_tlb_setup(void)
{
#ifdef CONFIG_BRCM_UPPER_MEMORY
	int i, tlbsz;

	/* Flush TLB.  local_flush_tlb_all() is not available yet. */
	write_c0_entrylo0(0);
	write_c0_entrylo1(0);
	write_c0_pagemask(PM_DEFAULT_MASK);
	write_c0_wired(0);

	tlbsz = (read_c0_config1() >> 25) & 0x3f;
	for (i = 0; i <= tlbsz; i++) {
		write_c0_entryhi(UNIQUE_ENTRYHI(i));
		write_c0_index(i);
		mtc0_tlbw_hazard();
		tlb_write_indexed();
		tlbw_use_hazard();
	}

	/* Create cached/uncached TLB entries */
	write_c0_entrylo0(ENTRYLO_CACHED(UPPERMEM_START));
	write_c0_entrylo1(ENTRYLO_UNCACHED(UPPERMEM_START));
	write_c0_entryhi(CAC_BASE_UPPER);
	write_c0_pagemask(PM_256M);

	write_c0_index(0);
	write_c0_wired(1);
	mtc0_tlbw_hazard();
	tlb_write_indexed();
	tlbw_use_hazard();

	write_c0_pagemask(PM_DEFAULT_MASK);
#endif
}

unsigned long brcm_setup_ebase(void)
{
	unsigned long ebase = CAC_BASE;
#if defined(CONFIG_BMIPS4380) && defined(CONFIG_SMP)
	/*
	 * Exception vector configuration on BMIPS4380 SMP:
	 *
	 * a000_0000 - new BEV TP1 reset vector        (was: bfc0_0000)
	 * 8000_0400 - new !BEV TP0/TP1 exception base (was: 8000_0000)
	 *
	 * The TP1 reset vector can only be adjusted in 1MB increments, so
	 * we put it at a000_0000 and then move the runtime exception vectors
	 * up a little bit.
	 */

	unsigned long cbr = BMIPS_GET_CBR();
	DEV_WR_RB(cbr + BMIPS_RELO_VECTOR_CONTROL_0, 0x00000800);
	DEV_WR_RB(cbr + BMIPS_RELO_VECTOR_CONTROL_1, 0xa0080800);

	ebase = 0x80000400;
#elif defined(CONFIG_BMIPS5000) && defined(CONFIG_SMP)
	/*
	 * BMIPS5000 is similar to BMIPS4380, but it uses different
	 * configuration registers with different semantics:
	 *
	 * a000_0000 - new BEV TP1 reset vector        (was: bfc0_0000)
	 * 8000_1000 - new !BEV TP0/TP1 exception base (was: 8000_0000)
	 */
	ebase = 0x80001000;

	write_c0_brcm_bootvec(0xa0080000);
	write_c0_ebase(ebase);
#endif
	return ebase;
}

/***********************************************************************
 * Miscellaneous utility functions
 ***********************************************************************/

static char brcm_system_type[64];

const char *get_system_type(void)
{
	u32 class = BRCM_CHIP_ID();

	if (class >> 16 == 0)
		class >>= 8;
	else
		class >>= 12;

	snprintf(brcm_system_type, 64, "BCM%04x%02X %s platform",
		BRCM_CHIP_ID(), BRCM_CHIP_REV() + 0xa0,
		class == 0x35 ? "DTV" :
		(class == 0x76 ? "DVD" : "STB"));

        return((const char *)brcm_system_type);
}

void __init prom_free_prom_memory(void) {}

int brcm_alloc_macaddr(u8 *buf)
{
	memcpy(buf, brcm_primary_macaddr, ETH_ALEN);
	brcm_primary_macaddr[4]++;
	return 0;
}
