/*---------------------------------------------------------------------------

    Copyright (c) 2001-2007 Broadcom Corporation                 /\
                                                          _     /  \     _
    _____________________________________________________/ \   /    \   / \_
                                                            \_/      \_/  

 Copyright (c) 2007 Broadcom Corporation
 All rights reserved.
 
 Redistribution and use of this software in source and binary forms, with or
 without modification, are permitted provided that the following conditions
 are met:
 
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 
 * Neither the name of Broadcom Corporation nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission of Broadcom Corporation.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

 File: pmtest.c

 Description:
 Power management sample application

    when        who         what
    -----       ---         ----
    20071030    cernekee    initial version
 ------------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <pmlib.h>

void usage(void)
{
	printf("usage: pmtest <cmd>\n");
	printf("\n");
	printf("examples:\n");
	printf("  status       show current power status\n");
	printf("  usb 0        power down USB controllers\n");
	printf("  enet 1       power up ENET controller(s)\n");
	printf("  moca 1       power up MoCA controller(s)\n");
	printf("  sata 1       power up SATA controller\n");
	printf("  tp1 0        power down TP1 (second CPU thread)\n");
	printf("  cpu 4        set CPU clock to BASE/4\n");
	printf("  pll 1        set alternate CPU PLL mode #1\n");
	printf("  ddr 64       enable DDR self-refresh after 64 idle cycles\n");
	printf("  ddr 0        disable DDR self-refresh\n");
	printf("  standby 1    enter passive standby\n");
	printf("  irw_halt 1   enter irw_halt mode\n");
	printf("\n");
	printf("options:\n");
	printf("  -d <0|1>     disable/enable DHCP on ENET\n");
	exit(1);
}

void fatal(char *str)
{
	printf("error: %s\n", str);
	exit(1);
}

int main(int argc, char **argv)
{
	struct brcm_pm_state state;
	struct brcm_pm_cfg cfg;
	void *brcm_pm_ctx;
	int val, use_dhcp = 1, ret;
	char *cmd, *arg;

	while((ret = getopt(argc, argv, "d:h")) != -1) {
		switch(ret) {
			case 'd':
				use_dhcp = atoi(optarg);
				break;
			case 'h':
			default:
				usage();
				break;
		}
	}

	if(optind >= argc)
		usage();
	cmd = argv[optind];
	optind++;

	brcm_pm_ctx = brcm_pm_init();
	if(! brcm_pm_ctx)
		fatal("can't open PM context");
	if(brcm_pm_get_status(brcm_pm_ctx, &state) != 0)
		fatal("can't get PM state");
	
	if(brcm_pm_get_cfg(brcm_pm_ctx, &cfg) != 0)
		fatal("can't get PM config");
	cfg.use_dhcp = use_dhcp;
	if(brcm_pm_set_cfg(brcm_pm_ctx, &cfg) != 0)
		fatal("can't set PM config");

	if(! strcmp(cmd, "status"))
	{
		printf("usb:          %d\n", state.usb_status);
		printf("enet:         %d\n", state.enet_status);
		printf("sata:         %d\n", state.sata_status);
		printf("tp1:          %d\n", state.tp1_status);
		printf("cpu_base:     %d\n", state.cpu_base);
		printf("cpu_divisor:  %d\n", state.cpu_divisor);
		printf("cpu_pll:      %d\n", state.cpu_pll);
		if (state.cpu_divisor != BRCM_PM_UNDEF)
			printf("cpu_speed:    %d\n",
				state.cpu_base / state.cpu_divisor);
		printf("ddr:          %d\n", state.ddr_timeout);
		return(0);
	}

	if(optind >= argc)
		usage();
	arg = argv[optind];
	optind++;
	val = atoi(arg);

	if(! strcmp(cmd, "usb"))
	{
		state.usb_status = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "enet"))
	{
		state.enet_status = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "moca"))
	{
		state.moca_status = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "sata"))
	{
		state.sata_status = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "tp1"))
	{
		state.tp1_status = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "cpu"))
	{
		state.cpu_divisor = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "ddr"))
	{
		state.ddr_timeout = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "pll"))
	{
		state.cpu_pll = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "standby"))
	{
		state.standby = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	if(! strcmp(cmd, "irw_halt"))
	{
		state.irw_halt = val;
		if(brcm_pm_set_status(brcm_pm_ctx, &state) != 0)
			fatal("can't set PM state");
		return(0);
	}

	usage();
	return(1);	/* never reached */
}