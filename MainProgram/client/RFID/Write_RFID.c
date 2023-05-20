#include "rfid_sensor.h"


int main(int argc, char *argv[]) {

	double set_val = 0;
 
	// must be run as root to open /dev/mem in BMC2835
    if (geteuid() != 0)
    {
        p_printf(RED,"Must be run as root.\n");
        exit(1);
    }

    // catch signals
	set_signals();
	    
    /* read /etc/rc522.conf */
    if (get_config_file()) exit(1);

    /* set BCM2835 Pins correct */
    if (HW_init(spi_speed,gpio)) close_out(1);
    
    /* initialise the RC522 */
	InitRc522();

	
    while (loop > 0)
    {
        if (NotEndless) // if not endless decrement loop counter
            p_printf(WHITE, "\nAcceptera toujours %d carte(s)\n", loop); // print loop counter
			if (uid_write()!= TAG_OK) // if card is not present or not valid
            close_out(1); 
		
		// count this card
        if (NotEndless)    loop--; // decrement loop counter
        
        // reset potential command line provided options
        set_val = 0;
        use_vblock = 0xff;

        // close / disconnect the card
        PcdHalt(); 

        p_printf(YELLOW,"Veuillez retirer la carte dans les 5 secondes\n");
            
        sleep(5);
        
    }   // loop

    close_out(0);
    
    // stop -Wall complaining
    exit(0);
}
