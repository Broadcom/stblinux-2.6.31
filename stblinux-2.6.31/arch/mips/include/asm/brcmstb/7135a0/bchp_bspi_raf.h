/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
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
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon May 17 03:22:33 2010
 *                 MD5 Checksum         3e283e8b2ae9d550071581bfdb7a5d89
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/

#ifndef BCHP_BSPI_RAF_H__
#define BCHP_BSPI_RAF_H__

/***************************************************************************
 *BSPI_RAF - Public Linear Read BSPI Pipe Registers
 ***************************************************************************/
#define BCHP_BSPI_RAF_START_ADDR                 0x00443100 /* Physical Starting Address Location in Flash device */
#define BCHP_BSPI_RAF_NUM_WORDS                  0x00443104 /* Number of Words to be fetched */
#define BCHP_BSPI_RAF_CTRL                       0x00443108 /* RAF Session Control Register */
#define BCHP_BSPI_RAF_FULLNESS                   0x0044310c /* Fullness indicator for the read ahead buffer */
#define BCHP_BSPI_RAF_WATERMARK                  0x00443110 /* Watermark level in the read ahead buffer that triggers an interrupt */
#define BCHP_BSPI_RAF_STATUS                     0x00443114 /* Linear Read Status Register */
#define BCHP_BSPI_RAF_READ_DATA                  0x00443118 /* Read data from Raf-buffer */
#define BCHP_BSPI_RAF_WORD_CNT                   0x0044311c /* Current number of words fetched from Flash */
#define BCHP_BSPI_RAF_CURR_ADDR                  0x00443120 /* Current read address for the linear read session */

/***************************************************************************
 *START_ADDR - Physical Starting Address Location in Flash device
 ***************************************************************************/
/* BSPI_RAF :: START_ADDR :: START_ADDR [31:00] */
#define BCHP_BSPI_RAF_START_ADDR_START_ADDR_MASK                   0xffffffff
#define BCHP_BSPI_RAF_START_ADDR_START_ADDR_SHIFT                  0

/***************************************************************************
 *NUM_WORDS - Number of Words to be fetched
 ***************************************************************************/
/* BSPI_RAF :: NUM_WORDS :: NumWords [31:00] */
#define BCHP_BSPI_RAF_NUM_WORDS_NumWords_MASK                      0xffffffff
#define BCHP_BSPI_RAF_NUM_WORDS_NumWords_SHIFT                     0

/***************************************************************************
 *CTRL - RAF Session Control Register
 ***************************************************************************/
/* BSPI_RAF :: CTRL :: reserved0 [31:02] */
#define BCHP_BSPI_RAF_CTRL_reserved0_MASK                          0xfffffffc
#define BCHP_BSPI_RAF_CTRL_reserved0_SHIFT                         2

/* BSPI_RAF :: CTRL :: CLEAR [01:01] */
#define BCHP_BSPI_RAF_CTRL_CLEAR_MASK                              0x00000002
#define BCHP_BSPI_RAF_CTRL_CLEAR_SHIFT                             1

/* BSPI_RAF :: CTRL :: START [00:00] */
#define BCHP_BSPI_RAF_CTRL_START_MASK                              0x00000001
#define BCHP_BSPI_RAF_CTRL_START_SHIFT                             0

/***************************************************************************
 *FULLNESS - Fullness indicator for the read ahead buffer
 ***************************************************************************/
/* BSPI_RAF :: FULLNESS :: reserved0 [31:07] */
#define BCHP_BSPI_RAF_FULLNESS_reserved0_MASK                      0xffffff80
#define BCHP_BSPI_RAF_FULLNESS_reserved0_SHIFT                     7

/* BSPI_RAF :: FULLNESS :: FULLNESS [06:00] */
#define BCHP_BSPI_RAF_FULLNESS_FULLNESS_MASK                       0x0000007f
#define BCHP_BSPI_RAF_FULLNESS_FULLNESS_SHIFT                      0

/***************************************************************************
 *WATERMARK - Watermark level in the read ahead buffer that triggers an interrupt
 ***************************************************************************/
/* BSPI_RAF :: WATERMARK :: reserved0 [31:02] */
#define BCHP_BSPI_RAF_WATERMARK_reserved0_MASK                     0xfffffffc
#define BCHP_BSPI_RAF_WATERMARK_reserved0_SHIFT                    2

/* BSPI_RAF :: WATERMARK :: FULLNESS_WATERMARK [01:00] */
#define BCHP_BSPI_RAF_WATERMARK_FULLNESS_WATERMARK_MASK            0x00000003
#define BCHP_BSPI_RAF_WATERMARK_FULLNESS_WATERMARK_SHIFT           0

/***************************************************************************
 *STATUS - Linear Read Status Register
 ***************************************************************************/
/* BSPI_RAF :: STATUS :: reserved0 [31:03] */
#define BCHP_BSPI_RAF_STATUS_reserved0_MASK                        0xfffffff8
#define BCHP_BSPI_RAF_STATUS_reserved0_SHIFT                       3

/* BSPI_RAF :: STATUS :: FIFO_FULL [02:02] */
#define BCHP_BSPI_RAF_STATUS_FIFO_FULL_MASK                        0x00000004
#define BCHP_BSPI_RAF_STATUS_FIFO_FULL_SHIFT                       2

/* BSPI_RAF :: STATUS :: FIFO_EMPTY [01:01] */
#define BCHP_BSPI_RAF_STATUS_FIFO_EMPTY_MASK                       0x00000002
#define BCHP_BSPI_RAF_STATUS_FIFO_EMPTY_SHIFT                      1

/* BSPI_RAF :: STATUS :: SESSION_BUSY [00:00] */
#define BCHP_BSPI_RAF_STATUS_SESSION_BUSY_MASK                     0x00000001
#define BCHP_BSPI_RAF_STATUS_SESSION_BUSY_SHIFT                    0

/***************************************************************************
 *READ_DATA - Read data from Raf-buffer
 ***************************************************************************/
/* BSPI_RAF :: READ_DATA :: DATA [31:00] */
#define BCHP_BSPI_RAF_READ_DATA_DATA_MASK                          0xffffffff
#define BCHP_BSPI_RAF_READ_DATA_DATA_SHIFT                         0

/***************************************************************************
 *WORD_CNT - Current number of words fetched from Flash
 ***************************************************************************/
/* BSPI_RAF :: WORD_CNT :: CURRENT_WORD_COUNT [31:00] */
#define BCHP_BSPI_RAF_WORD_CNT_CURRENT_WORD_COUNT_MASK             0xffffffff
#define BCHP_BSPI_RAF_WORD_CNT_CURRENT_WORD_COUNT_SHIFT            0

/***************************************************************************
 *CURR_ADDR - Current read address for the linear read session
 ***************************************************************************/
/* BSPI_RAF :: CURR_ADDR :: CURRENT_ADDRESS [31:00] */
#define BCHP_BSPI_RAF_CURR_ADDR_CURRENT_ADDRESS_MASK               0xffffffff
#define BCHP_BSPI_RAF_CURR_ADDR_CURRENT_ADDRESS_SHIFT              0

#endif /* #ifndef BCHP_BSPI_RAF_H__ */

/* End of File */
