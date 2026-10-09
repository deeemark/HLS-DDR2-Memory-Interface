#include "ddr2_controller.h"

#define T_POWERUP  40000
#define T_CKE_WAIT 80
#define T_RCD      3
#define T_RP       3
#define T_RAS      8
#define T_RC       11
#define T_RFC      15
#define T_WR       3
#define T_WTR      2
#define T_RTP      2
#define T_CCD      2
#define T_MRD      2
#define T_DLL      200
#define T_REFI     1560
#define T_RL       3
#define T_READ_TO_READY 5
#define MR_DLL_RESET       0x0532
#define MR_NORMAL          0x0432
#define EMR1_NORMAL        0x0000
#define EMR1_OCD_DEFAULT   0x0380
#define EMR1_OCD_EXIT      0x0000
#define EMR2_VALUE         0x0000
#define EMR3_VALUE         0x0000
#define ST_POWERUP        0
#define ST_CKE_WAIT       1
#define ST_PRECHARGE1     2
#define ST_EMR2           3
#define ST_EMR3           4
#define ST_EMR1_DLL       5
#define ST_MR_DLL_RESET   6
#define ST_PRECHARGE2     7
#define ST_REFRESH1       8
#define ST_REFRESH2       9
#define ST_MR_NORMAL     10
#define ST_DLL_WAIT      11
#define ST_OCD_DEFAULT   12
#define ST_OCD_EXIT      13
#define ST_READY         14
#define ST_CAPTURE       15
#define ST_ACTIVATE      16
#define ST_RCD_WAIT      17
#define ST_WRITE_CMD     18
#define ST_WRITE_REC     19
#define ST_READ_CMD      20
#define ST_READ_WAIT     21
#define ST_READ_RESP     22
#define ST_READ_REC      23
#define ST_REFRESH_CMD   24
#define ST_REFRESH_WAIT  25
#define T_WRITE_TO_READY 10

void ddr2_controller::run()
{
    state = ST_POWERUP;
    wait_count = 0;
    dll_count = 0;
    refresh_count = 0;
    latched_rdata = 0;
    latched_write = 0;
    latched_addr = 0;
    latched_wdata = 0;
    latched_wmask = 0;
    ddr_cke.write(0); ddr_cs_n.write(1); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
    ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
    init_done.write(0); req_ready.write(0); rsp_valid.write(0); rsp_rdata.write(0);
    phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0);
    wait();
    while (1) {
        if (rst.read() == 1)
        {
            state = ST_POWERUP;
            wait_count = 0;
            dll_count = 0;
            latched_addr = 0;
            latched_wdata = 0;
            latched_wmask = 0;
        
            ddr_cke.write(0); ddr_cs_n.write(1); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
            ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
            init_done.write(0); req_ready.write(0);
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
        }
        else if (state == ST_POWERUP)
        {
            if (wait_count >= (T_POWERUP - 1))
            {
                //CKE is now active but wait T_CKE_WAIT before we can assert the next command
                state = ST_CKE_WAIT; wait_count = 0; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_POWERUP; wait_count = wait_count + 1; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(0); ddr_cs_n.write(1); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_CKE_WAIT)
        {
            if (wait_count >= (T_CKE_WAIT - 1))
            {
                // We issue the precharge command to make sure we know banks are closed
                state = ST_PRECHARGE1; wait_count = 0; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_CKE_WAIT; wait_count = wait_count + 1; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_PRECHARGE1)
        {
            //We now start configuring the DRAM
            //Program Extended mode register 2
            state = ST_EMR2; wait_count = 0; dll_count = 0;
            refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
            ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(1); ddr_we_n.write(0);
            ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0x0400);
            init_done.write(0); req_ready.write(0);
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
        }
        else if (state == ST_EMR2)
        {
            if (wait_count >= (T_RP - 1))
            {
                //Program Extended mode register 3
                state = ST_EMR3; wait_count = 0; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(0);
                ddr_odt.write(0); ddr_ba.write(2); ddr_addr.write(EMR2_VALUE);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_EMR2; wait_count = wait_count + 1; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_EMR3)
        {
            if (wait_count >= (T_MRD - 1))
            {
                // Program Extended mode register 1. Here is where DLL is enabled.
                state = ST_EMR1_DLL; wait_count = 0; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(0);
                ddr_odt.write(0); ddr_ba.write(3); ddr_addr.write(EMR3_VALUE);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_EMR3; wait_count = wait_count + 1; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_EMR1_DLL)
        {
            if (wait_count >= (T_MRD - 1))
            {
                // Now we program the main mode register where we configure normal operating parameters 
                // Important parameters such as burst length = 4 and CAS latency =3
                // We also request the DLL reset so its properly alligned.
                state = ST_MR_DLL_RESET; wait_count = 0; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(0);
                ddr_odt.write(0); ddr_ba.write(1); ddr_addr.write(EMR1_NORMAL);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_EMR1_DLL; wait_count = wait_count + 1; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_MR_DLL_RESET)
        {
            if (wait_count >= (T_MRD - 1))
            {
                //Get banks into a known state again
                state = ST_PRECHARGE2; wait_count = 0; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(0);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(MR_DLL_RESET);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_MR_DLL_RESET; wait_count = wait_count + 1; dll_count = 0;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_PRECHARGE2)
        {
            if (wait_count >= (T_MRD - 1))
            {
                //These two refreshes are required for the internal dram to be in the correct state.
                state = ST_REFRESH1; wait_count = 0; dll_count = dll_count + 1;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(1); ddr_we_n.write(0);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0x0400);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_PRECHARGE2; wait_count = wait_count + 1; dll_count = dll_count + 1;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_REFRESH1)
        {
            if (wait_count >= (T_RP - 1))
            {
                state = ST_REFRESH2; wait_count = 0; dll_count = dll_count + 1;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_REFRESH1; wait_count = wait_count + 1; dll_count = dll_count + 1;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_REFRESH2)
        {
            if (wait_count >= (T_RFC - 1))
            {
                //Program the mode register again but without the reset
                state = ST_MR_NORMAL; wait_count = 0; dll_count = dll_count + 1;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_REFRESH2; wait_count = wait_count + 1; dll_count = dll_count + 1;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_MR_NORMAL)
        {
            if (wait_count >= (T_RFC - 1))
            {
                // We wait for the DLL to settle doing nothing for T_DLL.
                state = ST_DLL_WAIT; wait_count = 0; dll_count = dll_count + 1;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(0);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(MR_NORMAL);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_MR_NORMAL; wait_count = wait_count + 1; dll_count = dll_count + 1;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_DLL_WAIT)
        {
            if ((dll_count >= T_DLL) && (wait_count >= T_MRD))
            {
                //Configure the Off-Chip Driver Calibration using Extended Mode register 1.
                state = ST_OCD_DEFAULT; wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_DLL_WAIT;
                if (wait_count < T_MRD) wait_count = wait_count + 1;
                else wait_count = wait_count;
                if (dll_count < T_DLL) dll_count = dll_count + 1;
                else dll_count = dll_count;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        else if (state == ST_OCD_DEFAULT)
        {
            state = ST_OCD_EXIT; wait_count = 0; dll_count = dll_count;
            refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
            ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(0);
            ddr_odt.write(0); ddr_ba.write(1); ddr_addr.write(EMR1_OCD_DEFAULT);
            init_done.write(0); req_ready.write(0);
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
        }
        else if (state == ST_OCD_EXIT)
        {
            if (wait_count >= (T_MRD - 1))
            {
                // We are finally in the ready state and ready to accept requests.
                state = ST_READY; wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(0); ddr_cas_n.write(0); ddr_we_n.write(0);
                ddr_odt.write(0); ddr_ba.write(1); ddr_addr.write(EMR1_OCD_EXIT);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_OCD_EXIT; wait_count = wait_count + 1; dll_count = dll_count;
                refresh_count = refresh_count; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(0); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        
        
        // READY does not touch req_addr.read()/wdata/wmask.
        // It only observes req_valid.read() and moves to CAPTURE.
        
        else if (state == ST_READY)
        {
            // In the ready state we can do 3 things:
            // 1: Automatically refresh if its time to refresh.
            // 2: Move to the capture state if it gets a request.
            // 3: Idle.
            if (refresh_count >= (T_REFI - 2))
            {
                // Refresh has priority.  This transition cycle emits an
                // explicit NOP; the AUTO REFRESH command is emitted on
                // the following cycle in ST_REFRESH_CMD.
        
                state = ST_REFRESH_CMD; wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count;
                latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
        
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
                phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else if (req_valid.read() == 1)
            {
                state = ST_CAPTURE; wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count + 1;
                latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
        
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(1);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
                phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_READY; wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(1);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        
        // CAPTURE is the ONLY state that reads req_addr.read(),
        // req_wdata.read(), and req_wmask.read().
        else if (state == ST_CAPTURE)
        {
            state = ST_ACTIVATE; wait_count = 0; dll_count = dll_count;
        
            //Only in ACTIVATE is the request latched
            latched_write = req_write.read();
            latched_addr = req_addr.read();
            latched_wdata = req_wdata.read();
            latched_wmask = req_wmask.read();
        
            ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
            ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
            init_done.write(1); req_ready.write(0);
        
            // No request data is forwarded to PHY in CAPTURE.
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
        }
        
        
         // ACTIVE uses only the captured address.
         //   row  = latched_addr[23:11]
         //   bank = latched_addr[10:9]
        else if (state == ST_ACTIVATE)
        {
            state = ST_RCD_WAIT; wait_count = 0; dll_count = dll_count;
            refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
        
            ddr_cke.write(1);
            ddr_cs_n.write(0);
            ddr_ras_n.write(0);
            ddr_cas_n.write(1);
            ddr_we_n.write(1);
            ddr_odt.write(0);
        
            //Latched address is split into row/bank/column
            //4 banks = 2 bits
            //8192 rows = 13 bits
            //512 columns =  9 bits
            ddr_ba.write((latched_addr >> 9) & 0x3);
            ddr_addr.write((latched_addr >> 11) & 0x1FFF);
        
            init_done.write(1); req_ready.write(0);
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
        }
        
        // Make sure we Wait tRCD before WRITE.
        else if (state == ST_RCD_WAIT)
        {
            //State transitions and the counter update takes a cycle so we account for that.
            if (wait_count >= (T_RCD - 2))
            {
                //if latched_write == 1 then its a write command
                // if 0 its a read 
                if (latched_write == 1) state = ST_WRITE_CMD;
                else state = ST_READ_CMD;
                wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                // Make sure to NOP while waiting
                state = ST_RCD_WAIT; wait_count = wait_count + 1; dll_count = dll_count;
                refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        
        // WRITE command with A10=1 for auto-precharge.
        // Column comes from captured address.
        // PHY data comes ONLY from latched_wdata/wmask.
        
        else if (state == ST_WRITE_CMD)
        {
            //WRITE statepath:
            //RCD_WAIT -> WRITE_CMD -> WRITE_REC -> READY
            state = ST_WRITE_REC; wait_count = 0; dll_count = dll_count;
            refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
        
            ddr_cke.write(1);
            ddr_cs_n.write(0);
            ddr_ras_n.write(1);
            ddr_cas_n.write(0);
            ddr_we_n.write(0);
            ddr_odt.write(0);
            //For Write:
            //ba = bank
            //addr = column + command options
            ddr_ba.write((latched_addr >> 9) & 0x3);
            ddr_addr.write((latched_addr & 0x01FC) | 0x0400);
        
            init_done.write(1); req_ready.write(0);
            // Starts write to PHY and passes data
            phy_wr_start.write(1);
            phy_wr_data.write(latched_wdata);
            phy_wr_mask.write(latched_wmask);
        }
        
        
        // Closed-page WRITE recovery.
        // WRITE is issued with A10=1 (auto-precharge).  
        // For BL4: write data begins WL=2 CK after WRITE,
        // burst duration is BL/2=2 CK,
        // tWR=3 CK is required before internal precharge,
        // tRP=3 CK is required before the bank is ready again.
        
        // Bring-up controller returns to READY 10 CK after WRITE.
        // Blocks all banks.
        else if (state == ST_WRITE_REC)
        {
            if (wait_count >= (T_WRITE_TO_READY - 2))
            {
                state = ST_READY; wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                // NOP till T_WRITE_TO_READY passes and the controller/DRAM is ready for another command
                state = ST_WRITE_REC; wait_count = wait_count + 1; dll_count = dll_count;
                refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        
        
        // READ command with A10=1 for auto-precharge.
        
        else if (state == ST_READ_CMD)
        {
            //READ statepath:
            //READ_CMD -> READ_WAIT -> READ_RESP -> READ_REC -> READY
            state = ST_READ_WAIT; wait_count = 0; dll_count = dll_count;
            refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
        
            ddr_cke.write(1);
            ddr_cs_n.write(0);
            ddr_ras_n.write(1);
            ddr_cas_n.write(0);
            ddr_we_n.write(1);
            ddr_odt.write(0);
            ddr_ba.write((latched_addr >> 9) & 0x3);
            ddr_addr.write((latched_addr & 0x01FC) | 0x0400);
        
            init_done.write(1); req_ready.write(0);
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
            phy_rd_start.write(1); rsp_valid.write(0); rsp_rdata.write(0);
        }
        
        
        // RL=3 for CL3/AL0.  The PHY owns DQS/DQ capture and asserts
        // phy_rd_valid.read() when the complete 64-bit BL4 payload is ready.
        // So we dont accept a request early
        
        else if (state == ST_READ_WAIT)
        {
            // Wait for minimum read latency and the complete burst is ready
            if ((wait_count >= (T_RL - 2)) && (phy_rd_valid.read() == 1))
            {
                state = ST_READ_RESP; wait_count = 0; dll_count = dll_count;
                //PLACES READ dadta in a intermediate register
                latched_rdata = phy_rd_data.read(); latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
                phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                //Issue NOPs while waitng
                state = ST_READ_WAIT; wait_count = wait_count + 1; dll_count = dll_count;
                refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
                phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        
        // One-cycle registered response to the request side.
        else if (state == ST_READ_RESP)
        {
            state = ST_READ_REC; wait_count = 0; dll_count = dll_count;
            refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
            ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
            ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
            init_done.write(1); req_ready.write(0);
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
            //Request is done and valid data is on rsp_rdata
            phy_rd_start.write(0); rsp_valid.write(1); rsp_rdata.write(latched_rdata);
        }
        
        
        //closed-page read recovery.  
        // We keep the request side blocked for 5 CK
        // from READ before returning READY.  
        // The PHY response path may extend this naturally if phy_rd_valid.read() arrives later.
        
        else if (state == ST_READ_REC)
        {
            if (wait_count >= (T_READ_TO_READY - 2))
            {
                state = ST_READY; wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
                phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                state = ST_READ_REC; wait_count = wait_count + 1; dll_count = dll_count;
                refresh_count = refresh_count + 1; latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
                phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        
        
        /*
         * Operational AUTO REFRESH.
         *
         * Refresh has priority over accepting a new request once
         * refresh_count reaches T_REFI.  Because this controller has
         * only one outstanding transaction and uses closed-page
         * auto-precharge accesses, refresh is issued only after the
         * current transaction has returned to READY.
         */
        else if (state == ST_REFRESH_CMD)
        {
            state = ST_REFRESH_WAIT; wait_count = 0; dll_count = dll_count;
            refresh_count = 0;
            latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
        
            ddr_cke.write(1);
            ddr_cs_n.write(0);
            ddr_ras_n.write(0);
            ddr_cas_n.write(0);
            ddr_we_n.write(1);
            ddr_odt.write(0);
            ddr_ba.write(0);
            ddr_addr.write(0);
        
            init_done.write(1); req_ready.write(0);
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
            phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
        }
        
        
        // tRFC(Refresh Cycle Time) = 15 CK for the selected 256Mb DDR2-400 device.
        // Using the same N-2 convention gives REFRESH-command to READY spacing of 15 physical CK.
        
        else if (state == ST_REFRESH_WAIT)
        {
            if (wait_count >= (T_RFC - 2))
            {
                state = ST_READY; wait_count = 0; dll_count = dll_count;
                refresh_count = refresh_count + 1;
                latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
        
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
                phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
            else
            {
                //NOP til refresh is complete
                state = ST_REFRESH_WAIT; wait_count = wait_count + 1; dll_count = dll_count;
                refresh_count = refresh_count + 1;
                latched_rdata = latched_rdata; latched_write = latched_write; latched_addr = latched_addr; latched_wdata = latched_wdata; latched_wmask = latched_wmask;
        
                ddr_cke.write(1); ddr_cs_n.write(0); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
                ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
                init_done.write(1); req_ready.write(0);
                phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0);
                phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
            }
        }
        
        else
        {
            state = ST_POWERUP; wait_count = 0; dll_count = 0;
            refresh_count = 0; latched_rdata = 0; latched_write = 0; latched_addr = 0; latched_wdata = 0; latched_wmask = 0;
            ddr_cke.write(0); ddr_cs_n.write(1); ddr_ras_n.write(1); ddr_cas_n.write(1); ddr_we_n.write(1);
            ddr_odt.write(0); ddr_ba.write(0); ddr_addr.write(0);
            init_done.write(0); req_ready.write(0);
            phy_wr_start.write(0); phy_wr_data.write(0); phy_wr_mask.write(0); phy_rd_start.write(0); rsp_valid.write(0); rsp_rdata.write(0);
        }
        wait();
    }
}
