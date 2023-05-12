#include "rfid_sensor.h"


int main(int argc, char *argv[]) {

    int     tmp;
    int     d_write = 0,d_read = 0, action=0, change=0, add_act=0;
    int     option = 0, upd_key=0;
    
    // for write  & read  message from command line
    int 	start_addr, get_bytes=0, disp_as_alpha =0;
	int		start_sector = 1, start_block = 0, got_mess=0, length = 0;
	char	*message = NULL;
	
    // for value block
    int     create_block = 0, incr_block = 0, decr_block=0, show_block=0, rem_block=0, block;
    double 	set_val = 0, value;

    while ((option = getopt(argc,argv,"ab:cdehknprstwA:B:CDG:H:IM:N:RST:V:")) != -1) {
        switch(option) {
            case 'h' : {usage(argv[0]); exit(0);}
                break;
            case 'd' : {debug = 1;p_printf(YELLOW,"Debug mode.\n");}
                break;
            case 'w' : d_write = 1;
                break;
            case 'a' : add_act = 1;
                break;
            case 's' : save_mem = 1;
                break;
            case 'k' : upd_key = 1;
                break;
            case 'r' : d_read = 1;
                break;
            case 'p' : action = 1;
                break;
            case 'c' : change = 1;
                break;
            case 't' : UpdateTrailer = 1;
                break;
            case 'n' : NoColor = 1;
                break;
            case 'e' : extra_delay = 1;
				break;
            case 'b' : 
				tmp = check_num(optarg);
				
				if (tmp == 1) // decimal number
					use_vblock = (int) strtol(optarg,NULL,10);
				
				else if (tmp == 2) // hex number
					use_vblock = (int) strtol(optarg,NULL,16);
				else
                {
					p_printf(RED,"need a correct blocknumber to use\n");
					exit(1);
				}
                break;
            case 'C' : create_block = 1;
                break;
            case 'I' : incr_block = 1;
                break;
            case 'D' : decr_block = 1;
                break;
            case 'S' : show_block = 1;
                break;
            case 'R' : rem_block = 1;
                break;
            case 'V' : 
				
                if (check_num(optarg) == 3)
                {
					p_printf(RED,"need a correct value\n");
					exit(1);
			    }
			    
			    // decimal or hex number
			    set_val = strtod(optarg,NULL);	
				break;
				
             case 'M':		// message to write (either numeric of hex)
             case 'H':
				
				if (got_mess == 1)
				{
					p_printf(RED, "Can not handle BOTH -M and -H options\n");
					exit(1);
				}
				
				got_mess = 1;	// message received
				
				// set received message
				message = set_message(optarg, option, &length);
				
				if (message == NULL)	exit(1);
				break;
			
			case 'N': 	// starting sector
			
				if (check_num(optarg) == 3)
				{
					p_printf(RED,"Starting sector: incorrect value\n");
					exit(1);
				}
								
				// decimal or hex number
				start_sector = strtod(optarg,NULL);
					
				if (start_sector < 0 || start_sector > 15)
				{
					p_printf(RED,"need a correct starting sector (between 0 and 15)\n");
					exit(1);
			    }					
				break;
				
			case 'B': 	// starting block
				
				if (check_num(optarg) == 3)
				{
					p_printf(RED,"Starting block: incorrect value\n");
					exit(1);
				}

				// decimal or hex number
				start_block = strtod(optarg,NULL);
					
				if (start_block < 0 || start_block > 2)
				{
					p_printf(RED,"need a correct starting block ( 0,1 or 2)\n");
					exit(1);
			    }	
				break;
			
			case 'A': 	// starting address
				
				if (check_num(optarg) == 3)
				{
					p_printf(RED,"Starting address: incorrect value\n");
					exit(1);
				}

				// decimal or hex number
				start_addr = strtod(optarg,NULL);
					
				if (start_addr < 0 || start_addr >= max_blocks)
				{
					p_printf(RED,"need a correct starting block (between 0 and %d\n",max_blocks-1);
					exit(1);
			    }
			    
			    // translate to sector and block
			    start_sector = start_addr / 4;
			    start_block = start_addr%4;
			    
			    if ((start_block == 3) || (! ((start_block-3) % 4)))
				{
					p_printf(RED,"Can not use a trailer as start location.\n");
					exit(1);
				} 
				break;
				
			case 'T':	// get bytes, display as readable characters
				disp_as_alpha = 1;
			case 'G': 	// Get bytes address
				
				if (check_num(optarg) == 3)
				{
					p_printf(RED,"Get: incorrect value\n");
					exit(1);
				}

				// decimal or hex number
				get_bytes = strtod(optarg,NULL);
				
				// maximum bytes is only 3 out of 4 blocks is data * 16 bytes	
				if (get_bytes < 0 || get_bytes > (max_blocks-1)/4*3*16)
				{
					p_printf(RED,"Too many bytes requested. Maximum:  %d\n",(max_blocks-1)/4*3*16);
					exit(1);
			    }
			    
			    break;
            default  : {usage(argv[0]); exit(1);}
        }
    }

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

    /* read & set GID and UID from config file */
    if (read_conf_uid()!= 0) close_out(1);

	/* if requested to write message from command line, than only perform that action */
	if (got_mess == 1) 
	{
		tmp = write_message((uint8_t *) message, start_sector, start_block, length);
		
		// free allocated memory
		if (message != NULL) free(message);
		
		if (tmp == TAG_OK)	close_out(0);
		else close_out(1);
	}

	/* if requested to read/get message from command line, than only perform that action */
	if (get_bytes != 0) 
	{
		if (get_message(start_sector, start_block, get_bytes, disp_as_alpha) == TAG_OK)	
			close_out(0);
		else 
			close_out(1);
	}
	
    while (loop > 0)
    {
        if (NotEndless)
            p_printf(WHITE, "\nWill still handle %d card(s)\n", loop);

        /* read a specific card block */
        if (d_read){ if (read_from_card() != TAG_OK) close_out(1); }
        
        /* write a specific card block */
        else if (d_write){ if(write_to_card() != TAG_OK) close_out(1); }
        
        /* perform action based on info in a specific card block */
        else if (add_act){ if (block_action()!= TAG_OK) close_out(1); }
        
        /* change access right on a specific card block */
        else if (change){ if (change_block_access()!= TAG_OK) close_out(1); }
        
        /* update a key on specific sector */
        else if (upd_key){ if (key_upd()!= TAG_OK) close_out(1);}
        
        /* read a specific card block */
        else if (create_block) {if (create_value_block(set_val)!= V_OK) close_out(1);} 
	
        /* increment specific card block */
        else if (incr_block){ if(increment_value_block()!= V_OK) close_out(1); }
        
        /* decrement specific card block */
        else if (decr_block){ if (decrement_value_block()!= V_OK) close_out(1); }
        
        /* remove specific card block */
        else if (rem_block){ if (remove_value_block() != V_OK) close_out(1); }
 
         /* show specific card block */
        else if (show_block){ if (show_value_block(&block, &value,1) != V_OK) close_out(1); }
        
        /* perform action based on UID */
        else if (action) { if (uid_action()!= TAG_OK) close_out(1); }

		// save memory of card ??
        else if (save_mem) {if (read_card_to_file()) close_out(1); }

		// display card details only
        else 
			disp_card_details(); 
		
		// count this card
        if (NotEndless)    loop--;
        
        // reset potential command line provided options
        set_val = 0;
        use_vblock = 0xff;

        // close / disconnect the card
        PcdHalt();        
       
        p_printf(YELLOW,"Please remove card within 5 seconds\n");
            
        sleep(5);
        
    }   // loop

    close_out(0);
    
    // stop -Wall complaining
    exit(0);
}

