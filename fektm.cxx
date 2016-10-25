/********************************************************************\
KTM frontend program
 Thomas Lindner (TRIUMF)
\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "midas.h"
#include <stdint.h>
#include <unistd.h>

// SoC includes
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include "hwlib.h"
#include "soc_cv_av/socal/socal.h"
#include "soc_cv_av/socal/hps.h"
#include "soc_cv_av/socal/alt_gpio.h"
#include "hps_0.h"
// SoC constants
#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "fektm";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms */
INT display_period = 0;

/* maximum event size produced by this frontend */
INT max_event_size =  3 * 1024 * 1024;

/* maximum event size for fragmented events (EQ_FRAGMENTED) --- not really used here */
INT max_event_size_frag = 2 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 20 * 1000000;
void **info;
char  strin[256];
HNDLE hDB, hSet;

/*-- Function declarations -----------------------------------------*/

INT frontend_init();
INT frontend_exit();
INT begin_of_run(INT run_number, char *error);
INT end_of_run(INT run_number, char *error);
INT pause_run(INT run_number, char *error);
INT resume_run(INT run_number, char *error);
INT frontend_loop();

INT read_trigger_event(char *pevent, INT off);
INT read_scaler_event(char *pevent, INT off);

/*-- Equipment list ------------------------------------------------*/

#undef USE_INT

EQUIPMENT equipment[] = {

    {"KTM",                     /* equipment name */
     {1, 0,                     /* event ID, trigger mask */
      "SYSTEM",                 /* event buffer */
      EQ_PERIODIC,              /* equipment type */
      LAM_SOURCE(0, 0xFFFFFF),  /* event source crate 0, all stations */
      "MIDAS",                  /* format */
      TRUE,                     /* enabled */
      RO_RUNNING,               /* read only when running */
      10,                     /* poll for 1000ms */
      0,                        /* stop run after this event limit */
      0,                        /* number of sub events */
      0,                        /* don't log history */
      "", "", "",},
     read_trigger_event,        /* readout routine */
    },

    {""}
};

#ifdef __cplusplus
}
#endif

/********************************************************************\
              Callback routines for system transitions
  These routines are called whenever a system transition like start/
  stop of a run occurs. The routines are called on the following
  occations:
  frontend_init:  When the frontend program is started. This routine
                  should initialize the hardware.
  frontend_exit:  When the frontend program is shut down. Can be used
                  to releas any locked resources like memory, commu-
                  nications ports etc.
  begin_of_run:   When a new run is started. Clear scalers, open
                  rungates, etc.
  end_of_run:     Called on a request to stop a run. Can send
                  end-of-run event and close run gates.
  pause_run:      When a run is paused. Should disable trigger events.
  resume_run:     When a run is resumed. Should enable trigger events.
\********************************************************************/

time_t rawtime;
struct tm * timeinfo;

void *virtual_base;
int fd;
void *h2p_lw_addr;

/*-- Frontend Init -------------------------------------------------*/
INT frontend_init()
{
    printf("Test: frontend_init\n");
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    // map the address space for the AXI BRIDGE registers into user space so we can interact with them.
    // we'll actually map in the entire CSR span of the HPS since we want to access various registers within that span
    if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
        printf( "ERROR: could not open \"/dev/mem\"...\n" );
        return( 1 );
    }
    virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

    if( virtual_base == MAP_FAILED ) {
        printf( "ERROR: mmap() failed...\n" );
        close( fd );
        return( 1 );
    }

    h2p_lw_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + HPS_DATA_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
    return SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
    // clean up our memory mapping and exit
    if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
        printf( "ERROR: munmap() failed...\n" );
        close( fd );
        return( 1 );
    }

    //	close file
    close( fd );
    return SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/
// Upon run stasrt, read ODB settings and write them to DCRC
INT begin_of_run(INT run_number, char *error)
{

    return SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error)
{
    return SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error)
{
    return SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error)
{
    return SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{
    /* if frontend_call_loop is true, this routine gets called when
      the frontend is idle or once between every event */
//    usleep(10000);
    return SUCCESS;
}

/*------------------------------------------------------------------*/

/********************************************************************\
  Readout routines for different events
\********************************************************************/

/*-- Trigger event routines ----------------------------------------*/
// Not currently used for DCRC readout
extern "C" { INT poll_event(INT source, INT count, BOOL test)
/* Polling routine for events. Returns TRUE if event
   is available. If test equals TRUE, don't return. The test
   flag is used to time the polling */
{
    int i;

    for (i = 0; i < count; i++) {
        //      cam_lam_read(LAM_SOURCE_CRATE(source), &lam);

        //      if (lam & LAM_SOURCE_STATION(source))
        if (!test)
            return 1;
    }

    usleep(1000);
    return 0;
}
}

/*-- Interrupt configuration ---------------------------------------*/
// This is not currently used by the DCRC readout
extern "C" { INT interrupt_configure(INT cmd, INT source, POINTER_T adr)
{
    switch (cmd) {
    case CMD_INTERRUPT_ENABLE:
        break;
    case CMD_INTERRUPT_DISABLE:
        break;
    case CMD_INTERRUPT_ATTACH:
        break;
    case CMD_INTERRUPT_DETACH:
        break;
    }
    return SUCCESS;
}
}


/*-- Event readout -------------------------------------------------*/
INT read_trigger_event(char *pevent, INT off)
{
    unsigned int DataIn [600];
    unsigned int triggerOffset;
    unsigned int meanOffset;
    unsigned int highVoltage;
    unsigned int lowVoltage;
    unsigned int DataOut;
    unsigned int errorType;
    int errorData;
    unsigned int errorDone;
    long int i = 0;
    int loop_count;
    int loop_count2;
    int voltage;
    int clocks;
    int state;
    float time;
    float errtime;

    printf("Test: trigger_event\n");

    loop_count = 0;
    do
    {
        // control led
        DataOut = 3;
        *(uint32_t *)h2p_lw_addr = DataOut;
        DataIn[loop_count] = *(uint32_t *)h2p_lw_addr;
        while(DataIn[loop_count]%2 == 0 && (DataIn[loop_count] != 2 || loop_count==0))
        {
            DataIn[loop_count] = *(uint32_t *)h2p_lw_addr;
        }
        DataOut = 1;
        *(uint32_t *)h2p_lw_addr = DataOut;
        loop_count++;
    } while(DataIn[loop_count-1] != 2 && DataIn[loop_count-1]!=3 && loop_count<500);

    if(DataIn[loop_count-1]==3)
    {
        DataOut = 64;
        *(uint32_t *)h2p_lw_addr = DataOut;
        DataIn[loop_count]= *(uint32_t *)h2p_lw_addr;
        while(DataIn[loop_count]%2 == 0)
        {
            DataIn[loop_count]= *(uint32_t *)h2p_lw_addr;
        }
        errorData = (DataIn[loop_count]-1)/128;
        errorType = (((DataIn[loop_count]-1)*1.0/128.0)-errorData)*64.0;
        errtime = 0.0125*errorData+0.00005;
        timeinfo = localtime ( &rawtime );

        loop_count2=0;

        while( loop_count2<(loop_count-1) ){
            clocks = DataIn[loop_count2]/16384;
            time = 0.0125*clocks+0.00005;
            voltage = ((1.0*DataIn[loop_count2])/16384.0-clocks)*4096.0;
            loop_count2++;
        }

        DataOut = 8;
        *(uint32_t *)h2p_lw_addr = DataOut;
        highVoltage = *(uint32_t *)h2p_lw_addr;
        while(highVoltage%2 == 0)

        {
            highVoltage = *(uint32_t *)h2p_lw_addr;
        }

        switch(errorType)
        {
        case 1:
            printf("Kicker Trigger signal in on state longer than it should be\n");
            break;
        case 2:
            printf("ratio of beam on vs off is too large\n");
            break;
        case 3:
            printf("Pulser valley is smaller than 45us\n");
            printf("size: %3.4f\n",errtime);
        case 4:
            printf("Pulser mean period is larger than 1.4ms\n");
            break;
        case 5:
            printf("Pulser last period was larger than 1.4ms\n");
            break;
        case 6:
            printf("Pulser mean period is smaller than 0.84ms. a period smaller than this will trip the kicker PS (400Hz)\n");
            break;
        case 7:
            printf("Pulser last period was smaller than 0.84ms. a period smaller than this will trip the kicker PS (400Hz)\n");
            break;
        case 8:
            printf("1VM4 valley is smaller than 25us.\n");
            printf("The pulser valley spacing was: %3.4f\n",errtime);
            break;
        case 9:
            printf("1VM4 last period was smaller than 0.8ms\n");
            break;
        case 10:
            printf("1VM4 last period was larger than 1.4ms\n");
            break;
        case 11:
            printf("1VM4 valley is larger than 150us\n");
            printf("HV at that time: %u \n",errorData);
            break;
        case 12:
            printf("1VM4 signal is high for longer than 1.25ms\n");
            printf("The time was: %3.4f\n",errtime);
            break;
        case 13:
            printf("Pulser valley is larger than 150us\n");
            break;
        case 14:
            printf("Pulser signal is high for longer than 1.25ms\n");
            break;
        case 15:
            printf("1VM4 and pulser signal are not in sync. Common reasons would be a connectivity problem, or a logic issue.\n");
            break;
        case 16:
            printf("error Flag. due to either PLL not being locked, or pressing the reset button (KEY[0])\n");
            break;
        case 17:
            printf("1VM4 signal is high 15us after anticipated beggining of 1VM4 valley period\n");
            break;
        default:
            printf("Could not Identify error type. For more info, go to https://github.com/nchristopher04/KTCM-TRIUMF and search for 'conditions that cause an error' In KCM_LOGIC.v\n");
        }
        DataOut = 128;
        *(uint32_t *)h2p_lw_addr = DataOut;
        errorDone = *(uint32_t *)h2p_lw_addr;
        while(errorDone != 2)
        {
            errorDone = *(uint32_t *)h2p_lw_addr;
        }
    }
    else
    {
        DataOut = 4;

        *(uint32_t *)h2p_lw_addr = DataOut;
        triggerOffset = *(uint32_t *)h2p_lw_addr;
        while(triggerOffset%2 == 0)
        {
            triggerOffset = *(uint32_t *)h2p_lw_addr;
        }

        DataOut = 16;

        *(uint32_t *)h2p_lw_addr = DataOut;
        meanOffset = *(uint32_t *)h2p_lw_addr;
        while(meanOffset%2 == 0)
        {
            meanOffset = *(uint32_t *)h2p_lw_addr;
        }

        DataOut = 8;

        *(uint32_t *)h2p_lw_addr = DataOut;
        highVoltage = *(uint32_t *)h2p_lw_addr;
        while(highVoltage%2 == 0)
        {
            highVoltage = *(uint32_t *)h2p_lw_addr;
        }

        DataOut = 32;

        *(uint32_t *)h2p_lw_addr = DataOut;
        lowVoltage = *(uint32_t *)h2p_lw_addr;
        while(lowVoltage%2 == 0)
        {
            lowVoltage = *(uint32_t *)h2p_lw_addr;
        }

        loop_count2=0;

        while( loop_count2<(loop_count-1) ){
            clocks = DataIn[loop_count2]/16384;
            time = 0.0125*clocks+0.00005;
            voltage = ((1.0*DataIn[loop_count2])/16384.0-clocks)*4096.0;
            loop_count2++;
        }
    }

    /* init bank structure */
    bk_init32(pevent);

    uint32_t *data_in;
    uint32_t *calc_value;

    /* create structured KTM bank of double words */
    bk_create(pevent, "KTM0", TID_DWORD, (void **)&data_in);
    bk_create(pevent, "KTM1", TID_DWORD, (void **)&calc_value);

    // write raw data from FPGA to 1st bank
    while( loop_count2 < (loop_count-1) ){
        *data_in++ = DataIn[loop_count2];
        loop_count2++;
    }

    // write calculated values to the 2nd bank
    *calc_value++ = triggerOffset;
    *calc_value++ = meanOffset;
    *calc_value++ = highVoltage;
    *calc_value++ = lowVoltage;

    int size2 = bk_close(pevent, data_in);
    int size3 = bk_close(pevent, calc_value);

    return bk_size(pevent);
}
