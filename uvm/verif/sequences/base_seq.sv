`ifndef BASE_SEQ_SV
`define BASE_SEQ_SV

class base_seq extends uvm_sequence#(cnn_seq_item);

    `uvm_object_utils(base_seq)
    `uvm_declare_p_sequencer(cnn_sequencer)

    // ----------- CEJIC -----------
    /*
    string bias_file     =  "/home/ivan/Desktop/FAKS/4.godina/ESL/fakultetski\ git\ server/y23-g02/data/conv0_input/bias_formated.txt";
    string weights0_file =  "/home/ivan/Desktop/FAKS/4.godina/ESL/fakultetski\ git\ server/y23-g02/data/conv0_input/weights0_formated.txt";
    string image0_file   =  "/home/ivan/Desktop/FAKS/4.godina/ESL/fakultetski\ git\ server/y23-g02/data/conv0_input/picture_conv0_input.txt"; 
    string weights1_file =  "/home/ivan/Desktop/FAKS/4.godina/ESL/fakultetski\ git\ server/y23-g02/data/conv1_input/weights1_formated.txt";
    string image1_file   =  "/home/ivan/Desktop/FAKS/4.godina/ESL/fakultetski\ git\ server/y23-g02/data/conv1_input/picture_conv1_input.txt"; 
    string weights2_file =  "/home/ivan/Desktop/FAKS/4.godina/ESL/fakultetski\ git\ server/y23-g02/data/conv2_input/weights2_formated.txt";
    string image2_file   =  "/home/ivan/Desktop/FAKS/4.godina/ESL/fakultetski\ git\ server/y23-g02/data/conv2_input/picture_conv2_input.txt"; 
    */

    // ----------- MILIN -----------   
    string bias_file     =  "C:\/Users\/Ivan Milin\/Desktop\/Osmi semestar\/y23-g02\/data\/conv0_input\/bias_formated.txt";
    string weights0_file =  "C:\/Users\/Ivan Milin\/Desktop\/Osmi semestar\/y23-g02\/data\/conv0_input\/weights0_formated.txt";
    string image0_file   =  "C:\/Users\/Ivan Milin\/Desktop\/Osmi semestar\/y23-g02\/data\/conv0_input\/picture_conv0_input.txt"; 
    string weights1_file =  "C:\/Users\/Ivan Milin\/Desktop\/Osmi semestar\/y23-g02\/data\/conv1_input\/weights1_formated.txt";
    string image1_file   =  "C:\/Users\/Ivan Milin\/Desktop\/Osmi semestar\/y23-g02\/data\/conv1_input\/picture_conv1_input.txt"; 
    string weights2_file =  "C:\/Users\/Ivan Milin\/Desktop\/Osmi semestar\/y23-g02\/data\/conv2_input\/weights2_formated.txt";
    string image2_file   =  "C:\/Users\/Ivan Milin\/Desktop\/Osmi semestar\/y23-g02\/data\/conv2_input\/picture_conv2_input.txt"; 

    int fd = 0;
    int num = 0;
    real tmp;

    real bias[$];
    real weights0[$];
    real image0[$];
    real weights1[$];
    real image1[$];
    real weights2[$];
    real image2[$];

    function new(string name = "base_seq");
        super.new(name);
        extract_data();
    endfunction

    // objections are raised in pre_body
    virtual task pre_body();
        uvm_phase phase = get_starting_phase();
           
            if (phase != null)
            phase.raise_objection(this, {"Running sequence '", get_full_name(), "'"});
    endtask : pre_body

    // objections are dropped in post_body
    virtual task post_body();
        uvm_phase phase = get_starting_phase();
            
            if (phase != null)
            phase.drop_objection(this, {"Completed sequence '", get_full_name(), "'"});
    endtask : post_body

    /*
    function logic[15:0] real_to_fixed(real value, int fractional_bits);
        logic [15:0] result;
        result = value * (1<<fractional_bits);
        return result;
    endfunction
    */

    function void extract_data();
        
        //Inicijalno brisemo redove 
        //(Ako indeks nije zadat, brise se ceo red)
        bias.delete();
        weights0.delete();
        image0.delete();
        weights1.delete();
        image1.delete();
        weights2.delete();
        image2.delete();

        //---------------------- Getting numbers for biases ----------------------
        fd = $fopen(bias_file,"r");
        if(fd) begin 
            //`uvm_info(get_name(), $sformatf("Successfully OPENED bias_file"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f\n",tmp);
                bias.push_back(tmp);
            end
           
           
        // $display("First three parameters of bias.txt: '%f' '%f' '%f' ",bias[0],bias[1],bias[2]);
        //`uvm_info(get_name(),$sformatf("Number of bias parameters in queue is: %d",bias.size),UVM_HIGH)
        //`uvm_info(get_name(),$sformatf("First three parameters of bias.txt: '%f' '%f' '%f'",bias[0],bias[1],bias[2]),UVM_HIGH)
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening bias_file"),UVM_HIGH)    
        end
        $fclose(fd);
        //`uvm_info(get_name(), $sformatf("Successfully CLOSED bias_file\n"),UVM_HIGH)
    
        
        //---------------------- Getting numbers for weights 0 ----------------------
        fd = $fopen(weights0_file,"r");
        if(fd) begin
            //`uvm_info(get_name(), $sformatf("Successfully OPENED weights_file"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f\n",tmp);
                weights0.push_back(tmp);
            end
            //`uvm_info(get_name(),$sformatf("Number of weight parameters in queue is: %d",num),UVM_HIGH)
            //`uvm_info(get_name(),$sformatf("First three parameters of weights.txt: '%f' '%f' '%f'",weights[0],weights[1],weights[2]),UVM_HIGH)
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening weights_file"),UVM_HIGH) 
        end
        $fclose(fd);
        //`uvm_info(get_name(), $sformatf("Successfully CLOSED weights_file\n"),UVM_HIGH)


        //---------------------- Getting numbers for image 0 ----------------------
        fd = $fopen(image0_file,"r");
        if(fd) begin
            //`uvm_info(get_name(), $sformatf("Successfully OPENED image_file"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f\n",tmp);
                image0.push_back(tmp);
            end
            //`uvm_info(get_name(),$sformatf("Number of image parameters in queue is: %d",num),UVM_HIGH)
            //`uvm_info(get_name(),$sformatf("First three parameters of image.txt: '%f' '%f' '%f'",image[0],image[1],image[2]),UVM_HIGH)
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening image_file"),UVM_HIGH)  
        end
        $fclose(fd);
        //`uvm_info(get_name(), $sformatf("Successfully CLOSED image_file\n"),UVM_HIGH)
    
        //---------------------- Getting numbers for weights 1 ----------------------
        fd = $fopen(weights1_file,"r");
        if(fd) begin
            //`uvm_info(get_name(), $sformatf("Successfully OPENED weights_file"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f\n",tmp);
                weights1.push_back(tmp);
            end
            //`uvm_info(get_name(),$sformatf("Number of weight parameters in queue is: %d",num),UVM_HIGH)
            //`uvm_info(get_name(),$sformatf("First three parameters of weights.txt: '%f' '%f' '%f'",weights[0],weights[1],weights[2]),UVM_HIGH)
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening weights_file"),UVM_HIGH) 
        end
        $fclose(fd);
        //`uvm_info(get_name(), $sformatf("Successfully CLOSED weights_file\n"),UVM_HIGH)


        //---------------------- Getting numbers for image 1 ----------------------
        fd = $fopen(image1_file,"r");
        if(fd) begin
            //`uvm_info(get_name(), $sformatf("Successfully OPENED image_file"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f\n",tmp);
                image1.push_back(tmp);
            end
            //`uvm_info(get_name(),$sformatf("Number of image parameters in queue is: %d",num),UVM_HIGH)
            //`uvm_info(get_name(),$sformatf("First three parameters of image.txt: '%f' '%f' '%f'",image[0],image[1],image[2]),UVM_HIGH)
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening image_file"),UVM_HIGH)  
        end
        $fclose(fd);
        
        //---------------------- Getting numbers for weights 2  ----------------------
        fd = $fopen(weights2_file,"r");
        if(fd) begin
            //`uvm_info(get_name(), $sformatf("Successfully OPENED weights_file"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f",tmp);
                weights2.push_back(tmp);
            end
            //`uvm_info(get_name(),$sformatf("Number of weight parameters in queue is: %d",num),UVM_HIGH)
            //`uvm_info(get_name(),$sformatf("First three parameters of weights.txt: '%f' '%f' '%f'",weights[0],weights[1],weights[2]),UVM_HIGH)
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening weights_file"),UVM_HIGH) 
        end
        $fclose(fd);
        //`uvm_info(get_name(), $sformatf("Successfully CLOSED weights_file\n"),UVM_HIGH)


        //---------------------- Getting numbers for image 2----------------------
        fd = $fopen(image2_file,"r");
        if(fd) begin
            //`uvm_info(get_name(), $sformatf("Successfully OPENED image_file"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f",tmp);
                image2.push_back(tmp);
            end
            //`uvm_info(get_name(),$sformatf("Number of image parameters in queue is: %d",num),UVM_HIGH)
            //`uvm_info(get_name(),$sformatf("First three parameters of image.txt: '%f' '%f' '%f'",image[0],image[1],image[2]),UVM_HIGH)
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening image_file"),UVM_HIGH)  
        end
        $fclose(fd);

    endfunction

endclass : base_seq

`endif

/*
1. Relativna putanja fajlova - nisam uspeo da realizujem
2. Zasto ne mogu da printam brojeve van funkcije
3. Funkcija iz real u logic[15:0], zapis 4.12
*/
